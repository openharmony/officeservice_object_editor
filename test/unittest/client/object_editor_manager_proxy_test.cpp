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
#include "mock_object_editor_client.h"
#include "object_editor_manager_proxy.h"
#include "stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<ObjectEditorManagerProxy> client_;
};

void ObjectEditorManagerProxyTest::SetUpTestCase()
{
}

void ObjectEditorManagerProxyTest::TearDownTestCase()
{
}

void ObjectEditorManagerProxyTest::SetUp()
{
    const sptr<IRemoteObject> remote = nullptr;
    client_ = std::make_unique<ObjectEditorManagerProxy>(remote);
}

void ObjectEditorManagerProxyTest::TearDown()
{
}

namespace {
bool MockWriteInterfaceToken()
{
    return false;
}

bool MockWriteParcelable()
{
    return false;
}

bool MockRemote()
{
    return false;
}

bool MockWriteString16()
{
    return false;
}

bool MockWriteString()
{
    return false;
}
/**
 * @tc.name StartObjectEditorExtension_001
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_001, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback = nullptr;
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartObjectEditorExtension_002
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_002, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_FAILED);
}

/**
 * @tc.name StartObjectEditorExtension_003
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_003, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteParcelable), MockWriteParcelable);
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name StartObjectEditorExtension_004
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_004, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name StartObjectEditorExtension_005
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_005, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartObjectEditorExtension_006
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_006, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StopObjectEditorExtension_001
 * @tc.desc 测试StopObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StopObjectEditorExtension_001, TestSize.Level1)
{
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    std::string documentId = "123456";
    ErrCode ret = client_->StopObjectEditorExtension(
        documentId, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StopObjectEditorExtension_002
 * @tc.desc 测试StopObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StopObjectEditorExtension_002, TestSize.Level1)
{
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    std::string documentId = "123456";
    ErrCode ret = client_->StopObjectEditorExtension(
        documentId, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StopObjectEditorExtension_002
 * @tc.desc 测试StopObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StopObjectEditorExtension_002, TestSize.Level1)
{
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    std::string documentId = "123456";
    ErrCode ret = client_->StopObjectEditorExtension(
        documentId, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetHmidByFileExtension_001
 * @tc.desc 测试GetHmidByFileExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetHmidByFileExtension_001, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->GetHmidByFileExtension(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetHmidByFileExtension_002
 * @tc.desc 测试GetHmidByFileExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetHmidByFileExtension_002, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteString16), MockWriteString16);
    ErrCode ret = client_->GetHmidByFileExtension(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetHmidByFileExtension_003
 * @tc.desc 测试GetHmidByFileExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetHmidByFileExtension_003, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->GetHmidByFileExtension(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetIconByHmid_001
 * @tc.desc 测试GetIconByHmid方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetIconByHmid_001, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->GetIconByHmid(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetIconByHmid_002
 * @tc.desc 测试GetIconByHmid方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetIconByHmid_002, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteString16), MockWriteString16);
    ErrCode ret = client_->GetIconByHmid(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetIconByHmid_003
 * @tc.desc 测试GetIconByHmid方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetIconByHmid_003, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->GetIconByHmid(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetFormatName_001
 * @tc.desc 测试GetFormatName方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetFormatName_001, TestSize.Level1)
{
    const std::string hmid;
    std::string locale;
    std::string formatName;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->GetFormatName(hmid, locale, formatName);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetFormatName_002
 * @tc.desc 测试GetFormatName方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetFormatName_002, TestSize.Level1)
{
    const std::string hmid;
    std::string locale;
    std::string formatName;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteString16), MockWriteString16);
    ErrCode ret = client_->GetFormatName(hmid, locale, formatName);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetFormatName_003
 * @tc.desc 测试GetFormatName方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetFormatName_003, TestSize.Level1)
{
    const std::string hmid;
    std::string locale;
    std::string formatName;
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->GetFormatName(hmid, locale, formatName);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetObjectEditorFormatByHmidAndLocale_001
 * @tc.desc 测试GetObjectEditorFormatByHmidAndLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatByHmidAndLocale_001, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    ErrCode ret = client_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetObjectEditorFormatByHmidAndLocale_002
 * @tc.desc 测试GetObjectEditorFormatByHmidAndLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatByHmidAndLocale_002, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetObjectEditorFormatByHmidAndLocale_003
 * @tc.desc 测试GetObjectEditorFormatByHmidAndLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatByHmidAndLocale_003, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteString16), MockWriteString16);
    ErrCode ret = client_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetObjectEditorFormatByHmidAndLocale_004
 * @tc.desc 测试GetObjectEditorFormatByHmidAndLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatByHmidAndLocale_004, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetObjectEditorFormatsByLocale_001
 * @tc.desc 测试GetObjectEditorFormatsByLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatsByLocale_001, TestSize.Level1)
{
    const std::string locale;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->GetObjectEditorFormatsByLocale(locale, formats);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetObjectEditorFormatsByLocale_002
 * @tc.desc 测试GetObjectEditorFormatsByLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatsByLocale_002, TestSize.Level1)
{
    const std::string locale;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteString16), MockWriteString16);
    ErrCode ret = client_->GetObjectEditorFormatsByLocale(locale, formats);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetObjectEditorFormatsByLocale_003
 * @tc.desc 测试GetObjectEditorFormatsByLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatsByLocale_003, TestSize.Level1)
{
    const std::string locale;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->GetObjectEditorFormatsByLocale(locale, formats);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartUIAbility_001
 * @tc.desc 测试StartUIAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartUIAbility_001, TestSize.Level1)
{
    const std::unique_ptr<AAFwk::Want> want = std::make_unique<AAFwk::Want>();
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->StartUIAbility(want);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartUIAbility_002
 * @tc.desc 测试StartUIAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartUIAbility_002, TestSize.Level1)
{
    const std::unique_ptr<AAFwk::Want> want = std::make_unique<AAFwk::Want>();
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteParcelable), MockWriteParcelable);
    ErrCode ret = client_->StartUIAbility(want);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name StartUIAbility_003
 * @tc.desc 测试StartUIAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartUIAbility_003, TestSize.Level1)
{
    const std::unique_ptr<AAFwk::Want> want = std::make_unique<AAFwk::Want>();
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->StartUIAbility(want);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}
}
}
}