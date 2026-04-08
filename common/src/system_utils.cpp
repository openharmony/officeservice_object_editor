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

#include "system_utils.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <regex>
#include <sstream>
#include <sys/stat.h>
#include "file_uri.h"

#include "hilog_object_editor.h"

namespace OHOS {
namespace ObjectEditor {
namespace SystemUtils {

namespace {
constexpr int32_t HOURS_OF_ONE_DAY = 24;
constexpr int32_t BEIJING_TIME_ZONE = 8;
constexpr int32_t NUMBER_BASE = 10;
constexpr int32_t TIME_FORMAT_SIZE = 64;
const std::string FILE_MANAGER_AUTHORITY = "docs";
const std::string MEDIA_AUTHORITY = "media";
} // namespace

// LCOV_EXCL_START
uint64_t GetFileSize(const std::string &filePath)
{
    if (filePath.empty() || filePath.find("..") != std::string::npos || filePath.find("/") == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "File path is invalid");
        return 0;
    }
    uint64_t fileSize = 0;
    struct stat statbuf;
    if (stat(filePath.c_str(), &statbuf) == 0) {
        fileSize = static_cast<uint64_t>(statbuf.st_size);
    } else {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Get file size failed, errno: %{public}d", errno);
    }
    return fileSize;
}

std::string ReadFile(const std::string &filePath)
{
    std::filesystem::path path(filePath);
    std::string directory = path.parent_path().string() + "/";
    std::string filename = path.filename().string();

    char *canonicalDirPath = realpath(directory.c_str(), nullptr);
    if (canonicalDirPath == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "canonical directory path is null");
        return "";
    }

    std::string canonicalFilePath = std::string(canonicalDirPath) + "/" + filename;
    free(canonicalDirPath);
    canonicalDirPath = nullptr;

    std::ifstream in(canonicalFilePath, std::ios::in | std::ios::binary);
    if (!in.is_open()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Open file failed, path: %{private}s",
            canonicalFilePath.c_str());
        return "";
    }
    std::stringstream inStr;
    inStr << in.rdbuf();
    in.close();
    return inStr.str();
}

std::vector<std::string> SplitString(const std::string &str, char pattern)
{
    std::stringstream iss(str);
    std::vector<std::string> result;
    std::string token;
    while (getline(iss, token, pattern)) {
        result.emplace_back(token);
    }
    return result;
}

void TrimString(std::string &inputStr)
{
    if (inputStr.empty()) {
        return;
    }
    inputStr.erase(inputStr.begin(), std::find_if(inputStr.begin(), inputStr.end(),
        [](unsigned char ch) {return !std::isspace(ch); }));
    inputStr.erase(std::find_if(inputStr.rbegin(), inputStr.rend(),
        [](unsigned char ch) {return !std::isspace(ch); }).base(), inputStr.end());
}

bool StringToULong(const char *input, unsigned long &num)
{
    if (input == nullptr) {
        return false;
    }
    char *endPtr = nullptr;
    unsigned long result = 0;
    errno = 0;
    result = strtoul(input, &endPtr, NUMBER_BASE);
    if (errno == ERANGE || endPtr == input || *endPtr != '\0') {
        return false;
    }
    num = result;
    return true;
}

bool StringToLong(const char *input, long &num)
{
    if (input == nullptr) {
        return false;
    }
    char *endPtr = nullptr;
    long result = 0;
    errno = 0;
    result = strtol(input, &endPtr, NUMBER_BASE);
    if (errno == ERANGE || endPtr == input || *endPtr != '\0') {
        return false;
    }
    num = result;
    return true;
}

bool StringToFloat(const char *input, float &num)
{
    char *endPtr = nullptr;
    if (input == nullptr) {
        return false;
    }
    errno = 0;
    float result = strtof(input, &endPtr);
    if (input == endPtr || errno == ERANGE || *endPtr != '\0') {
        return false;
    }
    num = result;
    return true;
}

bool StringToInt(const char *input, int &num)
{
    if (input == nullptr) {
        return false;
    }
    long numLong = 0;
    bool ret = StringToLong(input, numLong);
    if (!ret) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "StringToLong failed, input: %{private}s", input);
        return false;
    }
    if (numLong < INT_MIN || numLong > INT_MAX) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "StringToInt failed, input: %{private}s", input);
        return false;
    }
    num = static_cast<int>(numLong);
    return true;
}

bool HasSQLWildcard(const std::string &str)
{
    return str.find('%') != std::string::npos || str.find('_') != std::string::npos;
}

bool IsValidFileExt(const std::string &fileExt)
{
    return std::all_of(fileExt.begin(), fileExt.end(), [](char ch) {
        return std::isalnum(ch) || ch == '.';
    });
}

bool FileExtsHasFileExt(const std::string &fileExts, const std::string &fileExt)
{
    if (fileExts.empty()) {
        return false;
    }
    std::vector<std::string> extList = SplitString(fileExts, '|');
    if (std::find(extList.begin(), extList.end(), fileExt) != extList.end()) {
        return true;
    }
    return false;
}

std::string UTCToBeijingTime(int64_t utcTime)
{
    if (utcTime == 0) {
        return "";
    }
    time_t timeValue = static_cast<time_t>(utcTime);
    tm *timePtr = gmtime(&timeValue);
    if (timePtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "gmtime failed, utcTime: %{private}s", ctime(&timeValue));
        return "";
    }
    timePtr->tm_hour += BEIJING_TIME_ZONE;
    if (timePtr->tm_hour >= HOURS_OF_ONE_DAY) {
        timePtr->tm_hour -= HOURS_OF_ONE_DAY;
        timePtr->tm_mday++;
    }
    char buffer[TIME_FORMAT_SIZE] = {0x00};
    if (strftime(buffer, TIME_FORMAT_SIZE, "%Y-%m-%d %H:%M:%S", timePtr) == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "strftime failed, utcTime: %{private}s", ctime(&timeValue));
        return "";
    }
    return std::string(buffer);
}

std::string GetFileSuffix(const std::string &filePath)
{
    if (filePath.empty()) {
        return "";
    }
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    std::string fileType = filePath.substr(dotPos);
    if (fileType.empty()) {
        return "";
    }
    std::transform(fileType.begin(), fileType.end(),
        fileType.begin(), [](char c) { return static_cast<char>(std::tolower(c)); });
    return fileType;
}

std::string GetUriFromPath(const std::string &path)
{
    AppFileService::ModuleFileUri::FileUri fileUri(path);
    return fileUri.ToString();
}

std::string GetPathFromUri(const std::string &uri)
{
    return AppFileService::ModuleFileUri::FileUri(uri).GetRealPath();
}

bool IsAppSandboxPath(const std::string &path)
{
    Uri uri(SystemUtils::GetUriFromPath(path));
    std::string authority = uri.GetAuthority();
    return authority != FILE_MANAGER_AUTHORITY && authority != MEDIA_AUTHORITY;
}

std::string GetSubstrByPrefix(const std::string &str, const std::string &prefix)
{
    size_t pos = str.rfind(prefix);
    if ((pos == std::string::npos) || (pos == str.size() - prefix.size())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "find value fail, str:%{private}s, prefix:%{private}s",
            str.c_str(), prefix.c_str());
        return "";
    }
    return str.substr(pos + prefix.size());
}

bool GetIntByPrefix(const std::string &str, const std::string &prefix, int &num)
{
    std::string strValue = GetSubstrByPrefix(str, prefix);
    if (!StringToInt(strValue.c_str(), num)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "StringToInt fail, str:%{private}s, prefix:%{private}s",
            str.c_str(), prefix.c_str());
        return false;
    }
    return true;
}
// LCOV_EXCL_STOP
} // namespace SystemUtils
} // namespace ObjectEditor
} // namespace OHOS