/*
 * Copyright (c) Huawei Device Co., Ltd. 2026-2026. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/inotify.h>
#include <sys/select.h>
#include <iostream>

#include "file_watcher.h"
#include "hilog_object_editor.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
namespace {
constexpr int32_t KB = 1024;
constexpr int32_t EVENT_SIZE_OFFSET = 16;
}

bool FileWatcher::Start()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "in");
    if (running_.load()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "watcher is running");
        return false;
    }
    if (callback_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "callback is nullptr");
        return false;
    }
    {
        std::lock_guard<std::mutex> lock(mutex_);
        inotifyFd_ = inotify_init1(IN_NONBLOCK);
        if (inotifyFd_ < 0) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "create inotify fd failed: %{public}s", strerror(errno));
            return false;
        }
        watchDescriptor_ = inotify_add_watch(inotifyFd_, filepath_.c_str(),
            IN_MODIFY | IN_ATTRIB | IN_MOVE_SELF | IN_DELETE_SELF | IN_CLOSE_WRITE);
        if (watchDescriptor_ < 0) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "add watch failed: %{public}s", strerror(errno));
            close(inotifyFd_);
            inotifyFd_ = -1;
            return false;
        }
    }
    running_.store(true);
    int32_t result = pthread_create(&watchThread_, nullptr, FileWatcher::ThreadFuncStatic, this);
    if (result != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "create thread failed: %{public}d", result);
        CleanupResources();
        return false;
    }
    threadCreated_.store(true);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "start watch file: %{private}s", filepath_.c_str());
    return true;
}

void FileWatcher::Stop()
{
    if (!running_.load() || !threadCreated_.load()) {
        CleanupResources();
        return;
    }
    running_.store(false);
    pthread_join(watchThread_, nullptr);
    threadCreated_.store(false);
    CleanupResources();
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "stop watch file: %{private}s", filepath_.c_str());
}

void* FileWatcher::ThreadFuncStatic(void *arg)
{
    FileWatcher* self = static_cast<FileWatcher*>(arg);
    self->WatchLoop();
    return nullptr;
}

void FileWatcher::CleanupResources()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (inotifyFd_ >= 0 && watchDescriptor_ >= 0) {
        inotify_rm_watch(inotifyFd_, watchDescriptor_);
        watchDescriptor_ = -1;
    }
    if (inotifyFd_ >= 0) {
        close(inotifyFd_);
        inotifyFd_ = -1;
    }
    running_.store(false);
}

void FileWatcher::WatchLoop()
{
    constexpr size_t EVENT_SIZE = sizeof(struct inotify_event);
    constexpr size_t BUFFER_SIZE = KB * (EVENT_SIZE + EVENT_SIZE_OFFSET);
    char buffer[BUFFER_SIZE] = {0};
    while (running_.load()) {
        ssize_t len = ReadEvents(buffer, BUFFER_SIZE);
        if (len <= 0) {
            continue;
        }
        ProcessEvent(buffer, len);
    }
}

ssize_t FileWatcher::ReadEvents(char *buffer, size_t bufferSize)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (inotifyFd_ < 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "inotify fd not initialized");
        return -1;
    }
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(inotifyFd_, &fds);

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int32_t ret = select(inotifyFd_ + 1, &fds, nullptr, nullptr, &timeout);
    if (ret < 0) {
        if (errno == EINTR) {
            return ret;
        }
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "select failed: %{public}s", strerror(errno));
        Stop();
        return ret;
    }
    if (ret == 0) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "no event");
        return ret;
    }
    if (!FD_ISSET(inotifyFd_, &fds)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "inotify fd not set");
        return -1;
    }
    ssize_t len = read(inotifyFd_, buffer, bufferSize);
    if (len < 0) {
        HandleReadError();
    } else if (len == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "inotify fd closed");
        Stop();
    }
    return len;
}

void FileWatcher::HandleReadError()
{
    switch (errno) {
        case EINTR:
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "read interrupted");
        case EAGAIN:
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "not data available");
            break;
        default:
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "read error: %{public}s", strerror(errno));
            Stop();
            break;
    }
}

bool FileWatcher::WaitForEvent()
{
    return false;
}

void FileWatcher::ProcessEvent(char *buffer, ssize_t len)
{
    char* end = buffer + len;
    char* ptr = buffer;
    while (ptr < end) {
        if (ptr + sizeof(struct inotify_event) > end) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "insufficient data");
            Stop();
            return;
        }
        struct inotify_event* event = reinterpret_cast<struct inotify_event*>(ptr);
        if (event == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "invalid event");
            Stop();
            return;
        }
        size_t event_size = sizeof(struct inotify_event) + event->len;
        if (ptr + event_size > end) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "event length exceeds buffer");
            Stop();
            return;
        }
        if (event->mask & (IN_DELETE_SELF | IN_MOVE_SELF)) {
            OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "file moved or deleted");
            Stop();
            return;
        }
        if (callback_ != nullptr) {
            callback_(event->mask, filepath_);
        }
        ptr += event_size;
    }
}

// LCOV_EXCL_END
} // namespace ObjectEditor
} // namespace OHOS
