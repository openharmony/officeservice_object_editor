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
#include "content_embed_extension.h"
#include "mock_object_editor_client_callback.h"
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
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnUpdate_CallbackNotRegistered, TestSize.Level1)
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
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnError_CallbackNotRegistered, TestSize.Level1)
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
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnEditingFinished_CallbackNotRegistered, TestSize.Level1)
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
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnEditingFinished_CallbackFailed, TestSize.Level1)
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
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnExtensionStopped_CallbackNotRegistered, TestSize.Level1)
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
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnExtensionStopped_CallbackFailed, TestSize.Level1)
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

void MockOnCreateFunc(ContentEmbed_ExtensionInstanceHandle instance, AbilityBase_Want *want) {}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnCreateFunc_001
 * @tc.desc: Test RegisterOnCreateFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnCreateFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnCreateFunc(instance, MockOnCreateFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(instance->onCreateFunc, nullptr);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnCreateFunc_002
 * @tc.desc: Test RegisterOnCreateFunc with null instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnCreateFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnCreateFunc(nullptr, MockOnCreateFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnCreateFunc_003
 * @tc.desc: Test RegisterOnCreateFunc with null function
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnCreateFunc_003, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnCreateFunc(instance, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete instance;
}

void MockOnDestroyFunc(ContentEmbed_ExtensionInstanceHandle instance) {}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnDestroyFunc_001
 * @tc.desc: Test RegisterOnDestroyFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnDestroyFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnDestroyFunc(instance, MockOnDestroyFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(instance->onDestroyFunc, nullptr);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnDestroyFunc_002
 * @tc.desc: Test RegisterOnDestroyFunc with null instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnDestroyFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnDestroyFunc(nullptr, MockOnDestroyFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnDestroyFunc_003
 * @tc.desc: Test RegisterOnDestroyFunc with null function
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnDestroyFunc_003, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnDestroyFunc(instance, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete instance;
}

void MockOnObjectAttachFunc(ContentEmbed_ExtensionInstanceHandle instance, ContentEmbed_ObjectHandle object) {}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnObjectAttachFunc_001
 * @tc.desc: Test RegisterOnObjectAttachFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnObjectAttachFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnObjectAttachFunc(instance, MockOnObjectAttachFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(instance->onObjectAttachFunc, nullptr);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnObjectAttachFunc_002
 * @tc.desc: Test RegisterOnObjectAttachFunc with null instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnObjectAttachFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnObjectAttachFunc(nullptr, MockOnObjectAttachFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnObjectAttachFunc_003
 * @tc.desc: Test RegisterOnObjectAttachFunc with null function
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnObjectAttachFunc_003, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnObjectAttachFunc(instance, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionUnRegisterOnObjectAttachFunc_001
 * @tc.desc: Test UnRegisterOnObjectAttachFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionUnRegisterOnObjectAttachFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    instance->onObjectAttachFunc = MockOnObjectAttachFunc;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_UnRegisterOnObjectAttachFunc(instance);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_EQ(instance->onObjectAttachFunc, nullptr);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionUnRegisterOnObjectAttachFunc_002
 * @tc.desc: Test UnRegisterOnObjectAttachFunc with null instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionUnRegisterOnObjectAttachFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_UnRegisterOnObjectAttachFunc(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

void MockOnObjectDetachFunc(ContentEmbed_ExtensionInstanceHandle instance, ContentEmbed_ObjectHandle object) {}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnObjectDetachFunc_001
 * @tc.desc: Test RegisterOnObjectDetachFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnObjectDetachFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnObjectDetachFunc(instance, MockOnObjectDetachFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(instance->onObjectDetachFunc, nullptr);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnObjectDetachFunc_002
 * @tc.desc: Test RegisterOnObjectDetachFunc with null instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnObjectDetachFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnObjectDetachFunc(nullptr, MockOnObjectDetachFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnObjectDetachFunc_003
 * @tc.desc: Test RegisterOnObjectDetachFunc with null function
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnObjectDetachFunc_003, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnObjectDetachFunc(instance, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionUnRegisterOnObjectDetachFunc_001
 * @tc.desc: Test UnRegisterOnObjectDetachFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionUnRegisterOnObjectDetachFunc_001, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    instance->onObjectDetachFunc = MockOnObjectDetachFunc;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_UnRegisterOnObjectDetachFunc(instance);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_EQ(instance->onObjectDetachFunc, nullptr);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionUnRegisterOnObjectDetachFunc_002
 * @tc.desc: Test UnRegisterOnObjectDetachFunc with null instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionUnRegisterOnObjectDetachFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_UnRegisterOnObjectDetachFunc(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

void MockOnWriteToDataStreamFunc(ContentEmbed_ObjectHandle object) {}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnWriteToDataStreamFunc_001
 * @tc.desc: Test RegisterOnWriteToDataStreamFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionRegisterOnWriteToDataStreamFunc_001, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnWriteToDataStreamFunc(object, MockOnWriteToDataStreamFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(object->onWriteToDataStreamFunc, nullptr);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnWriteToDataStreamFunc_002
 * @tc.desc: Test RegisterOnWriteToDataStreamFunc with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionRegisterOnWriteToDataStreamFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnWriteToDataStreamFunc(nullptr, MockOnWriteToDataStreamFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnWriteToDataStreamFunc_003
 * @tc.desc: Test RegisterOnWriteToDataStreamFunc with null function
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionRegisterOnWriteToDataStreamFunc_003, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnWriteToDataStreamFunc(object, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete object;
}

void MockOnGetSnapshotFunc(ContentEmbed_ObjectHandle object) {}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnGetSnapshotFunc_001
 * @tc.desc: Test RegisterOnGetSnapshotFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnGetSnapshotFunc_001, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnGetSnapshotFunc(object, MockOnGetSnapshotFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(object->onGetSnapshotFunc, nullptr);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnGetSnapshotFunc_002
 * @tc.desc: Test RegisterOnGetSnapshotFunc with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnGetSnapshotFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnGetSnapshotFunc(nullptr, MockOnGetSnapshotFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnGetSnapshotFunc_003
 * @tc.desc: Test RegisterOnGetSnapshotFunc with null function
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnGetSnapshotFunc_003, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnGetSnapshotFunc(object, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete object;
}

void MockOnDoEditFunc(ContentEmbed_ObjectHandle object) {}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnDoEditFunc_001
 * @tc.desc: Test RegisterOnDoEditFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnDoEditFunc_001, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnDoEditFunc(object, MockOnDoEditFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(object->onDoEditFunc, nullptr);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnDoEditFunc_002
 * @tc.desc: Test RegisterOnDoEditFunc with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnDoEditFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnDoEditFunc(nullptr, MockOnDoEditFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnDoEditFunc_003
 * @tc.desc: Test RegisterOnDoEditFunc with null function
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnDoEditFunc_003, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnDoEditFunc(object, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete object;
}

void MockOnGetEditStatusFunc(ContentEmbed_ObjectHandle object, bool *isEditing, bool *isModified) {}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnGetEditStatusFunc_001
 * @tc.desc: Test RegisterOnGetEditStatusFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnGetEditStatusFunc_001, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnGetEditStatusFunc(object, MockOnGetEditStatusFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(object->onGetEditStatusFunc, nullptr);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnGetEditStatusFunc_002
 * @tc.desc: Test RegisterOnGetEditStatusFunc with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnGetEditStatusFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnGetEditStatusFunc(nullptr, MockOnGetEditStatusFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnGetEditStatusFunc_003
 * @tc.desc: Test RegisterOnGetEditStatusFunc with null function
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnGetEditStatusFunc_003, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnGetEditStatusFunc(object, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete object;
}

void MockOnGetCapabilityFunc(ContentEmbed_ObjectHandle object, uint32_t *bitmask) {}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnGetCapabilityFunc_001
 * @tc.desc: Test RegisterOnGetCapabilityFunc normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnGetCapabilityFunc_001, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnGetCapabilityFunc(object, MockOnGetCapabilityFunc);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(object->onGetCapabilityFunc, nullptr);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnGetCapabilityFunc_002
 * @tc.desc: Test RegisterOnGetCapabilityFunc with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnGetCapabilityFunc_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_RegisterOnGetCapabilityFunc(nullptr, MockOnGetCapabilityFunc);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionRegisterOnGetCapabilityFunc_003
 * @tc.desc: Test RegisterOnGetCapabilityFunc with null function
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionRegisterOnGetCapabilityFunc_003, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_RegisterOnGetCapabilityFunc(object, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionGetContentEmbedDocument_001
 * @tc.desc: Test GetContentEmbedDocument normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetContentEmbedDocument_001, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->document = std::make_unique<ContentEmbed_Document>();
    object->document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    ContentEmbed_Document *ceDocument = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetContentEmbedDocument(object, &ceDocument);
    EXPECT_EQ(ret, CE_ERR_OK);
    EXPECT_NE(ceDocument, nullptr);
    object->document = nullptr;
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionGetContentEmbedDocument_002
 * @tc.desc: Test GetContentEmbedDocument with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetContentEmbedDocument_002, TestSize.Level1)
{
    ContentEmbed_Document *ceDocument = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetContentEmbedDocument(nullptr, &ceDocument);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionGetContentEmbedDocument_003
 * @tc.desc: Test GetContentEmbedDocument with null output pointer
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetContentEmbedDocument_003, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetContentEmbedDocument(object, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionGetContentEmbedDocument_004
 * @tc.desc: Test GetContentEmbedDocument with null document inner
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetContentEmbedDocument_004, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->document = nullptr;
    ContentEmbed_Document *ceDocument = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetContentEmbedDocument(object, &ceDocument);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionGetContext_001
 * @tc.desc: Test GetContext with null ceContext
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetContext_001, TestSize.Level1)
{
    AbilityRuntime_ContextHandle context = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetContext(nullptr, &context);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionGetContext_002
 * @tc.desc: Test GetContext with null output pointer
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetContext_002, TestSize.Level1)
{
    ContentEmbed_ExtensionContext *ceContext = new ContentEmbed_ExtensionContext();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetContext(ceContext, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete ceContext;
}

/**
 * @tc.name: OHContentEmbedExtensionGetExtensionInstance_001
 * @tc.desc: Test GetExtensionInstance with null baseInstance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetExtensionInstance_001, TestSize.Level1)
{
    ContentEmbed_ExtensionInstanceHandle ceInstance = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetExtensionInstance(nullptr, &ceInstance);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionGetExtensionInstance_002
 * @tc.desc: Test GetExtensionInstance with null output pointer
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetExtensionInstance_002, TestSize.Level1)
{
    AbilityRuntime_ExtensionInstanceHandle baseInstance = reinterpret_cast<AbilityRuntime_ExtensionInstanceHandle>(1);
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetExtensionInstance(baseInstance, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionSetSnapshot_001
 * @tc.desc: Test SetSnapshot with null pixelmap
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionSetSnapshot_001, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_SetSnapshot(object, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionSetSnapshot_002
 * @tc.desc: Test SetSnapshot with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionSetSnapshot_002, TestSize.Level1)
{
    OH_PixelmapNative *pixelmap = reinterpret_cast<OH_PixelmapNative*>(1);
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_SetSnapshot(nullptr, pixelmap);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionSetSnapshot_003
 * @tc.desc: Test SetSnapshot with null document inner
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionSetSnapshot_003, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->document = nullptr;
    OH_PixelmapNative *pixelmap = reinterpret_cast<OH_PixelmapNative*>(1);
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_SetSnapshot(object, pixelmap);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionContextStartSelfUIAbility_001
 * @tc.desc: Test ContextStartSelfUIAbility with null context
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionContextStartSelfUIAbility_001, TestSize.Level1)
{
    AbilityBase_Want *want = reinterpret_cast<AbilityBase_Want*>(1);
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_ContextStartSelfUIAbility(nullptr, want);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionContextStartSelfUIAbility_002
 * @tc.desc: Test ContextStartSelfUIAbility with null want
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionContextStartSelfUIAbility_002, TestSize.Level1)
{
    ContentEmbed_ExtensionContext *context = new ContentEmbed_ExtensionContext();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_ContextStartSelfUIAbility(context, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete context;
}

/**
 * @tc.name: OHContentEmbedExtensionContextStartSelfUIAbilityWithStartOptions_001
 * @tc.desc: Test ContextStartSelfUIAbilityWithStartOptions with null context
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionContextStartSelfUIAbilityWithStartOptions_001, TestSize.Level1)
{
    AbilityBase_Want *want = reinterpret_cast<AbilityBase_Want*>(1);
    AbilityRuntime_StartOptions *options = reinterpret_cast<AbilityRuntime_StartOptions*>(1);
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_ContextStartSelfUIAbilityWithStartOptions(nullptr, want, options);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionContextStartSelfUIAbilityWithStartOptions_002
 * @tc.desc: Test ContextStartSelfUIAbilityWithStartOptions with null want
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionContextStartSelfUIAbilityWithStartOptions_002, TestSize.Level1)
{
    ContentEmbed_ExtensionContext *context = new ContentEmbed_ExtensionContext();
    AbilityRuntime_StartOptions *options = reinterpret_cast<AbilityRuntime_StartOptions*>(1);
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_ContextStartSelfUIAbilityWithStartOptions(context, nullptr, options);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete context;
}

/**
 * @tc.name: OHContentEmbedExtensionContextStartSelfUIAbilityWithStartOptions_003
 * @tc.desc: Test ContextStartSelfUIAbilityWithStartOptions with null options
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionContextStartSelfUIAbilityWithStartOptions_003, TestSize.Level1)
{
    ContentEmbed_ExtensionContext *context = new ContentEmbed_ExtensionContext();
    AbilityBase_Want *want = reinterpret_cast<AbilityBase_Want*>(1);
    ContentEmbed_ErrorCode ret =
        OH_ContentEmbed_Extension_ContextStartSelfUIAbilityWithStartOptions(context, want, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete context;
}

/**
 * @tc.name: OHContentEmbedExtensionContextTerminateAbility_001
 * @tc.desc: Test ContextTerminateAbility with null context
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionContextTerminateAbility_001, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_ContextTerminateAbility(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionGetContentEmbedContext_001
 * @tc.desc: Test GetContentEmbedContext with null instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetContentEmbedContext_001, TestSize.Level1)
{
    ContentEmbed_ExtensionContextHandle ceContext = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetContentEmbedContext(nullptr, &ceContext);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionGetContentEmbedContext_002
 * @tc.desc: Test GetContentEmbedContext with null output pointer
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionGetContentEmbedContext_002, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_GetContentEmbedContext(instance, nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnUpdate_NullDocument
 * @tc.desc: Test CallbackToOnUpdate when document is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnUpdate_NullDocument, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->document = nullptr;
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnUpdate(_)).WillByDefault(Return(ERR_OK));
    object->clientCb = mockCallback;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnUpdate(object);
    EXPECT_EQ(ret, CE_ERR_NULL_POINTER);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnExtensionStopped_EmptyObjects
 * @tc.desc: Test CallbackToOnExtensionStopped when objects map is empty
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnExtensionStopped_EmptyObjects, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnExtensionStopped(instance);
    EXPECT_EQ(ret, CE_ERR_OK);
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnExtensionStopped_NullObjectInMap
 * @tc.desc: Test CallbackToOnExtensionStopped when object in map is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnExtensionStopped_NullObjectInMap, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    instance->objects["testObjectId"] = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnExtensionStopped(instance);
    EXPECT_EQ(ret, CE_ERR_OK);
    instance->objects.clear();
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnUpdate_Success
 * @tc.desc: Test CallbackToOnUpdate success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnUpdate_Success, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    object->document = std::make_unique<ContentEmbed_Document>();
    object->document->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnUpdate(_)).WillByDefault(Return(ERR_OK));
    object->clientCb = mockCallback;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnUpdate(object);
    EXPECT_EQ(ret, CE_ERR_OK);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnError_Success
 * @tc.desc: Test CallbackToOnError success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnError_Success, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnError(_)).WillByDefault(Return(ERR_OK));
    object->clientCb = mockCallback;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnError(object, CE_ERR_OK);
    EXPECT_EQ(ret, CE_ERR_OK);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnEditingFinished_Success
 * @tc.desc: Test CallbackToOnEditingFinished success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnEditingFinished_Success, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnStopEdit(_)).WillByDefault(Return(ERR_OK));
    object->clientCb = mockCallback;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnEditingFinished(object, true);
    EXPECT_EQ(ret, CE_ERR_OK);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnEditingFinished_DataNotModified
 * @tc.desc: Test CallbackToOnEditingFinished with dataModified false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnEditingFinished_DataNotModified, TestSize.Level1)
{
    ContentEmbed_Object *object = new ContentEmbed_Object();
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnStopEdit(_)).WillByDefault(Return(ERR_OK));
    object->clientCb = mockCallback;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnEditingFinished(object, false);
    EXPECT_EQ(ret, CE_ERR_OK);
    delete object;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnExtensionStopped_Success
 * @tc.desc: Test CallbackToOnExtensionStopped success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnExtensionStopped_Success, TestSize.Level1)
{
    ContentEmbed_ExtensionInstance *instance = new ContentEmbed_ExtensionInstance();
    ContentEmbed_Object *object = new ContentEmbed_Object();
    sptr<MockObjectEditorClientCallback> mockCallback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    ON_CALL(*mockCallback, OnExtensionStopped()).WillByDefault(Return(ERR_OK));
    object->clientCb = mockCallback;
    instance->objects["testObjectId"] = std::unique_ptr<ContentEmbed_Object>(object);
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnExtensionStopped(instance);
    EXPECT_EQ(ret, CE_ERR_OK);
    instance->objects.clear();
    delete instance;
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnUpdate_NullObject
 * @tc.desc: Test CallbackToOnUpdate with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnUpdate_NullObject, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnUpdate(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnError_NullObject
 * @tc.desc: Test CallbackToOnError with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest, OHContentEmbedExtensionCallbackToOnError_NullObject, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnError(nullptr, CE_ERR_OK);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnEditingFinished_NullObject
 * @tc.desc: Test CallbackToOnEditingFinished with null object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnEditingFinished_NullObject, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnEditingFinished(nullptr, false);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

/**
 * @tc.name: OHContentEmbedExtensionCallbackToOnExtensionStopped_NullInstance
 * @tc.desc: Test CallbackToOnExtensionStopped with null instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionCapiOneTest,
    OHContentEmbedExtensionCallbackToOnExtensionStopped_NullInstance, TestSize.Level1)
{
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Extension_CallbackToOnExtensionStopped(nullptr);
    EXPECT_EQ(ret, CE_ERR_PARAM_INVALID);
}

} // namespace ObjectEditor
} // namespace OHOS
