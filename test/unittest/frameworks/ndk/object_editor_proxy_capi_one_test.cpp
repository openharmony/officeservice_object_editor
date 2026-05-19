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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#include "content_embed_proxy.h"
#include "mock_object_editor_client.h"
#include "mock_object_editor_manager.h"
#include "mock_object_editor_service.h"
#include "native_object_editor_types.h"
#include "object_editor_client.h"
#include "object_editor_config.h"
#include "object_editor_err_code.h"
#include "object_editor_extension_proxy.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

ErrCode MockStopObjectEditorExtensionPermissionDenied(
    std::unique_ptr<ObjectEditorDocument> &, const sptr<IObjectEditorService> &, const bool &)
{
    return ObjectEditorManagerErrCode::SA_PERMISSION_DENIED;
}

class ObjectEditorProxyCapiOneTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorProxyCapiOneTest::SetUpTestCase() {}

void ObjectEditorProxyCapiOneTest::TearDownTestCase() {}

void ObjectEditorProxyCapiOneTest::SetUp()
{
    ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
    config.isSupportObjectEditor_.isLoaded = true;
    config.isSupportObjectEditor_.value = true;
}

void ObjectEditorProxyCapiOneTest::TearDown() {}

/**
 * @tc.name: OH_ContentEmbed_CreateContentEmbedInfo_001
 * @tc.desc: Test OH_ContentEmbed_CreateContentEmbedInfo_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_CreateContentEmbedInfo_001, TestSize.Level1)
{
    ContentEmbed_Info *info = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateContentEmbedInfo(&info);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(info, nullptr);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_CreateContentEmbedInfo_002
 * @tc.desc: Test OH_ContentEmbed_CreateContentEmbedInfo_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_CreateContentEmbedInfo_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateContentEmbedInfo(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyContentEmbedInfo_001
 * @tc.desc: Test OH_ContentEmbed_DestroyContentEmbedInfo_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_DestroyContentEmbedInfo_001, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyContentEmbedInfo(info);
    EXPECT_EQ(ret, CE_ERR_OK);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyContentEmbedInfo_002
 * @tc.desc: Test OH_ContentEmbed_DestroyContentEmbedInfo_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_DestroyContentEmbedInfo_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyContentEmbedInfo(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_GetContentEmbedInfo_001
 * @tc.desc: Test OH_ContentEmbed_GetContentEmbedInfo_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetContentEmbedInfo_001, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetContentEmbedInfo("zh_CN", info);
    EXPECT_NE(ret, CE_ERR_OK);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_GetContentEmbedInfo_002
 * @tc.desc: Test OH_ContentEmbed_GetContentEmbedInfo_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetContentEmbedInfo_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetContentEmbedInfo("zh_CN", nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_GetContentEmbedInfo_003
 * @tc.desc: Test OH_ContentEmbed_GetContentEmbedInfo_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetContentEmbedInfo_003, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    auto format = std::make_unique<ContentEmbed_Format>();
    format->oeid = "test";
    info->formats.push_back(std::move(format));
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetContentEmbedInfo(nullptr, info);
    EXPECT_EQ(ret, CE_ERR_OK);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_GetFormatCountFromInfo_001
 * @tc.desc: Test OH_ContentEmbed_GetFormatCountFromInfo_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFormatCountFromInfo_001, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    auto format = std::make_unique<ContentEmbed_Format>();
    format->oeid = "test";
    info->formats.push_back(std::move(format));
    uint32_t count = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetFormatCountFromInfo(info, &count);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_EQ(count, 1);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_GetFormatCountFromInfo_002
 * @tc.desc: Test OH_ContentEmbed_GetFormatCountFromInfo_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFormatCountFromInfo_002, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    uint32_t count = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetFormatCountFromInfo(info, &count);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_EQ(count, 0);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_GetFormatCountFromInfo_003
 * @tc.desc: Test OH_ContentEmbed_GetFormatCountFromInfo_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFormatCountFromInfo_003, TestSize.Level1)
{
    uint32_t count = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetFormatCountFromInfo(nullptr, &count);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_GetFormatCountFromInfo_004
 * @tc.desc: Test OH_ContentEmbed_GetFormatCountFromInfo_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFormatCountFromInfo_004, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetFormatCountFromInfo(info, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_GetFormatFromInfo_001
 * @tc.desc: Test OH_ContentEmbed_GetFormatFromInfo_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFormatFromInfo_001, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    auto format = std::make_unique<ContentEmbed_Format>();
    format->oeid = "test";
    info->formats.push_back(std::move(format));
    ContentEmbed_Format *outFormat = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetFormatFromInfo(info, 0, &outFormat);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(outFormat, nullptr);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_GetFormatFromInfo_002
 * @tc.desc: Test OH_ContentEmbed_GetFormatFromInfo_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFormatFromInfo_002, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    ContentEmbed_Format *outFormat = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetFormatFromInfo(info, 0, &outFormat);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_GetFormatFromInfo_003
 * @tc.desc: Test OH_ContentEmbed_GetFormatFromInfo_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFormatFromInfo_003, TestSize.Level1)
{
    ContentEmbed_Format *outFormat = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetFormatFromInfo(nullptr, 0, &outFormat);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_GetFormatFromInfo_004
 * @tc.desc: Test OH_ContentEmbed_GetFormatFromInfo_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFormatFromInfo_004, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetFormatFromInfo(info, 0, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_GetFormatFromInfo_005
 * @tc.desc: Test OH_ContentEmbed_GetFormatFromInfo_005 exception case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFormatFromInfo_005, TestSize.Level1)
{
    ContentEmbed_Info *info = new ContentEmbed_Info();
    auto format = std::make_unique<ContentEmbed_Format>();
    format->oeid = "test";
    info->formats.push_back(std::move(format));
    ContentEmbed_Format *outFormat = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetFormatFromInfo(info, 100, &outFormat);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedInfo(info);
}

/**
 * @tc.name: OH_ContentEmbed_CreateContentEmbedFormat_001
 * @tc.desc: Test OH_ContentEmbed_CreateContentEmbedFormat_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_CreateContentEmbedFormat_001, TestSize.Level1)
{
    ContentEmbed_Format *format = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateContentEmbedFormat(&format);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(format, nullptr);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_CreateContentEmbedFormat_002
 * @tc.desc: Test OH_ContentEmbed_CreateContentEmbedFormat_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_CreateContentEmbedFormat_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateContentEmbedFormat(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyContentEmbedFormat_001
 * @tc.desc: Test OH_ContentEmbed_DestroyContentEmbedFormat_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_DestroyContentEmbedFormat_001, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyContentEmbedFormat(format);
    EXPECT_EQ(ret, CE_ERR_OK);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyContentEmbedFormat_002
 * @tc.desc: Test OH_ContentEmbed_DestroyContentEmbedFormat_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_DestroyContentEmbedFormat_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyContentEmbedFormat(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_001
 * @tc.desc: Test OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_001, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale(
        "0003000C-0000-0000-C000-000000000046", "zh_CN", format);
    EXPECT_NE(ret, CE_ERR_OK);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_002
 * @tc.desc: Test OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_002, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale(nullptr, "zh_CN", format);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_003
 * @tc.desc: Test OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_003, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale(
        "0003000C-0000-0000-C000-000000000046", "zh_CN", nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_004
 * @tc.desc: Test OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale_004, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale(
        "0003000C-0000-0000-C000-000000000046", nullptr, format);
    EXPECT_NE(ret, CE_ERR_OK);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetOEidFromFormat_001
 * @tc.desc: Test OH_ContentEmbed_GetOEidFromFormat_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetOEidFromFormat_001, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    format->oeid = "0003000C-0000-0000-C000-000000000046";
    char oeid[MAX_OEID_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetOEidFromFormat(format, oeid);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_STREQ(oeid, "0003000C-0000-0000-C000-000000000046");
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetOEidFromFormat_002
 * @tc.desc: Test OH_ContentEmbed_GetOEidFromFormat_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetOEidFromFormat_002, TestSize.Level1)
{
    char oeid[MAX_OEID_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetOEidFromFormat(nullptr, oeid);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_GetOEidFromFormat_003
 * @tc.desc: Test OH_ContentEmbed_GetOEidFromFormat_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetOEidFromFormat_003, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    format->oeid = "test";
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetOEidFromFormat(format, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetNameAndDescriptionFromFormat_001
 * @tc.desc: Test OH_ContentEmbed_GetNameAndDescriptionFromFormat_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetNameAndDescriptionFromFormat_001, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    format->name = "TestFormat";
    format->description = "TestDescription";
    char name[MAX_NAME_LENGTH] = {0};
    char description[MAX_DESCRIPTION_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetNameAndDescriptionFromFormat(format, name, description);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_STREQ(name, "TestFormat");
    EXPECT_STREQ(description, "TestDescription");
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetNameAndDescriptionFromFormat_002
 * @tc.desc: Test OH_ContentEmbed_GetNameAndDescriptionFromFormat_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetNameAndDescriptionFromFormat_002, TestSize.Level1)
{
    char name[MAX_NAME_LENGTH] = {0};
    char description[MAX_DESCRIPTION_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetNameAndDescriptionFromFormat(nullptr, name, description);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_GetNameAndDescriptionFromFormat_003
 * @tc.desc: Test OH_ContentEmbed_GetNameAndDescriptionFromFormat_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetNameAndDescriptionFromFormat_003, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    format->name = "TestFormat";
    format->description = "TestDescription";
    char description[MAX_DESCRIPTION_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetNameAndDescriptionFromFormat(format, nullptr, description);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetNameAndDescriptionFromFormat_004
 * @tc.desc: Test OH_ContentEmbed_GetNameAndDescriptionFromFormat_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetNameAndDescriptionFromFormat_004, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    format->name = "TestFormat";
    format->description = "TestDescription";
    char name[MAX_NAME_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetNameAndDescriptionFromFormat(format, name, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetIconFromFormat_001
 * @tc.desc: Test OH_ContentEmbed_GetIconFromFormat_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetIconFromFormat_001, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    format->icon = nullptr;
    OH_PixelmapNative *icon = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetIconFromFormat(format, &icon);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetIconFromFormat_002
 * @tc.desc: Test OH_ContentEmbed_GetIconFromFormat_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetIconFromFormat_002, TestSize.Level1)
{
    OH_PixelmapNative *icon = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetIconFromFormat(nullptr, &icon);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_GetIconFromFormat_003
 * @tc.desc: Test OH_ContentEmbed_GetIconFromFormat_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetIconFromFormat_003, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_GetIconFromFormat(format, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetFileNameExtensionsFromFormat_001
 * @tc.desc: Test OH_ContentEmbed_GetFileNameExtensionsFromFormat_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFileNameExtensionsFromFormat_001, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    format->fileNameExtensionsCount = 0;
    format->fileNameExtensions = nullptr;
    unsigned int count = 0;
    char **extensions = OH_ContentEmbed_GetFileNameExtensionsFromFormat(format, &count);
    EXPECT_EQ(count, 0);
    EXPECT_EQ(extensions, nullptr);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_GetFileNameExtensionsFromFormat_002
 * @tc.desc: Test OH_ContentEmbed_GetFileNameExtensionsFromFormat_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFileNameExtensionsFromFormat_002, TestSize.Level1)
{
    unsigned int count = 0;
    char **extensions = OH_ContentEmbed_GetFileNameExtensionsFromFormat(nullptr, &count);
    EXPECT_EQ(extensions, nullptr);
}

/**
 * @tc.name: OH_ContentEmbed_GetFileNameExtensionsFromFormat_003
 * @tc.desc: Test OH_ContentEmbed_GetFileNameExtensionsFromFormat_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_GetFileNameExtensionsFromFormat_003, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    char **extensions = OH_ContentEmbed_GetFileNameExtensionsFromFormat(format, nullptr);
    EXPECT_EQ(extensions, nullptr);
    OH_ContentEmbed_DestroyContentEmbedFormat(format);
}

/**
 * @tc.name: OH_ContentEmbed_CreateExtensionProxy_001
 * @tc.desc: Test OH_ContentEmbed_CreateExtensionProxy_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_CreateExtensionProxy_001, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ExtensionProxy *proxy = nullptr;
    int context = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateExtensionProxy(document, &proxy, &context);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(proxy, nullptr);
    proxy->ceDocument = nullptr;
    OH_ContentEmbed_DestroyExtensionProxy(proxy);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_CreateExtensionProxy_002
 * @tc.desc: Test OH_ContentEmbed_CreateExtensionProxy_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_CreateExtensionProxy_002, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = nullptr;
    int context = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateExtensionProxy(nullptr, &proxy, &context);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_CreateExtensionProxy_003
 * @tc.desc: Test OH_ContentEmbed_CreateExtensionProxy_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_CreateExtensionProxy_003, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateExtensionProxy(document, nullptr, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_CreateExtensionProxy_004
 * @tc.desc: Test OH_ContentEmbed_CreateExtensionProxy_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_CreateExtensionProxy_004, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ExtensionProxy *proxy = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateExtensionProxy(document, &proxy, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_DestroyExtensionProxy_001
 * @tc.desc: Test OH_ContentEmbed_DestroyExtensionProxy_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_DestroyExtensionProxy_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->ceDocument = nullptr;
    proxy->objectEditorService = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyExtensionProxy(proxy);
    EXPECT_EQ(ret, CE_ERR_OK);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyExtensionProxy_002
 * @tc.desc: Test OH_ContentEmbed_DestroyExtensionProxy_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_DestroyExtensionProxy_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyExtensionProxy(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyExtensionProxy_003
 * @tc.desc: Test OH_ContentEmbed_DestroyExtensionProxy_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_DestroyExtensionProxy_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    proxy->objectEditorService = nullptr;
    proxy->deathRecipient = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyExtensionProxy(proxy);
    EXPECT_EQ(ret, CE_ERR_OK);
}

void MockOnUpdateFunc(ContentEmbed_ExtensionProxy *proxy) {}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnUpdateFunc_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnUpdateFunc_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnUpdateFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_RegisterOnUpdateFunc(proxy, MockOnUpdateFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(proxy->onUpdateFunc, nullptr);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnUpdateFunc_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnUpdateFunc_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnUpdateFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_RegisterOnUpdateFunc(nullptr, MockOnUpdateFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnUpdateFunc_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnUpdateFunc_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnUpdateFunc_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_RegisterOnUpdateFunc(proxy, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

void MockOnErrorFunc(ContentEmbed_ExtensionProxy *proxy, ContentEmbed_ErrorCode error) {}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnErrorFunc_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnErrorFunc_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnErrorFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_RegisterOnErrorFunc(proxy, MockOnErrorFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(proxy->onErrorFunc, nullptr);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnErrorFunc_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnErrorFunc_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnErrorFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_RegisterOnErrorFunc(nullptr, MockOnErrorFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnErrorFunc_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnErrorFunc_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnErrorFunc_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_RegisterOnErrorFunc(proxy, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

void MockOnEditingFinishedFunc(ContentEmbed_ExtensionProxy *proxy, bool dataModified) {}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc(proxy, MockOnEditingFinishedFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(proxy->onEditingFinishedFunc, nullptr);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc(nullptr, MockOnEditingFinishedFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc(proxy, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

void MockOnExtensionStoppedFunc(ContentEmbed_ExtensionProxy *proxy) {}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc(proxy, MockOnExtensionStoppedFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(proxy->onExtensionStoppedFunc, nullptr);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc(nullptr, MockOnExtensionStoppedFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc(proxy, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_StartWork_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_StartWork_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_StartWork_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->onUpdateFunc = MockOnUpdateFunc;
    proxy->onErrorFunc = MockOnErrorFunc;
    proxy->onEditingFinishedFunc = MockOnEditingFinishedFunc;
    proxy->onExtensionStoppedFunc = MockOnExtensionStoppedFunc;
    proxy->ceDocument = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StartWork(proxy);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_StartWork_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_StartWork_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_StartWork_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StartWork(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_StartWork_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_StartWork_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_StartWork_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->onUpdateFunc = nullptr;
    proxy->onErrorFunc = MockOnErrorFunc;
    proxy->onEditingFinishedFunc = MockOnEditingFinishedFunc;
    proxy->onExtensionStoppedFunc = MockOnExtensionStoppedFunc;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StartWork(proxy);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_StartWork_004
 * @tc.desc: Test OH_ContentEmbed_Proxy_StartWork_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_StartWork_004, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->onUpdateFunc = MockOnUpdateFunc;
    proxy->onErrorFunc = nullptr;
    proxy->onEditingFinishedFunc = MockOnEditingFinishedFunc;
    proxy->onExtensionStoppedFunc = MockOnExtensionStoppedFunc;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StartWork(proxy);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_StartWork_005
 * @tc.desc: Test OH_ContentEmbed_Proxy_StartWork_005 exception case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_StartWork_005, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->onUpdateFunc = MockOnUpdateFunc;
    proxy->onErrorFunc = MockOnErrorFunc;
    proxy->onEditingFinishedFunc = nullptr;
    proxy->onExtensionStoppedFunc = MockOnExtensionStoppedFunc;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StartWork(proxy);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_StartWork_006
 * @tc.desc: Test OH_ContentEmbed_Proxy_StartWork_006 test case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_StartWork_006, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->onUpdateFunc = MockOnUpdateFunc;
    proxy->onErrorFunc = MockOnErrorFunc;
    proxy->onEditingFinishedFunc = MockOnEditingFinishedFunc;
    proxy->onExtensionStoppedFunc = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StartWork(proxy);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_StopWork_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_StopWork_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_StopWork_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StopWork(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_StopWork_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_StopWork_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_StopWork_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->ceDocument = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StopWork(proxy);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_StopWork_004
 * @tc.desc: Test OH_ContentEmbed_Proxy_StopWork_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_StopWork_004, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StopWork(proxy);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetSnapshot_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetSnapshot_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetSnapshot_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = nullptr;
    OH_PixelmapNative *snapshot = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetSnapshot(proxy, &snapshot);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetSnapshot_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetSnapshot_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetSnapshot_002, TestSize.Level1)
{
    OH_PixelmapNative *snapshot = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetSnapshot(nullptr, &snapshot);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetSnapshot_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetSnapshot_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetSnapshot_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetSnapshot(proxy, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetSnapshot_004
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetSnapshot_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetSnapshot_004, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = nullptr;
    OH_PixelmapNative *snapshot = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetSnapshot(proxy, &snapshot);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetSnapshot_005
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetSnapshot_005 exception case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetSnapshot_005, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = nullptr;
    OH_PixelmapNative *snapshot = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetSnapshot(proxy, &snapshot);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_DoEdit_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_DoEdit_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_DoEdit_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_DoEdit(proxy);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_DoEdit_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_DoEdit_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_DoEdit_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_DoEdit(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_DoEdit_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_DoEdit_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_DoEdit_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_DoEdit(proxy);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_DoEdit_004
 * @tc.desc: Test OH_ContentEmbed_Proxy_DoEdit_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_DoEdit_004, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_DoEdit(proxy);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetEditStatus_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetEditStatus_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetEditStatus_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = nullptr;
    bool isEditing = false;
    bool isModified = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetEditStatus(proxy, &isEditing, &isModified);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetEditStatus_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetEditStatus_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetEditStatus_002, TestSize.Level1)
{
    bool isEditing = false;
    bool isModified = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetEditStatus(nullptr, &isEditing, &isModified);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetEditStatus_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetEditStatus_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetEditStatus_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    bool isModified = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetEditStatus(proxy, nullptr, &isModified);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetEditStatus_004
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetEditStatus_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetEditStatus_004, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    bool isEditing = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetEditStatus(proxy, &isEditing, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetEditStatus_005
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetEditStatus_005 exception case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetEditStatus_005, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = nullptr;
    bool isEditing = false;
    bool isModified = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetEditStatus(proxy, &isEditing, &isModified);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetCapability_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetCapability_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetCapability_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = nullptr;
    uint32_t bitmask = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetCapability(proxy, &bitmask);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetCapability_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetCapability_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetCapability_002, TestSize.Level1)
{
    uint32_t bitmask = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetCapability(nullptr, &bitmask);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetCapability_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetCapability_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetCapability_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetCapability(proxy, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetCapability_004
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetCapability_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetCapability_004, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->objectEditorService = nullptr;
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = nullptr;
    uint32_t bitmask = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetCapability(proxy, &bitmask);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetDocument_001
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetDocument_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetDocument_001, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_Document *ceDocument = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetDocument(proxy, &ceDocument);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(ceDocument, nullptr);
    proxy->ceDocument = nullptr;
    delete ceDocument;
    OH_ContentEmbed_DestroyExtensionProxy(proxy);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetDocument_002
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetDocument_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetDocument_002, TestSize.Level1)
{
    ContentEmbed_Document *ceDocument = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetDocument(nullptr, &ceDocument);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetDocument_003
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetDocument_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetDocument_003, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    ContentEmbed_Document *ceDocument = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetDocument(proxy, &ceDocument);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy;
}

/**
 * @tc.name: OH_ContentEmbed_Proxy_GetDocument_004
 * @tc.desc: Test OH_ContentEmbed_Proxy_GetDocument_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OH_ContentEmbed_Proxy_GetDocument_004, TestSize.Level1)
{
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_GetDocument(proxy, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete proxy->ceDocument;
    delete proxy;
}

/**
 * @tc.name: ContentEmbed_Format_Build_001
 * @tc.desc: Test ContentEmbed_Format_Build_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, ContentEmbed_Format_Build_001, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    format->oeid = "test_oeid";
    format->locale = "zh_CN";
    format->formatName = "test_name";
    format->description = "test_description";
    format->fileExts = "doc,xls,ppt";
    ContentEmbed_Format embedFormat;
    embedFormat.Build(format);
    EXPECT_EQ(embedFormat.oeid, "test_oeid");
    EXPECT_EQ(embedFormat.locale, "zh_CN");
    EXPECT_EQ(embedFormat.name, "test_name");
    EXPECT_EQ(embedFormat.description, "test_description");
}

/**
 * @tc.name: ContentEmbed_Info_Build_001
 * @tc.desc: Test ContentEmbed_Info_Build_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, ContentEmbed_Info_Build_001, TestSize.Level1)
{
    std::vector<std::unique_ptr<ObjectEditorFormat>> oeFormats;
    auto format1 = std::make_unique<ObjectEditorFormat>();
    format1->oeid = "oeid1";
    oeFormats.push_back(std::move(format1));
    auto format2 = std::make_unique<ObjectEditorFormat>();
    format2->oeid = "oeid2";
    oeFormats.push_back(std::move(format2));
    ContentEmbed_Info info;
    info.Build(oeFormats);
    EXPECT_EQ(info.formats.size(), 2);
}

/**
 * @tc.name: ContentEmbed_Format_Destructor_001
 * @tc.desc: Test ContentEmbed_Format_Destructor_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, ContentEmbed_Format_Destructor_001, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    format->fileNameExtensions = nullptr;
    format->fileNameExtensionsCount = 0;
    delete format;
}

/**
 * @tc.name: ContentEmbed_Format_Destructor_002
 * @tc.desc: Test ContentEmbed_Format_Destructor_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, ContentEmbed_Format_Destructor_002, TestSize.Level1)
{
    ContentEmbed_Format *format = new ContentEmbed_Format();
    format->fileNameExtensionsCount = 2;
    format->fileNameExtensions = new char*[2];
    format->fileNameExtensions[0] = new char[5];
    strcpy_s(format->fileNameExtensions[0], 5, "doc");
    format->fileNameExtensions[1] = new char[5];
    strcpy_s(format->fileNameExtensions[1], 5, "xls");
    delete format;
}

} // namespace ObjectEditor
} // namespace OHOS
