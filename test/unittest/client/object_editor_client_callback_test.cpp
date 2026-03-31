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
#include "mock_object_editor_client_callback.h"
#include "object_editor_client_callback.h"
#include "stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorClientCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<ObjectEditorClientCallback> clientCallback_;
};

void ObjectEditorClientCallbackTest::SetUpTestCase()
{
}

void ObjectEditorClientCallbackTest::TearDownTestCase()
{
}

void ObjectEditorClientCallbackTest::SetUp()
{
    struct ContentEmbed_ExtensionProxy *proxy = nullptr;
    clientCallback_ = std::make_unique<ObjectEditorClientCallback>(proxy);
}

void ObjectEditorClientCallbackTest::TearDown()
{
    if (clientCallback_) {
        delete clientCallback_->proxy_;
    }
    clientCallback_->proxy_ = nullptr;
}

static void MockErrorFunc(ContentEmbed_ExtensionProxy *proxy, ContentEmbed_ErrorCode error)
{
    return;
}

static void MockUpdateFunc(ContentEmbed_ExtensionProxy *proxy)
{
    return;
}

static void MockStopEditorFunc(ContentEmbed_ExtensionProxy *proxy, bool dataModified)
{
    return;
}
namespace {

/**
 * @tc.name CallbackEnter_001
 * @tc.desc 测试CallbackEnter方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, CallbackEnter_001, TestSize.Level1)
{
    uint32_t code = 0;
    auto ret = clientCallback_->CallbackEnter(code);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name CallbackExit_001
 * @tc.desc 测试CallbackExit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, CallbackExit_001, TestSize.Level1)
{
    uint32_t code = 0;
    int32_t result = 0;
    auto ret = clientCallback_->CallbackExit(code, result);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name OnUpdate_001
 * @tc.desc 测试OnUpdate方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnUpdate_001, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    clientCallback_->proxy_ = nullptr;
    auto ret = clientCallback_->OnUpdate(document);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name OnUpdate_002
 * @tc.desc 测试OnUpdate方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnUpdate_002, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->UpdateFunc = MockUpdateFunc;
    clientCallback_->proxy_->ceDocument = new ContentEmbed_Document();
    auto ret = clientCallback_->OnUpdate(document);
    EXPECT_EQ(ret, ERR_OK);
    delete clientCallback_->proxy_->ceDocument;
}

/**
 * @tc.name OnUpdate_003
 * @tc.desc 测试OnUpdate方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnUpdate_003, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->UpdateFunc = MockUpdateFunc;
    clientCallback_->proxy_->ceDocument = nullptr;
    auto ret = clientCallback_->OnUpdate(document);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name OnUpdate_004
 * @tc.desc 测试OnUpdate方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnUpdate_004, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->UpdateFunc = nullptr;
    clientCallback_->proxy_->ceDocument = new ContentEmbed_Document();
    auto ret = clientCallback_->OnUpdate(document);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    delete clientCallback_->proxy_->ceDocument;
}

/**
 * @tc.name OnUpdate_005
 * @tc.desc 测试OnUpdate方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnUpdate_005, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->UpdateFunc = MockUpdateFunc;
    clientCallback_->proxy_->ceDocument = new ContentEmbed_Document();
    auto ret = clientCallback_->OnUpdate(document);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    delete clientCallback_->proxy_->ceDocument;
}

/**
 * @tc.name OnError_001
 * @tc.desc 测试OnError方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnError_001, TestSize.Level1)
{
    ContentEmbed_ErrorCode error = CE_ERR_OK;
    clientCallback_->proxy_ = nullptr;
    auto ret = clientCallback_->OnError(error);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name OnError_002
 * @tc.desc 测试OnError方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnError_002, TestSize.Level1)
{
    ContentEmbed_ErrorCode error = CE_ERR_OK;
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->ErrorFunc = MockErrorFunc;
    auto ret = clientCallback_->OnError(error);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name OnError_003
 * @tc.desc 测试OnError方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnError_003, TestSize.Level1)
{
    ContentEmbed_ErrorCode error = CE_ERR_OK;
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->ErrorFunc = MockErrorFunc;
    auto ret = clientCallback_->OnError(error);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name OnStopEdit_001
 * @tc.desc 测试OnStopEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnStopEdit_001, TestSize.Level1)
{
    clientCallback_->proxy_ = nullptr;
    auto ret = clientCallback_->OnStopEdit(false);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name OnStopEdit_002
 * @tc.desc 测试OnStopEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnStopEdit_002, TestSize.Level1)
{
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->onEditingFinishedFunc = MockStopEditorFunc;
    auto ret = clientCallback_->OnStopEdit(false);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name OnStopEdit_003
 * @tc.desc 测试OnStopEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnStopEdit_003, TestSize.Level1)
{
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->onEditingFinishedFunc = nullptr;
    auto ret = clientCallback_->OnStopEdit(false);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name OnStopEdit_004
 * @tc.desc 测试OnStopEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnStopEdit_004, TestSize.Level1)
{
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->onEditingFinishedFunc = MockStopEditorFunc;
    clientCallback_->proxy_->ceDocument = new ContentEmbed_Document();
    clientCallback_->proxy_->ceDocument->oeDocumentInner = nullptr;
    auto ret = clientCallback_->OnStopEdit(false);
    EXPECT_EQ(ret, ERR_OK);
    delete clientCallback_->proxy_->ceDocument;
}

/**
 * @tc.name OnStopEdit_005
 * @tc.desc 测试OnStopEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnStopEdit_005, TestSize.Level1)
{
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->onEditingFinishedFunc = MockStopEditorFunc;
    clientCallback_->proxy_->ceDocument = new ContentEmbed_Document();
    clientCallback_->proxy_->ceDocument->oeDocumentInner =
        std::make_unique<ObjectEditorDocument>();
    auto ret = clientCallback_->OnStopEdit(false);
    EXPECT_EQ(ret, ERR_OK);
    delete clientCallback_->proxy_->ceDocument;
}

/**
 * @tc.name OnExtensionStopped_001
 * @tc.desc 测试OnExtensionStopped方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnExtensionStopped_001, TestSize.Level1)
{
    clientCallback_->proxy_ = nullptr;
    auto ret = clientCallback_->OnExtensionStopped();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name OnExtensionStopped_002
 * @tc.desc 测试OnExtensionStopped方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnExtensionStopped_002, TestSize.Level1)
{
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->OnExtensionStoppedFunc = [](ContentEmbed_ExtensionProxy *proxy) {};
    auto ret = clientCallback_->OnExtensionStopped();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name OnExtensionStoped_003
 * @tc.desc 测试OnExtensionStoped方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientCallbackTest, OnExtensionStoped_003, TestSize.Level1)
{
    clientCallback_->proxy_ = new ContentEmbed_ExtensionProxy();
    clientCallback_->proxy_->OnExtensionStoppedFunc = nullptr;
    auto ret = clientCallback_->OnExtensionStopped();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

}
}
}