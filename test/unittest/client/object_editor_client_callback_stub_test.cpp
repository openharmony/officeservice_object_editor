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
#include "mock_object_editor_client_callback_stub.h"
#include "stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorClientCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorClientCallbackStubTest::SetUpTestCase()
{
}

void ObjectEditorClientCallbackStubTest::TearDownTestCase()
{
}

void ObjectEditorClientCallbackStubTest::SetUp()
{
}

void ObjectEditorClientCallbackStubTest::TearDown()
{
}

bool MockWriteInt32Fail()
{
    return false;
}

namespace {

/**
 * @tc.name onRemoteRequest_001
 * @tc.desc 测试onRemoteRequest方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, onRemoteRequest_001, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    EXPECT_CALL(*stub, CallbackEnter(code)).WillOnce(Return(1));
    int32_t ret = stub->onRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name onRemoteRequest_002
 * @tc.desc 测试onRemoteRequest方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, onRemoteRequest_002, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    EXPECT_CALL(*stub, CallbackEnter(code)).WillOnce(Return(ERR_NONE));
    EXPECT_CALL(*stub, CallbackExit(_, _)).WillOnce(Return(1));
    int32_t ret = stub->onRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name onRemoteRequest_003
 * @tc.desc 测试onRemoteRequest方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, onRemoteRequest_003, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    EXPECT_CALL(*stub, CallbackEnter(code)).WillOnce(Return(ERR_NONE));
    EXPECT_CALL(*stub, CallbackExit(_, _)).WillOnce(Return(1));
    int32_t ret = stub->onRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name onRemoteRequestInner_001
 * @tc.desc 测试onRemoteRequest方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, onRemoteRequestInner_001, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(u"com.example.InvalidEditor")
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->onRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_TRANSACTION_FAILED);
}

/**
 * @tc.name onRemoteRequestInner_002
 * @tc.desc 测试onRemoteRequest方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, onRemoteRequestInner_002, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(ObjectEditorClientCallbackStub::GetDescriptor());
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->onRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_FAILED);
}

/**
 * @tc.name HandleUpdate_001
 * @tc.desc 测试HandleUpdate方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleUpdate_001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleUpdate(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name HandleUpdate_002
 * @tc.desc 测试HandleUpdate方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleUpdate_002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    auto document = std::make_unique<ObjectEditorDocument>();
    data.WriteParcelable(document.get());
    int32_t ret = stub->HandleUpdate(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleUpdate_003
 * @tc.desc 测试HandleUpdate方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleUpdate_003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    auto document = std::make_unique<ObjectEditorDocument>();
    data.WriteParcelable(document.get());
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInt32), MockWriteInt32Fail);
    int32_t ret = stub->HandleUpdate(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name HandleOnError_001
 * @tc.desc 测试HandleOnError方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnError_001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t errorCode = static_cast<int32_t>(0);
    data.writeInt32(errorCode);
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnError(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleOnError_002
 * @tc.desc 测试HandleOnError方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnError_002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t errorCode = static_cast<int32_t>(0);
    data.writeInt32(errorCode);
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInt32), MockWriteInt32Fail);
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnError(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name HandleOnStopEdit_001
 * @tc.desc 测试HandleOnStopEdit方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnStopEdit_001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnStopEdit(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleOnStopEdit_002
 * @tc.desc 测试HandleOnStopEdit方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnStopEdit_002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t errorCode = static_cast<int32_t>(0);
    data.writeInt32(errorCode);
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInt32), MockWriteInt32Fail);
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnStopEdit(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name HandleOnExtensionStopped_001
 * @tc.desc 测试HandleOnExtensionStopped方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnExtensionStopped_001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnExtensionStopped(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleOnExtensionStopped_002
 * @tc.desc 测试HandleOnExtensionStopped方法
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnExtensionStopped_002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t errorCode = static_cast<int32_t>(0);
    data.writeInt32(errorCode);
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInt32), MockWriteInt32Fail);
    sptr<ObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnExtensionStopped(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}
}
}
}