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

/**
 * @brief Resolves the canonical absolute path for a given path.
 * Uses PATH_MAX fixed-size buffer to avoid dynamic allocation.
 * @param path The file or directory path to resolve.
 * @return The resolved canonical path string, or empty string on failure.
 */
std::string GetRealPath(const std::string &path);

/**
 * @brief Gets the file size in bytes.
 * Rejects paths containing "..", "~", "\\" or starting with "/".
 * @param filePath The path to the file.
 * @return File size in bytes, or 0 if path is invalid or stat fails.
 */
uint64_t GetFileSize(const std::string &filePath);

/**
 * @brief Extracts the file suffix (extension) from a file path, converted to lowercase.
 * Includes the dot, e.g. ".txt".
 * @param filePath The file path.
 * @return The lowercase suffix string, or empty string if no dot found.
 */
std::string GetFileSuffix(const std::string &filePath);

/**
 * @brief Reads the entire content of a file into a string.
 * Resolves the parent directory via GetRealPath for path safety.
 * @param filePath The path to the file.
 * @return The file content string, or empty string on failure.
 */
std::string ReadFile(const std::string &filePath);

/**
 * @brief Splits a string into substrings by a delimiter character.
 * @param str The input string to split.
 * @param pattern The delimiter character.
 * @return A vector of substrings.
 */
std::vector<std::string> SplitString(const std::string &str, char pattern);

/**
 * @brief Removes leading and trailing whitespace from a string.
 * @param inputStr The string to trim, modified in place.
 */
void TrimString(std::string &inputStr);

/**
 * @brief Converts a decimal string to unsigned long.
 * Validates no overflow (ERANGE), no trailing characters, and input is not null.
 * @param input The decimal string to convert.
 * @param num Output parameter for the converted value.
 * @return True if conversion succeeded, false otherwise.
 */
bool StringToULong(const char *input, unsigned long &num);

/**
 * @brief Converts a decimal string to long.
 * Validates no overflow (ERANGE), no trailing characters, and input is not null.
 * @param input The decimal string to convert.
 * @param num Output parameter for the converted value.
 * @return True if conversion succeeded, false otherwise.
 */
bool StringToLong(const char *input, long &num);

/**
 * @brief Converts a decimal string to int.
 * First converts to long, then checks range within [INT_MIN, INT_MAX].
 * @param input The decimal string to convert.
 * @param num Output parameter for the converted value.
 * @return True if conversion succeeded, false otherwise.
 */
bool StringToInt(const char *input, int &num);

/**
 * @brief Converts a string to float using strtof.
 * Validates no overflow (ERANGE), no trailing characters, result is finite (not inf/NaN),
 * and input is not null.
 * @param input The string to convert.
 * @param num Output parameter for the converted value.
 * @return True if conversion succeeded, false otherwise.
 */
bool StringToFloat(const char *input, float &num);

/**
 * @brief Checks whether a string contains SQL wildcard characters ('%' or '_').
 * @param str The string to check.
 * @return True if the string contains '%' or '_', false otherwise.
 */
bool HasSQLWildcard(const std::string &str);

/**
 * @brief Checks whether a file extension string contains only valid characters.
 * Valid characters are letters, digits, and '.'.
 * @param fileExt The file extension string to validate.
 * @return True if all characters are valid, false otherwise.
 */
bool IsValidFileExt(const std::string &fileExt);

/**
 * @brief Checks whether a pipe-delimited extension list contains a specific extension.
 * @param fileExts The pipe-delimited extension list (e.g. "doc|xls|ppt").
 * @param fileExt The extension to search for (e.g. "doc").
 * @return True if fileExt is found in fileExts, false otherwise.
 */
bool FileExtsHasFileExt(const std::string &fileExts, const std::string &fileExt);

/**
 * @brief Converts a UTC timestamp (seconds since epoch) to Beijing time string.
 * Format: "YYYY-MM-DD HH:MM:SS".
 * @param utcTime The UTC timestamp in seconds.
 * @return The formatted Beijing time string, or empty string if utcTime is 0 or conversion fails.
 */
std::string UTCToBeijingTime(int64_t timestamp);

/**
 * @brief Converts a file path to a URI string using FileUri.
 * @param path The file path.
 * @return The URI string.
 */
std::string GetUriFromPath(const std::string &path);

/**
 * @brief Converts a URI string to a real file path using FileUri.
 * @param uri The URI string.
 * @return The real file path string.
 */
std::string GetPathFromUri(const std::string &uri);

/**
 * @brief Checks whether a path is an app sandbox path.
 * A sandbox path has authority neither "docs" nor "media".
 * @param path The file path to check.
 * @return True if the path is an app sandbox path, false otherwise.
 */
bool IsAppSandboxPath(const std::string &path);

/**
 * @brief Extracts the substring after a given prefix.
 * @param str The input string, e.g. "prefixabcdef".
 * @param prefix The prefix to search for, e.g. "prefix".
 * @return The substring after the prefix, e.g. "abcdef", or empty string if prefix not found.
 */
std::string GetSubstrByPrefix(const std::string &str, const std::string &prefix);

/**
 * @brief Extracts an integer value after a given prefix and converts it.
 * @param str The input string, e.g. "prefix123456".
 * @param prefix The prefix to search for, e.g. "prefix".
 * @param num Output parameter for the parsed integer value.
 * @return True if prefix found and integer conversion succeeded, false otherwise.
 */
bool GetIntByPrefix(const std::string &str, const std::string &prefix, int &num);
/**
 * @brief Validates a path and normalizes it to its canonical form.
 * Resolves the parent directory via realpath and combines it with the filename.
 * Rejects empty paths or paths whose parent directory cannot be resolved.
 * @param path The input file path to validate and normalize.
 * @param canonicalFileName Output parameter for the canonical path string.
 * @return True if the path is valid and normalization succeeded, false otherwise.
 */
bool ValidateAndNormalizePath(const std::string &path, std::string &canonicalFileName);

/**
 * @brief Gets the bundle name of the current application.
 * Retrieves the bundle name from the application context.
 * @return The bundle name string, or empty string if the application context is null.
 */
std::string GetBundleName();
} // namespace SystemUtils
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_SYSTEM_UTILS_H