# 编码铁律

本页是修改本仓代码前必须核对的项目宪法（领域约束 + 风格基线）。发现文档与实现差异时以当前源码为准并回改本文。

## IPC 面

- **三个 IDL 文件（`system_ability/IObjectEditorManager.idl`、`extension_ability/IObjectEditorExtension.idl`、`client/IObjectEditorClientCallback.idl`）均无 GN 目标引用，stub/proxy 全部手写。** “改 IDL 等待重新生成”在本仓不成立。改 Manager IPC 面必须同步 4 处：`interfaces/innerkits/include/iobject_editor_manager.h`（接口+命令枚举，新命令只能**追加**到 `IPC_CODE_END` 前）、`system_ability/src/object_editor_manager_stub.cpp` 分发表与 handler、`client/src/object_editor_manager_proxy.cpp`、SA 实现。Extension/Callback 通道同理（各自 3 处）。
- stub 分发表目前缺 `COMMAND_GET_FORMAT_NAME` 的 case（落入 `IPCObjectStub::OnRemoteRequest`）；SA 侧 `GetOEidByFileExtension/GetIconByOEid/GetFormatName` 是返回 `SA_OK` 的空实现。新增命令前先核对“枚举有、分发无、实现空”的现状，不要按枚举推断行为。
- 所有 proxy 方法 `TF_SYNC` + `WriteInterfaceToken(GetDescriptor())`；所有 stub 校验接口令牌与命令范围后再解 parcel。回调通道同样 `TF_SYNC`——extension 调用线程阻塞到客户端回调返回，客户端回调里禁止再做同步反向 IPC（会串行化整个链路）。
- 跨进程传文档只传 `ObjectEditorDocument` 的 8 个元数据字段（`Marshalling` 顺序：oeid、isLinking、tmpFileUri、oriFileUri、snapshotUri、nativeFileUri、operateType、documentId）。新增字段必须同时改 Marshalling/Unmarshalling 与所有 mock 的 parcel 假实现。

## 并发与线程

- 本仓无 FFRT，线程一律 `std::thread`；新增长生命周期线程必须可 join（参考 `cleanThread_`、每连接 `timerThread_`）。**禁止新增 detach 线程**——现有 `TimerThreadStopSA` 的 detach + 捕获 `this` 是已知缺陷（SA 停止后线程仍可能存活并调 `UnloadSystemAbility`），不要复制该模式。
- 客户端 `callbackMutex_` 纪律：锁内只做“取指针/查表/置分发标志”，**用户 C 回调必须在锁外执行**（`DispatchFlagGuard` 模式）；回调内调用会获取该锁的 API 会死锁（头文件注释明示）。
- `ObjectEditorDocument` 模式：`docMutex_` 内拷贝 `shared_ptr<Storage>`，锁外做文件 I/O——防止 `RestoreStorage`/`RebuildAndFlush` 并发替换导致 use-after-free。修改文档对象的新方法沿用该模式。
- 锁序注意：`OnAbilityConnectDone` 持 `extensionProxyMutex_` 时进入定时器锁，而 `TimerThreadStopExtension` 反向先取定时器锁再取 `extensionProxyMutex_`。改这两个文件前先画锁序图；`ResetStopSATimer` 的 notify 未持 `mutexTimer_`，靠 `wait_for` 超时兜底，勿据此假设唤醒即时。
- `DirTree::current_` 是 Storage 级共享游标（`EnterDirectory/LeaveDirectory` 改全局当前目录）：遍历时必须用“保存-恢复路径”模式，禁止假设游标在多步操作间不被移动。

## 生命周期与资源

- **删除 death recipient 前先移除注册**：`ObjectEditorExtensionDeathRecipient` 持裸 `proxy_`；`DestroyExtensionProxy/StopWork` 的 `RemoveDeathRecipient` 顺序不可调换。新增持客户端指针的 recipient 时沿用“先解绑后销毁”。
- Extension 进程侧 UAF 防护三件套必须保留：`isStopping_`（`OnStop` 先置位）、`weak_ptr::expired()` 预检后 `lock()`、回调表锁内做 shared_ptr 快照、锁外遍历调用。
- 回调分发中销毁对象：`PrepareForDestroy` 在锁内查 `IsDispatching()`，分发中拒绝销毁返回 `CE_ERR_PARAM_INVALID`——调用方重试，不要绕过该检查强删。
- 停止原因登记窗口仅 60 秒（`CLEAN_THRESHOLD_MS=60000`）：`QueryExtensionStopReason` 的调用方（客户端死亡监听）依赖此窗口判定 `SA_CLEAN_IDLE` 静默。改动清理周期前评估该消费者。
- 临时文件布局 `<TempDir>/<documentId>/{ole.bin, snapshot.png, native/}` 由 `PrepareFiles` 建立并被 SA/Extension 侧 `access(R_OK|W_OK)` 校验；新增会话文件必须同时加入 `CheckClientFileValid` 的 URI 校验清单，否则 Extension 侧读不到。
- `AddDeathRecipient` 失败当前仅打日志继续（退化为定时器回收）：接受该兜底语义，但不得移除后续的 20s/600s 定时器路径。

## 错误处理

- 三套错误码并存：IPC 框架码（`ERR_INVALID_DATA` 等，stub parcel 失败）、内部四族（Client 3531xxx / SA 3532xxx / Extension 3533xxx / Document 3534xxx，`interfaces/innerkits/include/object_editor_err_code.h`）、对外 C 码（`CE_ERR_*`）。跨层返回必须经 `ConvertErrorToCode`（`frameworks/ndk/src/object_editor_native_common.cpp` 映射表）或 `ValidateErrorCode`（入站），禁止直接把内部码透传给 C API 调用者。
- 已知语义复用：`EXTENSION_MODULE_LOAD_FAILED` 与“设备不支持”都映射为 `SA_CONNECT_LIMIT_EXCEED`（限流/不支持/模块加载失败不可区分）。新代码禁止继续往该错误码上叠语义；修复方向是拆分错误码并同步四族枚举与映射表。
- `ObjectEditorPackage::SaveData` 的 `withData` 用成员 `dataSize_`（仅解析路径赋值）而非本次 `fs::file_size` 计算（`package/src/package_data.cpp`）——CREATE_BY_FILE 路径恒走整块内存 buffer。触碰该文件时先修正或保持测试同步，勿按变量名推断行为。
- parcel 失败、DB 失败、connect 失败的返回路径都必须清理已分配资源（参考 `CleanupTempFiles`、`RestoreFlushState`）；`AddBundle/UpdateBundle` 失败静默 return 是 BMS 事件线程现状，新事件处理不要沿用“无返回值无上报”的写法。

## 安全与权限

- DLP fail-closed 不可放松：`CheckIsInDlp/CheckCallerInDlpSandbox` 查询失败一律按“在 DLP 中”处理。`WITH_DLP` 仅在 `system_ability` 与 `frameworks/ndk` 两个目标定义；`client` .so 内的同名代码无该宏，DLP 门槛实际由 NDK 副本执行——改 `common/src/object_editor_config.cpp` 时两处编译环境都要考虑。
- 新增 IPC 命令必须接入 `CallbackEnter` 的权限矩阵（普通命令 `CONNECT_OBJECTEDITOR_EXTENSION`，StartUIAbility 用 `PERMISSION_SERVER`）与 50 req/s 限流；范围外 code 一律拒绝。
- 服务端处理任何文件路径前必须过 `SystemUtils::ValidateAndNormalizePath`（父目录 realpath + 前缀校验防沙箱逃逸）；拼接文件名用 `fs::path::filename()` 防穿越（参考 `WriteFileToSandbox`）。
- 文件后缀入库前过 `IsValidFileExt`（仅字母数字与 `.`，配合 `HasSQLWildcard` 拒绝 `%`/`_`，防 LIKE 注入）；DB 查询一律参数化。
- Extension 侧新增命令处理必须校验 `IPCSkeleton::GetCallingUid()==OESA_UID(7061)` 或 `callingTokenID==callerTokenId`（参考 `Initial/Close/GetSnapshot` 的双通道校验）。
- 注册准入依赖 `CheckRequestPermission(bundleName, REGISTER_OBJECTEDITOR_EXTENSION)`：新增注册来源（事件类型）时保持该校验前置。

## 数据与格式

- OEID 语义不可混淆：OEID 存 **根目录项 CLSID**（`FlushOEid`/`ConfigMinimalCd`），`Storage::Clsid()` 读的是**文件头 CLSID**——两者是不同字段。GUID 序列化按字节序规则（前 3 组小端反转），改 `document/src/utils.cpp` 前先写往返单测。
- `StorageIO::Flush` 的写序是事务设计的一部分：目录树 → MiniFAT → FAT → DIFAT → **Header 最后**；失败仅回滚内存。不要在 Flush 序列中间插入新的落盘步骤。`DeleteEntry` 立即 Flush 是现状例外，新增删除语义保持一致。
- `AtomicReplaceFile` 实际是 `fs::copy_file` 覆盖（非原子 rename）；`ShouldRebuild` 阈值（>64KB 且浪费 ≥8MB 且浪费×10 ≥ 文件×3）与 `RebuildAndFlush` 的“新建-拷贝-验证-替换”顺序是防半写的唯一手段，不可省略验证步骤。
- 流升缩容单向：mini 流可升级为大块链（`ConvertSmallToBig`），**大→小降级不支持**（返回 InvalidOperation）。写依赖该约束的代码时显式检查。
- 命名与长度硬约束：DirEntry 名 ≤31 个 UTF-16 单元且禁 `/ \ : !`；oeid 长度 32 或 36；C API 出参缓冲 ≥ `MAX_OEID_LENGTH(40)`/`MAX_PATH_LENGTH(4096)`/`MAX_NAME_LENGTH(1024)`。新增校验在 NDK 入口做，引擎层不重复。
- 大 I/O 分块常量表已存在（流拷贝 1MB、package 提取/回写 4MB、写缓冲 16MB、单次读写 2GB 上限）：复用常量，不再引入字面量。

## 构建与特性开关

- 新增生产 `ohos_shared_library` 保持 `branch_protector_ret = "pac_ret"` 与 `sanitize { cfi = true; cfi_cross_dso = true; }`（service/ndk/extension 现状；extension 额外开 boundary/integer/ubsan）。豁免需具体理由。
- `WITH_DLP`（`object_editor.gni` 的 `object_editor_dlp_part_enabled`）与 `IS_EMULATOR`（GN 内建 `is_emulator`）是仅有的特性开关；DLP 相关代码必须 `#ifdef WITH_DLP` 包裹并给出无宏时的 false 分支。
- 同名 cpp 编进多个 .so（client 与 ndk 各编译 `object_editor_config.cpp` 等）：修改这些文件时确认两处编译配置（宏、deps）语义一致，防止“同一函数两个进程内行为不同”。
- 修改业务实现必须运行受影响测试套（`test/unittest/<module>/` 对应 target）；测试目标自带 `-Wall -Werror -Dprivate=public -Dprotected=public`（`testcase_flags`），fuzz 目标不带。

## 代码风格基线

- C++17；**列宽 ≤120**；缩进 4 空格禁 Tab；类/方法 PascalCase，成员变量 camelCase_，常量 UPPER_SNAKE_CASE，文件名 snake_case 且跨模块文件带 `object_editor_` 前缀。
- include 顺序：对应头 → std → OH 框架 → `interfaces/` → 其它内部。
- 函数 ≤50 行代码（不含空行/注释）；平均圈复杂度 ≤20；嵌套 ≤5 层——用早返回、guard、提取助手压平。
- **禁止魔数**：新增数值常量一律命名（复用 `OBJECT_EDITOR_SERVICE_SA_ID`、`LOAD_TIMEOUT_MS`、`RETRY_TIMES`、`CONNECT_TIMEOUT` 等，见 `interfaces/innerkits/include/iobject_editor_manager.h` 与各模块头部）。
- 拷贝定长缓冲使用 `memcpy`（本仓约定，非 `strcpy`）；**宏参数一律加括号**（`#define MAX(x) ((x) > (x) ? ...)` 风格）。
