/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
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
#include "ability_handler.h"
#include "hilog_object_editor.h"
#include "native_object_editor_types.h"
#define private public
#include "object_editor_extension.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace AbilityRuntime {
using namespace OHOS::AppExecFwk;

class ObjectEditorExtensionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<ObjectEditorExtension> extension_;
};

void ObjectEditorExtensionTest::SetUpTestCase()
{
}

void ObjectEditorExtensionTest::TearDownTestCase()
{
}

void ObjectEditorExtensionTest::SetUp()
{
    extension_ = std::make_shared<ObjectEditorExtension>();
}

void ObjectEditorExtensionTest::TearDown()
{
    extension_ = nullptr;
}

namespace {
/**
 * @tc.name Create_002
 * @tc.desc Test Create method with null runtime
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, Create_002, TestSize.Level1)
{
    std::unique_ptr<Runtime> runtime = nullptr;
    ObjectEditorExtension* extension = ObjectEditorExtension::Create(runtime);
    EXPECT_NE(extension, nullptr);
    delete extension;
}

/**
 * @tc.name CallbackEnter_001
 * @tc.desc Test CallbackEnter method with valid code
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, CallbackEnter_001, TestSize.Level1)
{
    uint32_t code = 1;
    int32_t ret = extension_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_OK);
}

/**
 * @tc.name CallbackEnter_002
 * @tc.desc Test CallbackEnter method with zero code
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, CallbackEnter_002, TestSize.Level1)
{
    uint32_t code = 0;
    int32_t ret = extension_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_OK);
}

/**
 * @tc.name CallbackExit_001
 * @tc.desc Test CallbackExit method with valid parameters
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, CallbackExit_001, TestSize.Level1)
{
    uint32_t code = 1;
    int32_t result = ObjectorEditorExtensionErrCode::EXTENSION_OK;
    int32_t ret = extension_->CallbackExit(code, result);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name CallbackExit_002
 * @tc.desc Test CallbackExit method with error result
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, CallbackExit_002, TestSize.Level1)
{
    uint32_t code = 1;
    int32_t result = ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER;
    int32_t ret = extension_->CallbackExit(code, result);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name GetCEContext_001
 * @tc.desc Test GetCEContext method before Init
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetCEContext_001, TestSize.Level1)
{
    auto context = extension_->GetCEContext();
    EXPECT_EQ(context, nullptr);
}

/**
 * @tc.name GetSnapshot_001
 * @tc.desc Test GetSnapshot method with null ceInstance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetSnapshot_001, TestSize.Level1)
{
    std::string documentId = "test_document_id";
    ErrCode ret = extension_->GetSnapshot(documentId);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name GetSnapshot_002
 * @tc.desc Test GetSnapshot method with empty documentId
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetSnapshot_002, TestSize.Level1)
{
    std::string documentId = "";
    ErrCode ret = extension_->GetSnapshot(documentId);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name DoEdit_001
 * @tc.desc Test DoEdit method with null ceInstance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, DoEdit_001, TestSize.Level1)
{
    std::string documentId = "test_document_id";
    ErrCode ret = extension_->DoEdit(documentId);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name DoEdit_002
 * @tc.desc Test DoEdit method with empty documentId
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, DoEdit_002, TestSize.Level1)
{
    std::string documentId = "";
    ErrCode ret = extension_->DoEdit(documentId);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name GetEditStatus_001
 * @tc.desc Test GetEditStatus method with null ceInstance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetEditStatus_001, TestSize.Level1)
{
    std::string documentId = "test_document_id";
    bool isEditing = false;
    bool isModified = false;
    ErrCode ret = extension_->GetEditStatus(documentId, &isEditing, &isModified);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name GetEditStatus_002
 * @tc.desc Test GetEditStatus method with null pointers
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetEditStatus_002, TestSize.Level1)
{
    std::string documentId = "test_document_id";
    ErrCode ret = extension_->GetEditStatus(documentId, nullptr, nullptr);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name GetEditStatus_003
 * @tc.desc Test GetEditStatus method with one null pointer
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetEditStatus_003, TestSize.Level1)
{
    std::string documentId = "test_document_id";
    bool isEditing = false;
    ErrCode ret = extension_->GetEditStatus(documentId, &isEditing, nullptr);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name GetEditStatus_004
 * @tc.desc Test GetEditStatus method with other null pointer
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetEditStatus_004, TestSize.Level1)
{
    std::string documentId = "test_document_id";
    bool isModified = false;
    ErrCode ret = extension_->GetEditStatus(documentId, nullptr, &isModified);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name GetExtensionEditStatus_001
 * @tc.desc Test GetExtensionEditStatus method with null ceInstance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetExtensionEditStatus_001, TestSize.Level1)
{
    bool isEditing = false;
    ErrCode ret = extension_->GetExtensionEditStatus(isEditing);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_ERR_INVALID_UID);
    EXPECT_EQ(isEditing, false);
}

/**
 * @tc.name GetCapability_001
 * @tc.desc Test GetCapability method with null ceInstance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetCapability_001, TestSize.Level1)
{
    std::string documentId = "test_document_id";
    uint32_t bitmask = 0;
    ErrCode ret = extension_->GetCapability(documentId, &bitmask);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name GetCapability_002
 * @tc.desc Test GetCapability method with null bitmask
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, GetCapability_002, TestSize.Level1)
{
    std::string documentId = "test_document_id";
    ErrCode ret = extension_->GetCapability(documentId, nullptr);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name Close_001
 * @tc.desc Test Close method with null ceInstance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, Close_001, TestSize.Level1)
{
    std::string documentId = "test_document_id";
    bool isAllObjectsRemoved = false;
    ErrCode ret = extension_->Close(documentId, isAllObjectsRemoved, 0);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_ERR_INVALID_UID);
}

/**
 * @tc.name Close_002
 * @tc.desc Test Close method with empty documentId
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, Close_002, TestSize.Level1)
{
    std::string documentId = "";
    bool isAllObjectsRemoved = false;
    ErrCode ret = extension_->Close(documentId, isAllObjectsRemoved, 0);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_ERR_INVALID_UID);
}

/**
 * @tc.name Initial_001
 * @tc.desc Test Initial method with null document
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, Initial_001, TestSize.Level1)
{
    std::unique_ptr<ObjectEditor::ObjectEditorDocument> document = nullptr;
    sptr<IObjectEditorClientCallback> clientCb = nullptr;
    extension_->moduleLoaded_ = true;
    ErrCode ret = extension_->Initial(std::move(document), clientCb, 0);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_PARAM_INVALID);
}

/**
 * @tc.name Initial_002
 * @tc.desc Test Initial method with null client callback
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, Initial_002, TestSize.Level1)
{
    std::unique_ptr<ObjectEditor::ObjectEditorDocument> document =
        std::make_unique<ObjectEditor::ObjectEditorDocument>();
    sptr<IObjectEditorClientCallback> clientCb = nullptr;
    extension_->moduleLoaded_ = true;
    ErrCode ret = extension_->Initial(std::move(document), clientCb, 0);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_PARAM_INVALID);
}

/**
 * @tc.name CreateObject_001
 * @tc.desc Test CreateObject with null document
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, CreateObject_001, TestSize.Level1)
{
    std::unique_ptr<ObjectEditor::ObjectEditorDocument> document = nullptr;
    sptr<IObjectEditorClientCallback> clientCb = nullptr;
    ErrCode ret = extension_->CreateObject(std::move(document), clientCb, 0);
    EXPECT_EQ(ret, ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER);
}

/**
 * @tc.name Destructor_001
 * @tc.desc Test destructor
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionTest, Destructor_001, TestSize.Level1)
{
    ObjectEditorExtension* extension = new ObjectEditorExtension();
    delete extension;
    EXPECT_TRUE(true);
}
}
}
}