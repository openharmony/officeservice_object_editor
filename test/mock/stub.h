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

#ifndef STUB_H
#define STUB_H

#include <map>
#include <string>
#include <functional>
#include <cstdint>

class Stub {
public:
    using FuncType = std::function<void()>;

    void set(const std::string &addr, FuncType func)
    {
        stubs_[addr] = func;
    }

    bool has(const std::string &addr) const
    {
        return stubs_.find(addr) != stubs_.end();
    }

    FuncType get(const std::string &addr) const
    {
        auto it = stubs_.find(addr);
        if (it != stubs_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void reset()
    {
        stubs_.clear();
    }

private:
    std::map<std::string, FuncType> stubs_;
};

#define ADDR(cls, method) (std::string(#cls) + "::" + #method)

#endif // STUB_H
