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
 * @tc.name OnRemoteRequest_001
 * @tc.desc Test OnRemoteRequest method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, OnRemoteRequest_001, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    EXPECT_CALL(*stub, CallbackEnter(code)).WillOnce(Return(1));
    int32_t ret = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name OnRemoteRequest_002
 * @tc.desc Test OnRemoteRequest method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, OnRemoteRequest_002, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    EXPECT_CALL(*stub, CallbackEnter(code)).WillOnce(Return(ERR_NONE));
    EXPECT_CALL(*stub, CallbackExit(_, _)).WillOnce(Return(1));
    int32_t ret = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name OnRemoteRequest_003
 * @tc.desc Test OnRemoteRequest method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, OnRemoteRequest_003, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    EXPECT_CALL(*stub, CallbackEnter(code)).WillOnce(Return(ERR_NONE));
    EXPECT_CALL(*stub, CallbackExit(_, _)).WillOnce(Return(1));
    int32_t ret = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_TRANSACTION_FAILED);
}

/**
 * @tc.name OnRemoteRequestInner_001
 * @tc.desc Test OnRemoteRequest method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, OnRemoteRequestInner_001, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(u"com.example.InvalidEditor");
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name OnRemoteRequestInner_002
 * @tc.desc Test OnRemoteRequest method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, OnRemoteRequestInner_002, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(ObjectEditorClientCallbackStub::GetDescriptor());
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleOnUpdate_001
 * @tc.desc Test HandleOnUpdate method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnUpdate_001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnUpdate(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name HandleOnUpdate_002
 * @tc.desc Test HandleOnUpdate method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnUpdate_002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    auto document = std::make_unique<ObjectEditorDocument>();
    data.WriteParcelable(document.get());
    int32_t ret = stub->HandleOnUpdate(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name HandleOnUpdate_003
 * @tc.desc Test HandleOnUpdate method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnUpdate_003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    auto document = std::make_unique<ObjectEditorDocument>();
    data.WriteParcelable(document.get());
    Stub funcStub;
    funcStub.set(ADDR(MessageParcel, WriteInt32), MockWriteInt32Fail);
    int32_t ret = stub->HandleOnUpdate(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name HandleOnError_001
 * @tc.desc Test HandleOnError method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnError_001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t errorCode = static_cast<int32_t>(0);
    data.WriteInt32(errorCode);
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnError(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleOnError_002
 * @tc.desc Test HandleOnError method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnError_002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t errorCode = static_cast<int32_t>(0);
    data.WriteInt32(errorCode);
    Stub funcStub;
    funcStub.set(ADDR(MessageParcel, WriteInt32), MockWriteInt32Fail);
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnError(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleOnStopEdit_001
 * @tc.desc Test HandleOnStopEdit method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnStopEdit_001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnStopEdit(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleOnStopEdit_002
 * @tc.desc Test HandleOnStopEdit method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnStopEdit_002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t errorCode = static_cast<int32_t>(0);
    data.WriteInt32(errorCode);
    Stub funcStub;
    funcStub.set(ADDR(MessageParcel, WriteInt32), MockWriteInt32Fail);
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnStopEdit(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleOnExtensionStopped_001
 * @tc.desc Test HandleOnExtensionStopped method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnExtensionStopped_001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnExtensionStopped(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name HandleOnExtensionStopped_002
 * @tc.desc Test HandleOnExtensionStopped method
 * @tc.type FUNC
 */

HWTEST_F(ObjectEditorClientCallbackStubTest, HandleOnExtensionStopped_002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t errorCode = static_cast<int32_t>(0);
    data.WriteInt32(errorCode);
    Stub funcStub;
    funcStub.set(ADDR(MessageParcel, WriteInt32), MockWriteInt32Fail);
    sptr<MockObjectEditorClientCallbackStub> stub = sptr<MockObjectEditorClientCallbackStub>::MakeSptr();
    int32_t ret = stub->HandleOnExtensionStopped(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}
}
}
}