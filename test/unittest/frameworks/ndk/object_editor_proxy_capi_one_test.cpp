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
#include "content_embed_proxy.h"
#include "mock_object_editor_client.h"
#include "mock_object_editor_document.h"
#include "mock_object_editor_manager.h"
#include "mock_object_editor_service.h"
#include "native_object_editor_types.h"
#include "object_editor_client.h"
#include "object_editor_config.h"
#include "object_editor_err_code.h"
#include "object_editor_extension_proxy.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

ErrCode MockStopObjectEditorExtensionPermissionDenied(
    std::unique_ptr<ObjectEditorDocument> &, const sptr<IObjectEditorService> &, const bool &)
{
    return ObjectEditorManagerErrCode::SA_PERMISSION_DENIED;
}

class ObjectEditorProxyCapiOneTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorProxyCapiOneTest::SetUpTestCase() {}

void ObjectEditorProxyCapiOneTest::TearDownTestCase() {}

void ObjectEditorProxyCapiOneTest::SetUp()
{
    ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
    config.isSupportObjectEditor_.isLoaded = true;
    config.isSupportObjectEditor_.value = true;
}

void ObjectEditorProxyCapiOneTest::TearDown() {}

/**
 * @tc.name: OHContentEmbedProxyStopWork_PermissionDenied
 * @tc.desc: Test StopWork when StopObjectEditorExtension returns SA_PERMISSION_DENIED
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorProxyCapiOneTest, OHContentEmbedProxyStopWork_PermissionDenied, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, StopObjectEditorExtension), MockStopObjectEditorExtensionPermissionDenied);
    
    ContentEmbed_ExtensionProxy *proxy = new ContentEmbed_ExtensionProxy();
    proxy->ceDocument = new ContentEmbed_Document();
    proxy->ceDocument->oeDocumentInner = std::make_unique<ObjectEditorDocument>();
    proxy->objectEditorService = nullptr;
    proxy->isPackageExtension = false;
    proxy->deathRecipient = nullptr;
    
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Proxy_StopWork(proxy);
    EXPECT_EQ(ret, CE_PERMISSION_DENIED);
    
    delete proxy->ceDocument;
    delete proxy;
}

} // namespace ObjectEditor
} // namespace OHOS