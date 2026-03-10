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
#ifndef OHOS_OBJECT_EDITOR_SYSTEM_UTILS_H
#define OHOS_OBJECT_EDITOR_SYSTEM_UTILS_H

#include <string>
#include <vector>

namespace OHOS {
namespace ObjectEditor {
namespace SystemUtils {
uint64_t GetFileSize(const std::string &filePath);
std::string GetFileSuffix(const std::string &filePath);
std::string ReadFile(const std::string &filePath);
std::vector<std::string> SplitString(const std::string &str, char pattern);
void TrimString(std::string &inputStr);
bool StringToULong(const char *input, unsigned long &num);
bool StringToLong(const char *input, long &num);
bool StringToInt(const char *input, int &num);
bool StringToFloat(const char *input, float &num);
bool HasSQLWildcard(const std::string &str);
/* fileExt仅允许包含字母、数字或'.' */
bool IsValidFileExt(const std::string &fileExt);
bool FileExtsHasFileExt(const std::string &fileExts, const std::string &fileExt);
std::string UTCToBeijingTime(int64_t timestamp);
std::string GetUriFromPath(const std::string &path);
std::string GetPathFromUri(const std::string &uri);
/* str形如 "prefixabcdef"，返回"abcdef"*/
std::string GetSubstrByPrefix(const std::string &str, const std::string &prefix);
/* str形如 "prefix123456"，num返回"123456"*/
bool GetIntByPrefix(const std::string &str, const std::string &prefix, int &num);
} // namespace SystemUtils
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_SYSTEM_UTILS_H