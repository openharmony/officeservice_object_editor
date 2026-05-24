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

#ifndef MOCK_RDB_STORE_H
#define MOCK_RDB_STORE_H

#include <gmock/gmock.h>
#include "rdb_store.h"
#include "rdb_helper.h"
#include "abs_shared_result_set.h"
#include "values_bucket.h"
#include "abs_rdb_predicates.h"

namespace OHOS {
namespace NativeRdb {

class MockRdbStore : public RdbStore {
public:
    MockRdbStore() = default;
    ~MockRdbStore() override = default;

    MOCK_METHOD(int32_t, BeginTransaction, (), (override));
    MOCK_METHOD(int32_t, Commit, (), (override));
    MOCK_METHOD(int32_t, RollBack, (), (override));
    MOCK_METHOD(int32_t, ExecuteSql, (const std::string &sql), (override));
    MOCK_METHOD(std::shared_ptr<AbsSharedResultSet>, QuerySql,
        (const std::string &sql, const std::vector<ValueObject> &whereArgs), (override));
    MOCK_METHOD(std::pair<int32_t, int64_t>, BatchInsert,
        (const std::string &tableName, const std::vector<ValuesBucket> &values,
         ConflictResolution conflictResolution), (override));
    MOCK_METHOD(int32_t, Delete, (int32_t &deletedRows, const AbsRdbPredicates &predicates), (override));
    MOCK_METHOD(int32_t, Update, (int32_t &changedRows, const ValuesBucket &values,
         const AbsRdbPredicates &predicates), (override));
    MOCK_METHOD(int32_t, Insert, (int64_t &outRowId, const std::string &tableName,
         const ValuesBucket &values, ConflictResolution conflictResolution), (override));
};

class MockAbsSharedResultSet : public AbsSharedResultSet {
public:
    MockAbsSharedResultSet() = default;
    ~MockAbsSharedResultSet() override = default;

    MOCK_METHOD(int32_t, GoToFirstRow, (), (override));
    MOCK_METHOD(int32_t, GoToNextRow, (), (override));
    MOCK_METHOD(int32_t, GoToRow, (int32_t position), (override));
    MOCK_METHOD(int32_t, GetRow, (RowEntity &rowEntity), (override));
    MOCK_METHOD(int32_t, GetRowCount, (int32_t &count), (override));
    MOCK_METHOD(int32_t, GetColumnCount, (int32_t &count), (override));
    MOCK_METHOD(int32_t, GetAllColumnNames, (std::vector<std::string> &columnNames), (override));
    MOCK_METHOD(bool, IsClosed, (), (override));
    MOCK_METHOD(int32_t, Close, (), (override));
};

} // namespace NativeRdb
} // namespace OHOS

#endif // MOCK_RDB_STORE_H