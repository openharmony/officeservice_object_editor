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
#include "abs_shared_result_set.h"

namespace OHOS {
namespace NativeRdb {

class MockRdbStore : public RdbStore {
public:
    MOCK_METHOD(int, BeginTransaction, (), (override));
    MOCK_METHOD(int, Commit, (), (override));
    MOCK_METHOD(int, RollBack, (), (override));
    MOCK_METHOD(int, ExecuteSql, (const std::string &sql, const Values &args), (override));
    MOCK_METHOD(std::shared_ptr<AbsSharedResultSet>, QuerySql,
        (const std::string &sql, const std::vector<ValueObject> &whereArgs), (override));
    MOCK_METHOD((std::pair<int, int64_t>), BatchInsert,
        (const std::string &table, const RefRows &rows, Resolution resolution), (override));
    MOCK_METHOD(int, Delete, (int &deletedRows, const AbsRdbPredicates &predicates), (override));
    MOCK_METHOD(int, GetVersion, (int &version), (override));
    MOCK_METHOD(int, SetVersion, (int version), (override));
};

class MockAbsSharedResultSet : public AbsSharedResultSet {
public:
    MOCK_METHOD(int, GoToFirstRow, (), (override));
    MOCK_METHOD(int, GoToNextRow, (), (override));
    MOCK_METHOD(int, GetRow, (RowEntity &rowEntity), (override));
};

} // namespace NativeRdb
} // namespace OHOS

#endif // MOCK_RDB_STORE_H