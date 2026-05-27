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
#define final

#define private public
#include "content_embed_document.h"
#include "mock_object_editor_document.h"
#undef final
#include "mock_storage.h"
#include "native_object_editor_types.h"
#include "object_editor_config.h"
#include "stream.h"
#include "pole.h"
#include "storage.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorDocumentCapiOneTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorDocumentCapiOneTest::SetUpTestCase() {}

void ObjectEditorDocumentCapiOneTest::TearDownTestCase() {}

void ObjectEditorDocumentCapiOneTest::SetUp()
{
    ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
    config.isSupportObjectEditor_.isLoaded = true;
    config.isSupportObjectEditor_.value = true;
}

void ObjectEditorDocumentCapiOneTest::TearDown() {}

/**
 * @tc.name: OH_ContentEmbed_CreateDocumentByOEid_001
 * @tc.desc: Test OH_ContentEmbed_CreateDocumentByOEid_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_CreateDocumentByOEid_001, TestSize.Level1)
{
    const char *oeid = "0003000C-0000-0000-C000-000000000046";
    ContentEmbed_Document *document = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateDocumentByOEid(oeid, &document);
    EXPECT_EQ(ret, CE_ERR_OK);
}

/**
 * @tc.name: OH_ContentEmbed_CreateDocumentByOEid_002
 * @tc.desc: Test OH_ContentEmbed_CreateDocumentByOEid_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_CreateDocumentByOEid_002, TestSize.Level1)
{
    ContentEmbed_Document *document = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateDocumentByOEid(nullptr, &document);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_CreateDocumentByOEid_003
 * @tc.desc: Test OH_ContentEmbed_CreateDocumentByOEid_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_CreateDocumentByOEid_003, TestSize.Level1)
{
    const char *oeid = "0003000C-0000-0000-C000-000000000046";
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateDocumentByOEid(oeid, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_CreateDocumentByOEid_004
 * @tc.desc: Test OH_ContentEmbed_CreateDocumentByOEid_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_CreateDocumentByOEid_004, TestSize.Level1)
{
    const char *oeid = "invalid";
    ContentEmbed_Document *document = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateDocumentByOEid(oeid, &document);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_CreateDocumentByFile_001
 * @tc.desc: Test OH_ContentEmbed_CreateDocumentByFile_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_CreateDocumentByFile_001, TestSize.Level1)
{
    const char *filePath = "/data/test/test.doc";
    size_t length = strlen(filePath);
    ContentEmbed_Document *document = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateDocumentByFile(filePath, length, false, &document);
    EXPECT_EQ(ret, CE_ERR_OK);
}

/**
 * @tc.name: OH_ContentEmbed_CreateDocumentByFile_002
 * @tc.desc: Test OH_ContentEmbed_CreateDocumentByFile_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_CreateDocumentByFile_002, TestSize.Level1)
{
    ContentEmbed_Document *document = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateDocumentByFile(nullptr, 0, false, &document);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_CreateDocumentByFile_003
 * @tc.desc: Test OH_ContentEmbed_CreateDocumentByFile_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_CreateDocumentByFile_003, TestSize.Level1)
{
    const char *filePath = "/data/test/test.doc";
    size_t length = strlen(filePath);
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateDocumentByFile(filePath, length, false, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_CreateDocumentByFile_004
 * @tc.desc: Test OH_ContentEmbed_CreateDocumentByFile_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_CreateDocumentByFile_004, TestSize.Level1)
{
    const char *filePath = "/data/test/test.doc";
    size_t length = 0;
    ContentEmbed_Document *document = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_CreateDocumentByFile(filePath, length, false, &document);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_LoadDocumentFromFile_001
 * @tc.desc: Test OH_ContentEmbed_LoadDocumentFromFile_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_LoadDocumentFromFile_001, TestSize.Level1)
{
    const char *filePath = "/data/test/test.doc";
    size_t length = strlen(filePath);
    ContentEmbed_Document *document = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_LoadDocumentFromFile(filePath, length, &document);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
}

/**
 * @tc.name: OH_ContentEmbed_LoadDocumentFromFile_002
 * @tc.desc: Test OH_ContentEmbed_LoadDocumentFromFile_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_LoadDocumentFromFile_002, TestSize.Level1)
{
    ContentEmbed_Document *document = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_LoadDocumentFromFile(nullptr, 0, &document);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_LoadDocumentFromFile_003
 * @tc.desc: Test OH_ContentEmbed_LoadDocumentFromFile_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_LoadDocumentFromFile_003, TestSize.Level1)
{
    const char *filePath = "/data/test/test.doc";
    size_t length = strlen(filePath);
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_LoadDocumentFromFile(filePath, length, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_LoadDocumentFromFile_004
 * @tc.desc: Test OH_ContentEmbed_LoadDocumentFromFile_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_LoadDocumentFromFile_004, TestSize.Level1)
{
    const char *filePath = "/data/test/test.doc";
    size_t length = 0;
    ContentEmbed_Document *document = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_LoadDocumentFromFile(filePath, length, &document);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Document_Read_001
 * @tc.desc: Test OH_ContentEmbed_Document_Read_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_Read_001, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    uint8_t buffer[1024] = {0};
    size_t length = 1024;
    size_t offset = 0;
    size_t readSize = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_Read(buffer, length, document, offset, &readSize);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_Read_002
 * @tc.desc: Test OH_ContentEmbed_Document_Read_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_Read_002, TestSize.Level1)
{
    uint8_t buffer[1024] = {0};
    size_t length = 1024;
    size_t offset = 0;
    size_t readSize = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_Read(buffer, length, nullptr, offset, &readSize);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Document_Read_003
 * @tc.desc: Test OH_ContentEmbed_Document_Read_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_Read_003, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    size_t length = 1024;
    size_t offset = 0;
    size_t readSize = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_Read(nullptr, length, document, offset, &readSize);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_Read_004
 * @tc.desc: Test OH_ContentEmbed_Document_Read_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_Read_004, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    uint8_t buffer[1024] = {0};
    size_t length = 1024;
    size_t offset = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_Read(buffer, length, document, offset, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_Read_005
 * @tc.desc: Test OH_ContentEmbed_Document_Read_005 exception case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_Read_005, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    uint8_t buffer[1024] = {0};
    size_t length = 0;
    size_t offset = 0;
    size_t readSize = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_Read(buffer, length, document, offset, &readSize);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_EQ(readSize, 0);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetOEid_001
 * @tc.desc: Test OH_ContentEmbed_Document_GetOEid_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetOEid_001, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeid = "0003000C-0000-0000-C000-000000000046";
    char oeid[MAX_OEID_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetOEid(document, oeid);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_STREQ(oeid, "0003000C-0000-0000-C000-000000000046");
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetOEid_002
 * @tc.desc: Test OH_ContentEmbed_Document_GetOEid_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetOEid_002, TestSize.Level1)
{
    char oeid[MAX_OEID_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetOEid(nullptr, oeid);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetOEid_003
 * @tc.desc: Test OH_ContentEmbed_Document_GetOEid_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetOEid_003, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetOEid(document, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_IsLinking_001
 * @tc.desc: Test OH_ContentEmbed_Document_IsLinking_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_IsLinking_001, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->linking = true;
    bool isLinking = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_IsLinking(document, &isLinking);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_TRUE(isLinking);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_IsLinking_002
 * @tc.desc: Test OH_ContentEmbed_Document_IsLinking_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_IsLinking_002, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->linking = false;
    bool isLinking = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_IsLinking(document, &isLinking);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_FALSE(isLinking);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_IsLinking_003
 * @tc.desc: Test OH_ContentEmbed_Document_IsLinking_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_IsLinking_003, TestSize.Level1)
{
    bool isLinking = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_IsLinking(nullptr, &isLinking);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Document_IsLinking_004
 * @tc.desc: Test OH_ContentEmbed_Document_IsLinking_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_IsLinking_004, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_IsLinking(document, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetNativeFilePath_001
 * @tc.desc: Test OH_ContentEmbed_Document_GetNativeFilePath_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetNativeFilePath_001, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    char nativeFilePath[MAX_PATH_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetNativeFilePath(document, nativeFilePath);
    EXPECT_EQ(ret, CE_ERR_OK);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetNativeFilePath_002
 * @tc.desc: Test OH_ContentEmbed_Document_GetNativeFilePath_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetNativeFilePath_002, TestSize.Level1)
{
    char nativeFilePath[MAX_PATH_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetNativeFilePath(nullptr, nativeFilePath);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetNativeFilePath_003
 * @tc.desc: Test OH_ContentEmbed_Document_GetNativeFilePath_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetNativeFilePath_003, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetNativeFilePath(document, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetNativeFilePath_004
 * @tc.desc: Test OH_ContentEmbed_Document_GetNativeFilePath_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetNativeFilePath_004, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    char nativeFilePath[MAX_PATH_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetNativeFilePath(document, nativeFilePath);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetRootStorage_001
 * @tc.desc: Test OH_ContentEmbed_Document_GetRootStorage_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetRootStorage_001, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_Storage *storage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetRootStorage(document, &storage);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetRootStorage_002
 * @tc.desc: Test OH_ContentEmbed_Document_GetRootStorage_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetRootStorage_002, TestSize.Level1)
{
    ContentEmbed_Storage *storage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetRootStorage(nullptr, &storage);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetRootStorage_003
 * @tc.desc: Test OH_ContentEmbed_Document_GetRootStorage_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetRootStorage_003, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetRootStorage(document, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_GetRootStorage_004
 * @tc.desc: Test OH_ContentEmbed_Document_GetRootStorage_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_GetRootStorage_004, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = nullptr;
    ContentEmbed_Storage *storage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_GetRootStorage(document, &storage);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_Flush_001
 * @tc.desc: Test OH_ContentEmbed_Document_Flush_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_Flush_001, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_Flush(document);
    EXPECT_EQ(ret, CE_ERR_FILE_OPERATION_FAILED);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_Document_Flush_002
 * @tc.desc: Test OH_ContentEmbed_Document_Flush_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_Flush_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_Flush(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Document_Flush_003
 * @tc.desc: Test OH_ContentEmbed_Document_Flush_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Document_Flush_003, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Document_Flush(document);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete document;
}

/**
 * @tc.name: OH_ContentEmbed_DestroyDocument_001
 * @tc.desc: Test OH_ContentEmbed_DestroyDocument_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_DestroyDocument_001, TestSize.Level1)
{
    ContentEmbed_Document *document = new ContentEmbed_Document();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyDocument(document);
    EXPECT_EQ(ret, CE_ERR_OK);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyDocument_002
 * @tc.desc: Test OH_ContentEmbed_DestroyDocument_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_DestroyDocument_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyDocument(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CreateStorage_001
 * @tc.desc: Test OH_ContentEmbed_Storage_CreateStorage_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CreateStorage_001, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    parentStorage->owner = new ContentEmbed_Document();
    parentStorage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    parentStorage->name = "/";
    ContentEmbed_Storage *childStorage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CreateStorage(parentStorage, nullptr, &childStorage);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage->owner;
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CreateStorage_002
 * @tc.desc: Test OH_ContentEmbed_Storage_CreateStorage_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CreateStorage_002, TestSize.Level1)
{
    ContentEmbed_Storage *childStorage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CreateStorage(nullptr, "test", &childStorage);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CreateStorage_003
 * @tc.desc: Test OH_ContentEmbed_Storage_CreateStorage_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CreateStorage_003, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CreateStorage(parentStorage, "test", nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CreateStorage_004
 * @tc.desc: Test OH_ContentEmbed_Storage_CreateStorage_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CreateStorage_004, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    parentStorage->owner = new ContentEmbed_Document();
    parentStorage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    parentStorage->name = "/";
    ContentEmbed_Storage *childStorage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CreateStorage(parentStorage, "", &childStorage);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage->owner;
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetStorage_001
 * @tc.desc: Test OH_ContentEmbed_Storage_GetStorage_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetStorage_001, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    parentStorage->owner = new ContentEmbed_Document();
    parentStorage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    parentStorage->name = "/";
    ContentEmbed_Storage *childStorage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetStorage(parentStorage, nullptr, &childStorage);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage->owner;
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetStorage_002
 * @tc.desc: Test OH_ContentEmbed_Storage_GetStorage_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetStorage_002, TestSize.Level1)
{
    ContentEmbed_Storage *childStorage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetStorage(nullptr, "test", &childStorage);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetStorage_003
 * @tc.desc: Test OH_ContentEmbed_Storage_GetStorage_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetStorage_003, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetStorage(parentStorage, "test", nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CreateStream_001
 * @tc.desc: Test OH_ContentEmbed_Storage_CreateStream_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CreateStream_001, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    parentStorage->owner = new ContentEmbed_Document();
    parentStorage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    parentStorage->name = "/";
    ContentEmbed_Stream *childStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CreateStream(parentStorage, nullptr, &childStream);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage->owner;
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CreateStream_002
 * @tc.desc: Test OH_ContentEmbed_Storage_CreateStream_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CreateStream_002, TestSize.Level1)
{
    ContentEmbed_Stream *childStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CreateStream(nullptr, "test", &childStream);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CreateStream_003
 * @tc.desc: Test OH_ContentEmbed_Storage_CreateStream_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CreateStream_003, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CreateStream(parentStorage, "test", nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetStream_001
 * @tc.desc: Test OH_ContentEmbed_Storage_GetStream_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetStream_001, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    parentStorage->owner = new ContentEmbed_Document();
    parentStorage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    parentStorage->name = "/";
    ContentEmbed_Stream *childStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetStream(parentStorage, nullptr, &childStream);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage->owner;
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetStream_002
 * @tc.desc: Test OH_ContentEmbed_Storage_GetStream_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetStream_002, TestSize.Level1)
{
    ContentEmbed_Stream *childStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetStream(nullptr, "test", &childStream);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetStream_003
 * @tc.desc: Test OH_ContentEmbed_Storage_GetStream_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetStream_003, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetStream(parentStorage, "test", nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_DeleteEntry_001
 * @tc.desc: Test OH_ContentEmbed_Storage_DeleteEntry_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_DeleteEntry_001, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    parentStorage->owner = new ContentEmbed_Document();
    parentStorage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    parentStorage->name = "/";
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_DeleteEntry(parentStorage, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage->owner;
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_DeleteEntry_002
 * @tc.desc: Test OH_ContentEmbed_Storage_DeleteEntry_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_DeleteEntry_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_DeleteEntry(nullptr, "test");
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_DeleteEntry_003
 * @tc.desc: Test OH_ContentEmbed_Storage_DeleteEntry_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_DeleteEntry_003, TestSize.Level1)
{
    ContentEmbed_Storage *parentStorage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_DeleteEntry(parentStorage, "");
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete parentStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_DeleteAllEntry_001
 * @tc.desc: Test OH_ContentEmbed_Storage_DeleteAllEntry_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_DeleteAllEntry_001, TestSize.Level1)
{
    ContentEmbed_Storage *storage = new ContentEmbed_Storage();
    storage->owner = new ContentEmbed_Document();
    storage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    storage->name = "/";
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_DeleteAllEntry(storage);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete storage->owner;
    delete storage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_DeleteAllEntry_002
 * @tc.desc: Test OH_ContentEmbed_Storage_DeleteAllEntry_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_DeleteAllEntry_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_DeleteAllEntry(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyStorage_001
 * @tc.desc: Test OH_ContentEmbed_DestroyStorage_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_DestroyStorage_001, TestSize.Level1)
{
    ContentEmbed_Storage *storage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyStorage(storage);
    EXPECT_EQ(ret, CE_ERR_OK);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyStorage_002
 * @tc.desc: Test OH_ContentEmbed_DestroyStorage_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_DestroyStorage_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyStorage(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Read_001
 * @tc.desc: Test OH_ContentEmbed_Stream_Read_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Read_001, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    stream->owner = new ContentEmbed_Document();
    stream->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    stream->path = "/test";
    unsigned char *buffer = nullptr;
    size_t length = 1024;
    size_t num = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Read(stream, &buffer, length, &num);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    if (buffer != nullptr) {
        delete[] buffer;
    }
    delete stream->owner;
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Read_002
 * @tc.desc: Test OH_ContentEmbed_Stream_Read_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Read_002, TestSize.Level1)
{
    unsigned char *buffer = nullptr;
    size_t length = 1024;
    size_t num = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Read(nullptr, &buffer, length, &num);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Read_003
 * @tc.desc: Test OH_ContentEmbed_Stream_Read_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Read_003, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    size_t length = 1024;
    size_t num = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Read(stream, nullptr, length, &num);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Read_004
 * @tc.desc: Test OH_ContentEmbed_Stream_Read_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Read_004, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    unsigned char *buffer = nullptr;
    size_t length = 1024;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Read(stream, &buffer, length, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Read_005
 * @tc.desc: Test OH_ContentEmbed_Stream_Read_005 exception case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Read_005, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    unsigned char *buffer = nullptr;
    size_t length = 0;
    size_t num = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Read(stream, &buffer, length, &num);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Write_001
 * @tc.desc: Test OH_ContentEmbed_Stream_Write_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Write_001, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    stream->owner = new ContentEmbed_Document();
    stream->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    stream->path = "/test";
    unsigned char data[1024] = {0};
    size_t length = 1024;
    size_t num = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Write(stream, data, length, &num);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete stream->owner;
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Write_002
 * @tc.desc: Test OH_ContentEmbed_Stream_Write_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Write_002, TestSize.Level1)
{
    unsigned char data[1024] = {0};
    size_t length = 1024;
    size_t num = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Write(nullptr, data, length, &num);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Write_003
 * @tc.desc: Test OH_ContentEmbed_Stream_Write_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Write_003, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    size_t length = 1024;
    size_t num = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Write(stream, nullptr, length, &num);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Write_004
 * @tc.desc: Test OH_ContentEmbed_Stream_Write_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Write_004, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    unsigned char data[1024] = {0};
    size_t length = 1024;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Write(stream, data, length, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Write_005
 * @tc.desc: Test OH_ContentEmbed_Stream_Write_005 exception case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Write_005, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    unsigned char data[1024] = {0};
    size_t length = 0;
    size_t num = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Write(stream, data, length, &num);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_EQ(num, 0);
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Seek_001
 * @tc.desc: Test OH_ContentEmbed_Stream_Seek_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Seek_001, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    stream->owner = new ContentEmbed_Document();
    stream->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    stream->path = "/test";
    size_t position = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Seek(stream, position);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete stream->owner;
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_Seek_002
 * @tc.desc: Test OH_ContentEmbed_Stream_Seek_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_Seek_002, TestSize.Level1)
{
    size_t position = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_Seek(nullptr, position);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Stream_GetPosition_001
 * @tc.desc: Test OH_ContentEmbed_Stream_GetPosition_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_GetPosition_001, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    stream->owner = new ContentEmbed_Document();
    stream->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    stream->path = "/test";
    size_t position = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_GetPosition(stream, &position);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete stream->owner;
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_GetPosition_002
 * @tc.desc: Test OH_ContentEmbed_Stream_GetPosition_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_GetPosition_002, TestSize.Level1)
{
    size_t position = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_GetPosition(nullptr, &position);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Stream_GetPosition_003
 * @tc.desc: Test OH_ContentEmbed_Stream_GetPosition_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_GetPosition_003, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_GetPosition(stream, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_GetSize_001
 * @tc.desc: Test OH_ContentEmbed_Stream_GetSize_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_GetSize_001, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    stream->owner = new ContentEmbed_Document();
    stream->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    stream->path = "/test";
    size_t size = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_GetSize(stream, &size);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete stream->owner;
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_Stream_GetSize_002
 * @tc.desc: Test OH_ContentEmbed_Stream_GetSize_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_GetSize_002, TestSize.Level1)
{
    size_t size = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_GetSize(nullptr, &size);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Stream_GetSize_003
 * @tc.desc: Test OH_ContentEmbed_Stream_GetSize_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Stream_GetSize_003, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Stream_GetSize(stream, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete stream;
}

/**
 * @tc.name: OH_ContentEmbed_DestroyStream_001
 * @tc.desc: Test OH_ContentEmbed_DestroyStream_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_DestroyStream_001, TestSize.Level1)
{
    ContentEmbed_Stream *stream = new ContentEmbed_Stream();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyStream(stream);
    EXPECT_EQ(ret, CE_ERR_OK);
}

/**
 * @tc.name: OH_ContentEmbed_DestroyStream_002
 * @tc.desc: Test OH_ContentEmbed_DestroyStream_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_DestroyStream_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_DestroyStream(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetOEid_001
 * @tc.desc: Test OH_ContentEmbed_Storage_GetOEid_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetOEid_001, TestSize.Level1)
{
    ContentEmbed_Storage *storage = new ContentEmbed_Storage();
    storage->owner = new ContentEmbed_Document();
    storage->owner->oeid = "0003000C-0000-0000-C000-000000000046";
    storage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    storage->name = "/";
    char oeid[MAX_OEID_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetOEid(storage, oeid, MAX_OEID_LENGTH);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete storage->owner;
    delete storage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetOEid_002
 * @tc.desc: Test OH_ContentEmbed_Storage_GetOEid_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetOEid_002, TestSize.Level1)
{
    char oeid[MAX_OEID_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetOEid(nullptr, oeid, MAX_OEID_LENGTH);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetOEid_003
 * @tc.desc: Test OH_ContentEmbed_Storage_GetOEid_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetOEid_003, TestSize.Level1)
{
    ContentEmbed_Storage *storage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetOEid(storage, nullptr, MAX_OEID_LENGTH);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_SetOEid_001
 * @tc.desc: Test OH_ContentEmbed_Storage_SetOEid_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_SetOEid_001, TestSize.Level1)
{
    ContentEmbed_Storage *storage = new ContentEmbed_Storage();
    storage->owner = new ContentEmbed_Document();
    storage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    storage->name = "/";
    char oeid[MAX_OEID_LENGTH] = "0003000C-0000-0000-C000-000000000046";
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_SetOEid(storage, oeid, MAX_OEID_LENGTH);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete storage->owner;
    delete storage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_SetOEid_002
 * @tc.desc: Test OH_ContentEmbed_Storage_SetOEid_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_SetOEid_002, TestSize.Level1)
{
    char oeid[MAX_OEID_LENGTH] = "0003000C-0000-0000-C000-000000000046";
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_SetOEid(nullptr, oeid, MAX_OEID_LENGTH);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_SetOEid_003
 * @tc.desc: Test OH_ContentEmbed_Storage_SetOEid_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_SetOEid_003, TestSize.Level1)
{
    ContentEmbed_Storage *storage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_SetOEid(storage, nullptr, MAX_OEID_LENGTH);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_SetOEid_004
 * @tc.desc: Test OH_ContentEmbed_Storage_SetOEid_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_SetOEid_004, TestSize.Level1)
{
    ContentEmbed_Storage *storage = new ContentEmbed_Storage();
    storage->owner = new ContentEmbed_Document();
    char oeid[MAX_OEID_LENGTH] = "invalid";
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_SetOEid(storage, oeid, MAX_OEID_LENGTH);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storage->owner;
    delete storage;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_Create_001
 * @tc.desc: Test OH_ContentEmbed_StorageElements_Create_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_Create_001, TestSize.Level1)
{
    ContentEmbed_StorageElements *storageElements = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_Create(&storageElements);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(storageElements, nullptr);
    OH_ContentEmbed_StorageElements_Destroy(storageElements);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_Create_002
 * @tc.desc: Test OH_ContentEmbed_StorageElements_Create_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_Create_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_Create(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_Destroy_001
 * @tc.desc: Test OH_ContentEmbed_StorageElements_Destroy_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_Destroy_001, TestSize.Level1)
{
    ContentEmbed_StorageElements *storageElements = new ContentEmbed_StorageElements();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_Destroy(storageElements);
    EXPECT_EQ(ret, CE_ERR_OK);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_Destroy_002
 * @tc.desc: Test OH_ContentEmbed_StorageElements_Destroy_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_Destroy_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_Destroy(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetElements_001
 * @tc.desc: Test OH_ContentEmbed_Storage_GetElements_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetElements_001, TestSize.Level1)
{
    ContentEmbed_Storage *storage = new ContentEmbed_Storage();
    storage->owner = new ContentEmbed_Document();
    storage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    storage->name = "/";
    ContentEmbed_StorageElements *storageElements = new ContentEmbed_StorageElements();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetElements(storage, storageElements);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete storageElements;
    delete storage->owner;
    delete storage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetElements_002
 * @tc.desc: Test OH_ContentEmbed_Storage_GetElements_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetElements_002, TestSize.Level1)
{
    ContentEmbed_StorageElements *storageElements = new ContentEmbed_StorageElements();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetElements(nullptr, storageElements);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storageElements;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_GetElements_003
 * @tc.desc: Test OH_ContentEmbed_Storage_GetElements_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_GetElements_003, TestSize.Level1)
{
    ContentEmbed_Storage *storage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_GetElements(storage, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storage;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_GetCount_001
 * @tc.desc: Test OH_ContentEmbed_StorageElements_GetCount_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_GetCount_001, TestSize.Level1)
{
    ContentEmbed_StorageElements *storageElements = new ContentEmbed_StorageElements();
    size_t count = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_GetCount(storageElements, &count);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_EQ(count, 0);
    OH_ContentEmbed_StorageElements_Destroy(storageElements);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_GetCount_002
 * @tc.desc: Test OH_ContentEmbed_StorageElements_GetCount_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_GetCount_002, TestSize.Level1)
{
    size_t count = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_GetCount(nullptr, &count);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_GetCount_003
 * @tc.desc: Test OH_ContentEmbed_StorageElements_GetCount_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_GetCount_003, TestSize.Level1)
{
    ContentEmbed_StorageElements *storageElements = new ContentEmbed_StorageElements();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_GetCount(storageElements, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_StorageElements_Destroy(storageElements);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_GetElement_001
 * @tc.desc: Test OH_ContentEmbed_StorageElements_GetElement_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_GetElement_001, TestSize.Level1)
{
    ContentEmbed_StorageElements *storageElements = new ContentEmbed_StorageElements();
    auto element = std::make_unique<ContentEmbed_StorageElement>();
    element->name = "test";
    element->isStorage = true;
    storageElements->elements.push_back(std::move(element));
    ContentEmbed_StorageElement *storageElement = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_GetElement(storageElements, 0, &storageElement);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(storageElement, nullptr);
    OH_ContentEmbed_StorageElements_Destroy(storageElements);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_GetElement_002
 * @tc.desc: Test OH_ContentEmbed_StorageElements_GetElement_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_GetElement_002, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_GetElement(nullptr, 0, &storageElement);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_GetElement_003
 * @tc.desc: Test OH_ContentEmbed_StorageElements_GetElement_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_GetElement_003, TestSize.Level1)
{
    ContentEmbed_StorageElements *storageElements = new ContentEmbed_StorageElements();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_GetElement(storageElements, 0, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_StorageElements_Destroy(storageElements);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElements_GetElement_004
 * @tc.desc: Test OH_ContentEmbed_StorageElements_GetElement_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElements_GetElement_004, TestSize.Level1)
{
    ContentEmbed_StorageElements *storageElements = new ContentEmbed_StorageElements();
    ContentEmbed_StorageElement *storageElement = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElements_GetElement(storageElements, 100, &storageElement);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    OH_ContentEmbed_StorageElements_Destroy(storageElements);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetName_001
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetName_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetName_001, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    storageElement->name = "test";
    char name[MAX_PATH_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetName(storageElement, name, MAX_PATH_LENGTH);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_STREQ(name, "test");
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetName_002
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetName_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetName_002, TestSize.Level1)
{
    char name[MAX_PATH_LENGTH] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetName(nullptr, name, MAX_PATH_LENGTH);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetName_003
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetName_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetName_003, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    storageElement->name = "test";
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetName(storageElement, nullptr, MAX_PATH_LENGTH);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetName_004
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetName_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetName_004, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    storageElement->name = "testlongname";
    char name[5] = {0};
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetName(storageElement, name, 5);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetCTime_001
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetCTime_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetCTime_001, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    storageElement->creationTime = 1000;
    uint64_t ctime = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetCTime(storageElement, &ctime);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_EQ(ctime, 1000);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetCTime_002
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetCTime_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetCTime_002, TestSize.Level1)
{
    uint64_t ctime = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetCTime(nullptr, &ctime);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetCTime_003
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetCTime_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetCTime_003, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetCTime(storageElement, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetMTime_001
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetMTime_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetMTime_001, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    storageElement->modifiedTime = 2000;
    uint64_t mtime = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetMTime(storageElement, &mtime);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_EQ(mtime, 2000);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetMTime_002
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetMTime_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetMTime_002, TestSize.Level1)
{
    uint64_t mtime = 0;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetMTime(nullptr, &mtime);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_GetMTime_003
 * @tc.desc: Test OH_ContentEmbed_StorageElement_GetMTime_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_GetMTime_003, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_GetMTime(storageElement, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_IsStorage_001
 * @tc.desc: Test OH_ContentEmbed_StorageElement_IsStorage_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_IsStorage_001, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    storageElement->isStorage = true;
    bool isStorage = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_IsStorage(storageElement, &isStorage);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_TRUE(isStorage);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_IsStorage_002
 * @tc.desc: Test OH_ContentEmbed_StorageElement_IsStorage_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_IsStorage_002, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    storageElement->isStorage = false;
    bool isStorage = true;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_IsStorage(storageElement, &isStorage);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_FALSE(isStorage);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_IsStorage_003
 * @tc.desc: Test OH_ContentEmbed_StorageElement_IsStorage_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_IsStorage_003, TestSize.Level1)
{
    bool isStorage = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_IsStorage(nullptr, &isStorage);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_IsStorage_004
 * @tc.desc: Test OH_ContentEmbed_StorageElement_IsStorage_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_IsStorage_004, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_IsStorage(storageElement, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_IsStream_001
 * @tc.desc: Test OH_ContentEmbed_StorageElement_IsStream_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_IsStream_001, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    storageElement->isStorage = false;
    bool isStream = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_IsStream(storageElement, &isStream);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_TRUE(isStream);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_IsStream_002
 * @tc.desc: Test OH_ContentEmbed_StorageElement_IsStream_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_IsStream_002, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    storageElement->isStorage = true;
    bool isStream = true;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_IsStream(storageElement, &isStream);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_FALSE(isStream);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_IsStream_003
 * @tc.desc: Test OH_ContentEmbed_StorageElement_IsStream_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_IsStream_003, TestSize.Level1)
{
    bool isStream = false;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_IsStream(nullptr, &isStream);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OH_ContentEmbed_StorageElement_IsStream_004
 * @tc.desc: Test OH_ContentEmbed_StorageElement_IsStream_004 exception case with boundary parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_StorageElement_IsStream_004, TestSize.Level1)
{
    ContentEmbed_StorageElement *storageElement = new ContentEmbed_StorageElement();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_StorageElement_IsStream(storageElement, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete storageElement;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CopyTo_001
 * @tc.desc: Test OH_ContentEmbed_Storage_CopyTo_001 normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CopyTo_001, TestSize.Level1)
{
    ContentEmbed_Storage *srcStorage = new ContentEmbed_Storage();
    srcStorage->owner = new ContentEmbed_Document();
    srcStorage->owner->oeid = "0003000C-0000-0000-C000-000000000046";
    srcStorage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    srcStorage->name = "/";
    ContentEmbed_Storage *dstStorage = new ContentEmbed_Storage();
    dstStorage->owner = new ContentEmbed_Document();
    dstStorage->owner->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    dstStorage->name = "/";
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CopyTo(srcStorage, dstStorage);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete srcStorage->owner;
    delete srcStorage;
    delete dstStorage->owner;
    delete dstStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CopyTo_002
 * @tc.desc: Test OH_ContentEmbed_Storage_CopyTo_002 exception case with null parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CopyTo_002, TestSize.Level1)
{
    ContentEmbed_Storage *dstStorage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CopyTo(nullptr, dstStorage);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete dstStorage;
}

/**
 * @tc.name: OH_ContentEmbed_Storage_CopyTo_003
 * @tc.desc: Test OH_ContentEmbed_Storage_CopyTo_003 exception case with invalid parameter
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorDocumentCapiOneTest, OH_ContentEmbed_Storage_CopyTo_003, TestSize.Level1)
{
    ContentEmbed_Storage *srcStorage = new ContentEmbed_Storage();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Storage_CopyTo(srcStorage, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete srcStorage;
}
} // namespace ObjectEditor
} // namespace OHOS