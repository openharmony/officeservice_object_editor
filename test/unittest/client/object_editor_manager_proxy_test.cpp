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

[[maybe_unused]] bool MockWriteString()
{
    return false;
}
/**
 * @tc.name StartObjectEditorExtension_001
 * @tc.desc Test StartObjectEditorExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_001, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback = nullptr;
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = client_->StartObjectEditorExtension(
        document, objectEditorCallback, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartObjectEditorExtension_002
 * @tc.desc Test StartObjectEditorExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_002, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->StartObjectEditorExtension(
        document, objectEditorCallback, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name StartObjectEditorExtension_003
 * @tc.desc Test StartObjectEditorExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_003, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteParcelable), MockWriteParcelable);
    ErrCode ret = client_->StartObjectEditorExtension(
        document, objectEditorCallback, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name StartObjectEditorExtension_004
 * @tc.desc Test StartObjectEditorExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_004, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->StartObjectEditorExtension(
        document, objectEditorCallback, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name StartObjectEditorExtension_005
 * @tc.desc Test StartObjectEditorExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_005, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = client_->StartObjectEditorExtension(
        document, objectEditorCallback, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartObjectEditorExtension_006
 * @tc.desc Test StartObjectEditorExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartObjectEditorExtension_006, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = client_->StartObjectEditorExtension(
        document, objectEditorCallback, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StopObjectEditorExtension_001
 * @tc.desc Test StopObjectEditorExtension method
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
 * @tc.name StopObjectEditorExtension_003
 * @tc.desc Test StopObjectEditorExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StopObjectEditorExtension_003, TestSize.Level1)
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
 * @tc.name StopObjectEditorExtension_004
 * @tc.desc Test StopObjectEditorExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StopObjectEditorExtension_004, TestSize.Level1)
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
 * @tc.name GetOEidByFileExtension_001
 * @tc.desc Test GetOEidByFileExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetOEidByFileExtension_001, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->GetOEidByFileExtension(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetOEidByFileExtension_002
 * @tc.desc Test GetOEidByFileExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetOEidByFileExtension_002, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteString16), MockWriteString16);
    ErrCode ret = client_->GetOEidByFileExtension(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetOEidByFileExtension_003
 * @tc.desc Test GetOEidByFileExtension method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetOEidByFileExtension_003, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->GetOEidByFileExtension(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetIconByOEid_001
 * @tc.desc Test GetIconByOEid method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetIconByOEid_001, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->GetIconByOEid(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetIconByOEid_002
 * @tc.desc Test GetIconByOEid method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetIconByOEid_002, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteString16), MockWriteString16);
    ErrCode ret = client_->GetIconByOEid(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetIconByOEid_003
 * @tc.desc Test GetIconByOEid method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetIconByOEid_003, TestSize.Level1)
{
    const std::string hmid;
    std::string fileExtension;
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->GetIconByOEid(hmid, fileExtension);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetFormatName_001
 * @tc.desc Test GetFormatName method
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
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetFormatName_002
 * @tc.desc Test GetFormatName method
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
 * @tc.desc Test GetFormatName method
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
 * @tc.name GetObjectEditorFormatByOEidAndLocale_001
 * @tc.desc Test GetObjectEditorFormatByOEidAndLocale method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatByOEidAndLocale_001, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    ErrCode ret = client_->GetObjectEditorFormatByOEidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetObjectEditorFormatByOEidAndLocale_002
 * @tc.desc Test GetObjectEditorFormatByOEidAndLocale method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatByOEidAndLocale_002, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->GetObjectEditorFormatByOEidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetObjectEditorFormatByOEidAndLocale_003
 * @tc.desc Test GetObjectEditorFormatByOEidAndLocale method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatByOEidAndLocale_003, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteString16), MockWriteString16);
    ErrCode ret = client_->GetObjectEditorFormatByOEidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetObjectEditorFormatByOEidAndLocale_004
 * @tc.desc Test GetObjectEditorFormatByOEidAndLocale method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, GetObjectEditorFormatByOEidAndLocale_004, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->GetObjectEditorFormatByOEidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetObjectEditorFormatsByLocale_001
 * @tc.desc Test GetObjectEditorFormatsByLocale method
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
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetObjectEditorFormatsByLocale_002
 * @tc.desc Test GetObjectEditorFormatsByLocale method
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
 * @tc.desc Test GetObjectEditorFormatsByLocale method
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
 * @tc.desc Test StartUIAbility method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartUIAbility_001, TestSize.Level1)
{
    const std::unique_ptr<AAFwk::Want> want = std::make_unique<AAFwk::Want>();
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteInterfaceToken), MockWriteInterfaceToken);
    ErrCode ret = client_->StartUIAbility(std::make_unique<AAFwk::Want>(), sptr<IRemoteObject>(nullptr), 0);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartUIAbility_002
 * @tc.desc Test StartUIAbility method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartUIAbility_002, TestSize.Level1)
{
    const std::unique_ptr<AAFwk::Want> want = std::make_unique<AAFwk::Want>();
    Stub stub;
    stub.set(ADDR(MessageParcel, WriteParcelable), MockWriteParcelable);
    ErrCode ret = client_->StartUIAbility(std::make_unique<AAFwk::Want>(), sptr<IRemoteObject>(nullptr), 0);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartUIAbility_003
 * @tc.desc Test StartUIAbility method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorManagerProxyTest, StartUIAbility_003, TestSize.Level1)
{
    const std::unique_ptr<AAFwk::Want> want = std::make_unique<AAFwk::Want>();
    Stub stub;
    stub.set(ADDR(ObjectEditorManagerProxy, RemoteRequest), MockRemote);
    ErrCode ret = client_->StartUIAbility(std::make_unique<AAFwk::Want>(), sptr<IRemoteObject>(nullptr), 0);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}
}
}
}