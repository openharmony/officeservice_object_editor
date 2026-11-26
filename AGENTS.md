# AGENTS.md — object_editor（对象编辑）

> 路径: `foundation/officeservice/object_editor` | 子系统: `officeservice` | SysCap: `SystemCapability.ContentEmbed.ObjectEditor` | 仅标准系统
>
> **维护规则**: 使用 agent 进行代码修改时，如新增代码结构或功能特性，必须同步更新本文档及 `docs/knowledge/` 下对应知识文档。

## 代码结构

```
interfaces/
  kits/c/                  # 对外 NDK 头 — content_embed_{common,proxy,document,extension}.h (@since 24)
  innerkits/include/       # 内部接口 — 三条 IPC 通道、错误码、常量（无 GN 目标，纯头文件）
frameworks/
  ndk/                     # NDK C API 实现 — 安装为 ndk/libcontent_embed_ndk.so
  kits/extension/          # OE Extension 运行时框架（type 511）
extension_ability/         # Extension 进程 IPC 骨架（stub + 反调 proxy；编入 content_embed_extension）
client/                    # 客户端库 — ObjectEditorClient 单例、ManagerProxy、回调 Stub、死亡监听
document/                  # MS-CFB 复合文档引擎（StorageIO/DirTree/AllocTable/Stream）
package/                   # 通用包兜底（Ole10Native + inotify；编入 client .so）
system_ability/            # SA 66528 主体 — 路由/连接池/限流/空闲退出（libcontent_embed_service.z.so）
database/                  # 注册库（RDB + resmgr + content_embed_config 解析）
common/                    # ObjectEditorConfig(DLP)、UserMgr、SystemUtils、ExtensionProxy
utils/                     # 权限校验、BMS 工具、事件管理、HiLog 封装（域 0xD005118）
sa_profile/                # 66528.json（bindercall 按需拉起、auto-restart=false）
etc/                       # object_editor_service.cfg + diversion_map.json（OLE 改道表）
test/                      # unittest（40 套）+ fuzztest（18 个 NDK proxy fuzzer）+ mock/
```

> 若子目录包含自己的 `AGENTS.md`，则该目录范围内的任务以子目录文档为准。

| 构建目标（`part:object_editor`） | 类型 | 产物 |
|---|---|---|
| `frameworks/ndk:content_embed_ndk` | 共享库 | `ndk/libcontent_embed_ndk.so`（对外 C API） |
| `frameworks/kits/extension:content_embed_extension` | 共享库 | `libcontent_embed_extension.z.so`（Extension 框架 + extension_ability 骨架） |
| `frameworks/kits/extension:content_embed_extension_module` | 共享库 | dlopen 入口（`extensionability/`，type 511） |
| `client:content_embed_client` | 共享库 | `libcontent_embed_client.z.so`（含 document/package/common 源码） |
| `system_ability:content_embed_service` | 共享库 | `libcontent_embed_service.z.so`（SA 主体，`use_exceptions`） |
| `utils:content_embed_utils` | 共享库 | 权限/事件/日志工具 |
| `sa_profile:object_editor_service_sa_profile` | profile | `/system/profile/object_editor_service.json` |
| `etc:object_editor_service_etc` / `:object_editor_service_diversion` | 配置 | init cfg / `diversion_map.json` |
| `test:test_target` | group | `unittest:unittest` + `fuzztest:fuzztest` |

`bundle.json` 的 `group_type`（fwk/service/test）是目标清单事实源；依赖 27 个 OH 组件。

## 知识路由

| 问题 | 先读 |
|---|---|
| 三条 IPC 通道（Manager 9 命令 / Extension 7 命令 / Callback 4 命令） | `interfaces/innerkits/include/iobject_editor_manager.h` → `object_editor_manager_stub.cpp`（手写分发表） |
| 会话建立全流程（StartWork → SA 路由 → AMS 拉起 → Initial） | `client/src/object_editor_client.cpp` → `system_ability/src/object_editor_manager_system_ability.cpp` |
| 格式选择策略（默认应用/最新注册/diversion/通用包） | `GetObjectEditorFormat` + `HandleDefaultAppFormatPolicy`（`object_editor_manager_system_ability.cpp`） |
| 注册数据入库与 BMS 事件 | `database/src/object_editor_manager_database.cpp` → `object_editor_manager_utils.cpp`（`content_embed_config` 解析） |
| CFB 复合文档读写 | `document/src/storage.cpp`（`StorageIO::Load/Flush`）→ `dirtree.cpp` → `stream.cpp` |
| OEID 存储与序列化 | `document/src/object_editor_document.cpp`（`FlushOEid`）→ `document/src/utils.cpp` |
| Extension 生命周期与回调 | `frameworks/kits/extension/src/object_editor_extension.cpp` → `extension_ability/src/object_editor_extension_stub.cpp` |
| 客户端回调分发与销毁竞态 | `client/src/object_editor_client_callback.cpp`（`DispatchFlagGuard`） |
| 空闲回收（Extension 20s / SA 600s） | `system_ability/src/object_editor_connection.cpp` → `object_editor_manager_system_ability.cpp`（`TimerThreadStopSA`） |
| 错误码 | `interfaces/kits/c/content_embed_common.h`（对外）+ `object_editor_err_code.h`（内部四族）+ `frameworks/ndk/src/object_editor_native_common.cpp`（映射表） |
| 权限/限流/调用方校验 | `object_editor_manager_system_ability.cpp`（`CallbackEnter`）+ `utils/src/object_editor_permission_utils.cpp` |
| DLP 拦截 | `common/src/object_editor_config.cpp`（唯一消费点，fail-closed） |
| 快照传递机制 | `frameworks/ndk/src/object_editor_proxy_capi.cpp`（GetSnapshot）+ `extension_capi.cpp`（SetSnapshot，PNG 走文件不走 parcel） |
| mock 体系 | `test/mock/`（链接期符号抢占 + gmock + `stub.h` 二进制补桩三种机制） |

### docs/knowledge 文档路由

| 问题 | 先读 |
|---|---|
| OE、OEID、会话、diversion 等术语的当前仓语义 | [术语表](docs/knowledge/glossary.md) |
| 业务定位、职责边界、输入输出与协作服务 | [业务背景](docs/knowledge/business-context.md) |
| 架构分层、关键流程、非功能设计与构建入口 | [架构设计及约束](docs/knowledge/architecture.md) |
| 修改代码前的 IPC/并发/生命周期/安全/格式约束 | [编码铁律](docs/knowledge/coding-standards.md) |

按任务选择文档：先查[术语表](docs/knowledge/glossary.md)统一语义，再读[业务背景](docs/knowledge/business-context.md)与[架构](docs/knowledge/architecture.md)定位模块，最后沿文中源码路径深入；修改实现前核对[编码铁律](docs/knowledge/coding-standards.md)。上述知识用于导航，具体行为仍需结合当前源码与实际配置确认。

**关键术语**: OE（对象编辑框架）、OEID（OE 文档标识，存根目录项 CLSID）、OE Extension（type 511 contentEmbed）、PACKAGE_OEID（`0003000C-...` 通用包兜底）、diversion（Windows OLE CLSID 改道）、isLinking（嵌入 vs 链接）、OESA_UID（7061，SA 固定 UID）、documentId（UUID v4 会话键）。

**按任务类型定位**:

| 任务 | 起点 |
|---|---|
| 新增/修改 IPC 命令 | `iobject_editor_manager.h`（枚举+接口）→ stub → proxy → SA 实现（4 处同步，见宪法 1） |
| 新增 OE 服务端格式支持 | `content_embed_config.json` 字段 → `object_editor_manager_utils.cpp`（解析）→ RDB 表 |
| 修改复合文件格式行为 | `document/src/storage.cpp`（Flush 写序）→ 对应测试套 `test/unittest/document/` |
| 修复会话/回调竞态 | `client/src/object_editor_client_callback.cpp` → `object_editor_client.cpp`（`PrepareForDestroy`） |
| 调整生命周期/空闲策略 | `object_editor_connection.cpp`（20s）+ `object_editor_manager_system_ability.cpp`（600s） |
| 新增 NDK C API | `interfaces/kits/c/` 头 → `frameworks/ndk/src/*_capi.cpp` → fuzzer（`test/fuzztest/frameworks/ndk/proxy/`） |

**编辑任何文件前，先声明**: (1) 任务类别、(2) 已读文档（本文 + docs/knowledge 对应篇）、(3) 适用约束（项目宪法条目编号）。

## 项目宪法（专家约束）

1. **IPC 面四处同步**: 三个 `.idl` 文件**均无 GN 目标引用**，stub/proxy 全手写。改 Manager IPC 面 = 同步改 `iobject_editor_manager.h`（接口 + `IObjectEditorManagerIpcCode`）、`object_editor_manager_stub.cpp`（分发表 + handler）、`object_editor_manager_proxy.cpp`、SA 实现。Extension/Callback 通道同理。
2. **IPC 命令只追加**: 新命令追加到 `IPC_CODE_END` 前，禁止重排/删除既有枚举——重排会静默破坏全系统跨进程 IPC。
3. **接口令牌与命令校验前置**: 所有 proxy `WriteInterfaceToken` + `TF_SYNC`；所有 stub 先校验 descriptor 与 code 范围再解 parcel；Extension 侧新命令必须校验 `GetCallingUid()==OESA_UID(7061)` 或 `callerTokenId`。
4. **回调锁外执行**: 客户端 `callbackMutex_` 锁内只取指针/查表/置 `DispatchFlagGuard` 标志，用户 C 回调在锁外（binder 线程）执行；回调内禁止调用会获取该锁的 API。
5. **禁止 detach 线程**: 新增长生命周期线程必须可 join；现有 `TimerThreadStopSA` 的 detach+捕获 this 是已知缺陷，禁止复制。
6. **销毁顺序**: 持客户端指针的 death recipient 先 `RemoveDeathRecipient` 再销毁对象（`DestroyExtensionProxy/StopWork` 现有序序不可调换）；`PrepareForDestroy` 分发中拒绝销毁的语义必须保留。
7. **OEID 字段语义**: OEID = 根目录项 CLSID（`DirTree` index 0）；`Storage::Clsid()` 读的是文件头 CLSID，两者不同字段，禁止混用。GUID 序列化按前 3 组小端反转的既定规则。
8. **Flush 事务写序**: 目录树 → MiniFAT → FAT → DIFAT → Header 最后；失败仅回滚内存。禁止在序列中插入落盘步骤。碎片回收 `RebuildAndFlush` 的“新建-拷贝-验证-替换”顺序不可省略验证。
9. **DLP fail-closed**: 查询失败一律按“在 DLP 沙箱”处理；`WITH_DLP` 仅 `system_ability`/`frameworks/ndk` 定义（`object_editor_dlp_part_enabled`，见 `object_editor.gni`）；client .so 内同源码无宏，修改 `object_editor_config.cpp` 需考虑双编译环境。
10. **安全前置**: 服务端处理文件路径前必须 `SystemUtils::ValidateAndNormalizePath`（防沙箱逃逸）；文件后缀过 `IsValidFileExt`（防 SQL LIKE 注入）；新 IPC 命令接入 `CallbackEnter` 权限矩阵与 50 req/s 限流；注册来源保持 `REGISTER_OBJECTEDITOR_EXTENSION` 准入校验。
11. **错误码转换**: 内部四族（3531xxx/3532xxx/3533xxx/3534xxx）跨层必须经 `ConvertErrorToCode`/`ValidateErrorCode` 转为 `CE_ERR_*` 对外码；禁止往 `SA_CONNECT_LIMIT_EXCEED` 上叠加新语义（该码已混用三种失败场景）。
12. **安全构建配置**: 生产库保持 `branch_protector_ret = "pac_ret"` + `sanitize { cfi = true; cfi_cross_dso = true; }`；豁免需具体理由。DLP 代码必须 `#ifdef WITH_DLP` 包裹。
13. **禁止魔数**: 复用已定义常量（`OBJECT_EDITOR_SERVICE_SA_ID=66528`、`LOAD_TIMEOUT_MS=5000`、`SA_EXIT_TIME_S=600`、`RETRY_TIMES`、`CONNECT_TIMEOUT` 等）；新增数值常量一律命名定义。
14. **同名源码双编译**: `object_editor_config.cpp`/`object_editor_document.cpp` 等同时编入 client .so（无 WITH_DLP）与 ndk .so（有）——修改需确认两处编译语义一致。
15. **修改业务实现必须运行受影响测试套**（`test/unittest/<module>/` 对应 `ohos_unittest` target）；报告完成前给出已验证/未验证清单。

**反模式**: 改 IDL 期望重新生成 stub（IDL 无人消费）· 按命令枚举推断 SA 行为（部分命令无 case 或空实现）· 在 `callbackMutex_` 锁内调用户回调 · detach 线程捕获 this · 绕过 `PrepareForDestroy` 强删分发中的 proxy · 把 OEID 当文件头 CLSID 读 · 后缀字符串直接拼 SQL · 客户端回调内做同步反向 IPC。

## 编译与测试

`./build.sh` 是 **OpenHarmony 源码树顶层脚本，不在本仓**。GN/build 命令须从源码树根执行，本仓检出路径必须为 `foundation/officeservice/object_editor`（`object_editor.gni` 的 `object_editor_path` 指向它）。GN 标签以前缀 `//foundation/officeservice/object_editor/...` 引用兄弟模块。

```bash
# 从 OpenHarmony 源码树根，不是本仓：
./build.sh --product-name {product_name} --ccache --build-target object_editor
# 64 位 ARM：
./build.sh --product-name {product_name} --ccache --target-cpu arm64 --build-target object_editor
# 单个测试套（套名见 test/unittest/<module>/BUILD.gn）：
./build.sh --product-name {product_name} --build-target object_editor_client_test
```

组件默认编译；退出需改源码树 `productdefine/common/inherit/rich.json`（移除 `object_editor`）。特性开关：`object_editor_dlp_part_enabled`（`object_editor.gni`，默认 true，无 dlp part 时 false）、`object_editor_feature_coverage`（`resources/config/build/BUILD.gn`，默认 false）、`is_emulator`（GN 内建，定义 `IS_EMULATOR`，放宽 linking 路径校验）。

**服务身份**（调试时相关）: SA 66528，进程 `object_editor_service`（`sa_main` 启动），UID/GID `object_editor_service`，SELinux `u:r:ohos_object_editor_service:s0`，APL `system_basic`，权限 `GET_BUNDLE_INFO_PRIVILEGED`/`GRANT_URI_PERMISSION_PRIVILEGED`/`GET_DEFAULT_APPLICATION`/`START_ABILITIES_FROM_BACKGROUND`/`MANAGE_LOCAL_ACCOUNTS`，数据目录 `/data/service/el2/public/object_editor_service`（init job 只建顶层，`database/<userId>/` 由 `HandleOpenDb` 创建）。`bindercall` 按需拉起、`auto-restart=false`（崩溃后靠客户端 `LoadSystemAbility` 恢复）；**空闲自退出**：600s 无 IPC 且无活跃连接 → `UnloadSystemAbility`（`OnIdle()` 为空实现，不依赖 samgr idle）；Extension 空闲 20s 停用；停止原因查询窗口 60s。

**测试体系**:
- 单测：40 个 `ohos_unittest` target，`module_out_path = "object_editor/object_editor"`，按模块分布 `test/unittest/<module>/`（client 5、common 5、database 3、document 8、extension_ability 2、frameworks/extension 3、frameworks/ndk 3、package 3、system_ability 4、utils 4 套）。
- Fuzz：18 个 fuzzer，仅覆盖 NDK proxy 入口（`test/fuzztest/frameworks/ndk/proxy/`），目录名 `ohcontentembed*_fuzzer`、内部 cpp 为 `oh_contentembed_*.cpp`。
- **mock 复用**：`test/mock/` 已有三种机制——链接期符号抢占（`message_parcel/`、`ability_manager_client/`、`system_ability/`、`access_token/`、`ipc_skeleton/`、`hisysevent/`）、gmock（`bundle_mgr/`、`rdb/`、`resource_manager/` 及各接口 mock）、二进制补桩（`stub.h`，mprotect 改代码段）。优先复用，勿手搓替代。注意 `mock_object_editor_client.h`/`mock_object_editor_extension_proxy.h` 文件名与实际 mock 内容不符，用前先读。
- **`private=public` 约定是有意为之，不是坏味道**：多数 system_ability 测试 target 设 `defines = ["private=public"]`；共享 `testcase_flags`（`-Wall -Werror -g3 -Dprivate=public -Dprotected=public`）仅被 unittest 引用（fuzz 不引用）。NDK 测试另有源内局部 `#define private public` 只包特定 include；mock `final` 类依赖 `#define final` 空宏。
- 覆盖率由 `object_editor_feature_coverage` 开关（`resources/config/build:coverage_flags`）。
- DLP 分支在单测中不可达（无测试 target 定义 `WITH_DLP`），相关单测恒走 false 分支。

**README.md 是面向用户的规范**（中文）：服务生命周期、OE Extension 注册、文档嵌入、文档编辑四大流程及 `OH_ContentEmbed_*` API 时序以它为准；官方文档在 `docs/zh-cn/application-dev/content-embed/`（本机 `/home/wxg/docs/`）。本仓无 `opencode.json`。

## Git 提交规则

- **需用户批准**：`git commit` 前先询问；批准后使用 `git commit -s`。
- **Angular 格式**：`type(scope): subject`（feat/fix/docs/style/refactor/test/chore）。
- **共同署名**：每个提交信息追加 `Co-Authored-By: Agent`。

**完成标准**: 报告完成前：(1) 展示编译退出码 0（或说明未编译及原因）；(2) 展示受影响的 `object_editor_*_test` 套通过（或说明未运行）；(3) 若改了 IPC/错误码/DFX 路径，核对宪法 1/2/11 条同步情况；(4) 无法运行验证时，明确列出已验证/未验证项与残留风险。
