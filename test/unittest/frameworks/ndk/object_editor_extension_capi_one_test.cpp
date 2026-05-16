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
#include "stub.h"

#define private public
#include "content_embed_extension.h"
#include "mock_object_editor_client_callback.h"
#include "mock_object_editor_document.h"
#include "native_object_editor_types.h"
#include "object_editor_config.h"
#include "object_editor_document.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorExtensionCapiOneTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorExtensionCapiOneTest::SetUpTestCase() {}

void ObjectEditorExtensionCapiOneTest::TearDownTestCase() {}

void ObjectEditorExtensionCapiOneTest::SetUp()
{
    ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
    config.isSupportObjectEditor_.isLoaded = true;
    config.isSupportObjectEditor_.value = true;
}

void ObjectEditorExtensionCapiOneTest::TearDown() {}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnUpdate_CallbackNotRegistered
 * @tc.desc: Test CallbackToOnUpdate when clientCb is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnUpdate_CallbackNotRegistered, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->document = std::make_unique<ContentEmbed_Document>();
    object->document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    object->clientCb = nullptr;
    
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnUpdate(object);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED);
    
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnUpdate_CallbackFailed
 * @tc.desc: Test CallbackToOnUpdate when OnUpdate returns error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnUpdate_CallbackFailed, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->document = std::make_unique<ContentEmbed_Document>();
    object->document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnUpdate(_)).WillByDefault(Return(ERR_INVALID_VALUE));
    
    object->clientCb = mockCallback;
    
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnUpdate(object);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_FAILED);
    
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnError_CallbackNotRegistered
 * @tc.desc: Test CallbackToOnError when clientCb is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnError_CallbackNotRegistered, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->clientCb = nullptr;
    
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnError(object, CE_ERR_OK);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED);
    
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnError_CallbackFailed
 * @tc.desc: Test CallbackToOnError when OnError returns error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnError_CallbackFailed, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnError(_)).WillByDefault(Return(ERR_INVALID_VALUE));
    
    object->clientCb = mockCallback;
    
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnError(object, CE_ERR_OK);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_FAILED);
    
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnEditingFinished_CallbackNotRegistered
 * @tc.desc: Test CallbackToOnEditingFinished when clientCb is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnEditingFinished_CallbackNotRegistered, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->clientCb = nullptr;
    
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnEditingFinished(object, false);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED);
    
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnEditingFinished_CallbackFailed
 * @tc.desc: Test CallbackToOnEditingFinished when OnStopEdit returns error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnEditingFinished_CallbackFailed, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnStopEdit(_)).WillByDefault(Return(ERR_INVALID_VALUE));
    
    object->clientCb = mockCallback;
    
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnEditingFinished(object, false);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_FAILED);
    
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnExtensionStopped_CallbackNotRegistered
 * @tc.desc: Test CallbackToOnExtensionStopped when object's clientCb is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnExtensionStopped_CallbackNotRegistered, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->clientCb = nullptr;
    
    instance->objects["testObjectId"] = std::unique_ptr<ContentEmbed_Object>(object);
    
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnExtensionStopped(instance);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED);
    
    instance->objects.clear();
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnExtensionStopped_CallbackFailed
 * @tc.desc: Test CallbackToOnExtensionStopped when OnExtensionStopped returns error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnExtensionStopped_CallbackFailed, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    
    ContentEmbed_Object *object = new ContentEmbed_Object();
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnExtensionStopped()).WillByDefault(Return(ERR_INVALID_VALUE));
    
    object->clientCb = mockCallback;
    
    instance->objects["testObjectId"] = std::unique_ptr<ContentEmbed_Object>(object);
    
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnExtensionStopped(instance);
    EXPECT_EQ(ret, CE_ERR_CLIENT_CALLBACK_FAILED);
    
    instance->objects.clear();
    delete instance;
}

} // namespace ObjectEditor
} // namespace OHOS
