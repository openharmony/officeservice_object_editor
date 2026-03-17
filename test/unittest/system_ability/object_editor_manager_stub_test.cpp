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
#define protected public
#include "mock_hilog.h"
#include "mock_message_parcel.h"
#include "object_editor_client_callback.h"
#include "object_editor_document.h"
#include "object_editor_manager_stub.h"
#include "object_editor_manager_system_ability.h"
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class MockObjectEditorManagerStub : public ObjectEditorManagerStub {
public:
    MockObjectEditorManagerStub() = default;
    ~MockObjectEditorManagerStub() = default;
    ErrCode StartObjectEditorExtension(
        std::unique_ptr<ObjectEditorDocument> &document,
        const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
        sptr<IRemoteObject> &oeExtensionRemoteObject,
        bool &isPackageExtension) override;
    ErrCode StopObjectEditorExtension(
        const std::string &documentId,
        const sptr<IRemoteObject> &oeExtensionRemoteObject,
        const bool &isPackageExtension) override;
    ErrCode GetHmidByFileExtension(
        const std::string &hmid,
        std::string &fileExtension) override;
    ErrCode GetIconByHmid(
        const std::string &hmid,
        std::string &resourceId) override;
    ErrCode GetFormatName(
        const std::string &hmid,
        const std::string &locale,
        std::string &formatName) override;
    ErrCode GetObjectEditorFormatByHmidAndLocale(
        const std::string &hmid,
        const std::string &locale,
        std::unique_ptr<ObjectEditorFormat> &format) override;
    ErrCode GetObjectEditorFormatsByLocale(
        const std::string &locale,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats) override;
    ErrCode StartUIAbility(const std::unique_ptr<AAFwk::Want> &want) override;
    int32_t CallbackEnter([[maybe_unused]] uint32_t code) override;
    int32_t CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result) override;

    ErrCode callbackEnterError = ERR_OK;
    ErrCode error = ERR_OK;
    std::string testcaseName;
    bool formatIsNull = false;
    uint32_t hookFormatsSize = 0;
};

class ObjectEditorManagerStubTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    sptr<MockObjectEditorManagerStub> stub_;
};

void ObjectEditorManagerStubTest::SetUpTestCase() {}

void ObjectEditorManagerStubTest::TearDownTestCase() {}

void ObjectEditorManagerStubTest::SetUp()
{
    stub_ = sptr<MockObjectEditorManagerStub>::MakeSptr();
    ASSERT_NE(stub_, nullptr);
    stub_->error = ERR_OK;
    stub_->callbackEnterError = ERR_OK;
    stub_->formatIsNull = false;
    stub_->hookFormatsSize = 0;
    logMsg.clear();
}

void ObjectEditorManagerStubTest::TearDown() {}

} // namespace ObjectEditor
} // namespace OHOS
