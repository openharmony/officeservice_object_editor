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
#include "object_editor_native_common.h"

#define private public
#include "content_embed_document.h"
#include "mock_object_editor_document.h"
#include "mock_storage.h"
#include "native_object_editor_types.h"
#include "object_editor_config.h"
#include "object_editor_document.h"
#include "stream.h"
#include "pole.h"
#include "storage.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorDocumentCapiOneTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorDocumentCapiOneTest::SetUpTestCase() {}

void ObjectEditorDocumentCapiOneTest::TearDownTestCase() {}

void ObjectEditorDocumentCapiOneTest::SetUp()
{
    ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
    config.isSupportObjectEditor_.isLoaded = true;
    config.isSupportObjectEditor_.value = true;
}

void ObjectEditorDocumentCapiOneTest::TearDown() {}



} // namespace ObjectEditor
} // namespace OHOS