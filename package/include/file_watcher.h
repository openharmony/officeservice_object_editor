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

#ifndef OHOS_OBJECT_EDITOR_PACKAGE_FILE_WATCHER_H
#define OHOS_OBJECT_EDITOR_PACKAGE_FILE_WATCHER_H

#include <atomic>
#include <cstdint>
#include <functional>
#include <pthread.h>
#include <string>
#include <unistd.h>

namespace OHOS {
namespace ObjectEditor {

using EventCallback = std::function<void(uint32_t, const std::string&)>;

class FileWatcher {
public:
    FileWatcher(const std::string &filepath, EventCallback callback)
        : filepath_(filepath), callback_(callback) {}
    ~FileWatcher()
    {
        Stop();
    }
    bool Start();
    void Stop();

private:
    FileWatcher(const FileWatcher&) = delete;
    FileWatcher& operator=(const FileWatcher&) = delete;

    static void* ThreadFuncStatic(void* arg);
    void CleanupResources();
    void WatchLoop();
    bool WaitForEvent();
    ssize_t ReadEvents(char *buffer, size_t bufferSize);
    void HandleReadError();
    void ProcessEvent(char *buffer, ssize_t len);
    int32_t inotifyFd_ = -1;
    int32_t watchDescriptor_ = -1;
    const std::string filepath_;
    const EventCallback callback_;
    pthread_t watchThread_;
    std::atomic<bool> running_ {false};
    std::atomic<bool> threadCreated_ {false};
    std::mutex mutex_;
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_PACKAGE_FILE_WATCHER_H
