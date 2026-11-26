# 术语表

业务、实体与机制的共同词汇。源码位置见[架构设计及约束](architecture.md)，行为背景见[业务背景](business-context.md)；修改实现前查阅[编码铁律](coding-standards.md)。

## OE（Object Editor / 对象编辑）

OpenHarmony 提供的应用间文档嵌入与协同编辑框架，本仓全部代码的实现对象。对外以 Content Embed Kit（NDK `libcontent_embed_ndk.so`，API 24 起）交付；部件名 `object_editor`，SysCap `SystemCapability.ContentEmbed.ObjectEditor`。

## OE 客户端应用 / OE 服务端应用

前者是嵌入外部文档的宿主应用，调用 `OH_ContentEmbed_*` 客户端接口；后者注册 OE Extension、由系统按需拉起，为前者提供特定格式文档的嵌入与编辑。判定依据是应用角色，不是 UID——同一个应用可以同时是两者。

## OE 文档（OE Document）

被嵌入文档在框架中的程序对象，C 侧 `ContentEmbed_Document`，内部包装 `document/src/object_editor_document.cpp` 的 `ObjectEditorDocument`（Parcelable）。跨进程只序列化 7 个字符串/枚举字段（oeid、isLinking、四个 file URI、operateType、documentId），复合文件本体留在磁盘，不走 IPC。

## OE 格式文件（OE 格式 / CD 文件）

OE 文档的持久化形态：MS-CFB 复合文档（魔数 `D0CF11E0A1B11AE1`，小端，v3=512B 扇区 / v4=4096B，MiniFAT 64B，cutoff 4096）。引擎在 `document/src/storage.cpp`。注意 NDK 的 `OH_ContentEmbed_Document_Read` 读的是这个复合文件的**原始字节**（`StorageIO::ReadRawCd`），不是逻辑内容。

## OEID

OE 文档的系统可识别标识符，格式为 GUID（32 位无连字符或 36 位带连字符 hex，`MAX_OEID_LENGTH=40`）。物理存储在 CFB **根目录项（Root Entry，index 0）的 CLSID 字段**，不是文件头 CLSID——`Storage::Clsid()` 读的是后者，两者语义不同。序列化按 GUID 字节序规则（前 3 组小端反转）转换（`document/src/utils.cpp`）。

## OE Extension

`type=contentEmbed` 的 ExtensionAbility，框架侧类型码 `511`，运行时由 AbilityRuntime dlopen `libcontent_embed_extension_module.z.so`（导出 `OHOS_EXTENSION_GetExtensionModule`）创建。框架实现 `frameworks/kits/extension/src/object_editor_extension.cpp`（`ObjectEditorExtension : ExtensionBase + ObjectEditorExtensionStub` 双继承）。应用侧入口 `OH_AbilityRuntime_OnNativeExtensionCreate`。

## OE SA（Object Editor Service）

SA 66528，进程 `object_editor_service`，`sa_main` 加载 `libcontent_embed_service.z.so`。主体 `system_ability/src/object_editor_manager_system_ability.cpp`。**按需启动**（`bindercall`）、**不自动重启**（`auto-restart=false`），空闲自退出（见“SA 空闲退出”）。

## 客户端 OE 对象 / 服务端 OE 对象

客户端 OE 对象 = `ContentEmbed_ExtensionProxy`（NDK 聚合体，`frameworks/ndk/include/native_object_editor_types.h`）；服务端 OE 对象 = Extension 进程内的 `ContentEmbed_Object`（document + clientCb + 5 个对象级回调）。两者指向同一 OE 文档，通过三条 IPC 通道交互。官方文档的“客户端/服务端 OE 对象”指此对应。

## 快照（Snapshot）

OE 文档在客户端界面的预览图。**不走 IPC parcel**：extension 侧 `OH_ContentEmbed_Extension_SetSnapshot` 用 ImagePacker 写 PNG 到客户端沙箱的 `snapshot.png`（URI 授权路径），客户端 `GetSnapshot` 再解码为 `OH_PixelmapNative`。写入前客户端 `GetSnapshot` 返回 `CE_ERR_PARAM_INVALID`。

## 嵌入与链接（isLinking）

`OH_ContentEmbed_CreateDocumentByFile` 的双模式。嵌入（false）：编辑前拷贝临时文件到客户端沙箱，编辑不改源文件，客户端正常退出清空临时文件；链接（true）：编辑直接改源文件，源文件移动/删除/重命名后编辑失败，且**不支持链接客户端应用沙箱路径**（`CE_ERR_INVALID_LINKING_PATH`，`SA` 侧 `SystemUtils::IsAppSandboxPath` 拒绝）。

## PACKAGE_OEID 与通用包文档

`0003000C-0000-0000-C000-000000000046`（OLE Packager CLSID）。当某格式无 OE 服务端应用注册（CREATE_BY_FILE 查库为空）或 EDIT 带此 OEID 时，SA 置 `isPackageExtension=true`，由 `package/` 模块在客户端进程内本地提供“以应用图标嵌入 + 唤起默认应用编辑”的系统兜底能力，无独立 Extension 进程、无 death recipient。

## Ole10Native 流

通用包文档存放被嵌入源文件的 CFB 流（`\x01Ole10Native`，另建空 `\x01CompObj`、`\x03ObjInfo`）。布局：fileSize(4B) + marker(2B) + filename + filepath + fileLinkSize(4B) + fileLink + dataSize(4B) + 文件字节。解析/生成在 `package/src/package_data.cpp`。提取出的物理文件即 `GetNativeFilePath()` 返回值。

## diversion（格式改道）

`etc/diversion_map.json` 把已知 Windows OLE CLSID（如 Excel 工作表）映射到本地 OEID + `minVersion`。EDIT 查库无果时 SA 用它改道到替代 OE 服务端应用（`GetTargetOEid`）。**仅进程构造期读取一次**，更新配置需等 SA 退出重拉。

## 默认应用策略 / 最新注册策略

多个 OE 服务端应用注册同一后缀时的选择规则（`HandleDefaultAppFormatPolicy`）：先查该后缀的**默认打开应用**（`IDefaultApp::GetDefaultApplication`），在候选内则选它；否则取 `ORDER BY create_time DESC` 的首个，即**最新注册者**。

## 会话与连接（connectionMap_）

SA 用 `map<remoteObject, sptr<ObjectEditorConnection>>` 管理活跃连接。计数语义为**跨 bundle**：与其它 bundle 的 Extension 连接数 ≥ `MAX_CONNECTION_COUNT=2` 拒绝；同 bundle 同 ability 不同客户端 bundle 也拒绝（一个 Extension 连接只服务一个客户端 bundle）；同 bundle 同 ability 同客户端则复用 remoteObject。

## 三条 IPC 通道

客户端→SA：`IObjectEditorManager`（descriptor `OHOS.ObjectEditor.IObjectEditorManager`，9 命令）；客户端↔Extension：`IObjectEditorExtension`（`OHOS.ObjectEditor.IObjectEditorExtension`，7 命令）；SA/Extension→客户端回调：`IObjectEditorClientCallback`（`OHOS.ObjectEditor.IObjectEditorClientCallback`，OnUpdate/OnError/OnStopEdit/OnExtensionStopped）。命令枚举在 `interfaces/innerkits/include/` 对应头文件。

## 手写 stub/proxy

**三个 `.idl` 文件（`system_ability/`、`extension_ability/`、`client/`）均无任何 GN 目标引用**，stub/proxy 全部手写（`object_editor_manager_stub.cpp`、`object_editor_extension_stub.cpp`、`object_editor_client_callback_stub.cpp` 及各自 proxy）。IDL 是过时描述（Manager IDL 仅 3 方法，实际 9 命令；且其中 2 个在 SA 侧是空实现）。改 IPC 面 ≠ 改 IDL。

## Extension 空闲停用（20 秒）

`ObjectEditorConnection::TimerThreadStopExtension`：连接建立后起 20s 定时（`EXTENSION_STOP_TIME_S=20`），每轮先查锁屏状态（锁屏容忍一轮），再 IPC 查 `GetExtensionEditStatus`，非编辑中则登记 `SA_CLEAN_IDLE` 并 `DisconnectAbility`。

## SA 空闲退出（10 分钟）

`TimerThreadStopSA`：`SA_EXIT_TIME_S=600` 内无任何 IPC（每个 IPC 的 `CallbackEnter` 重置计时）且 `connectionMap_` 为空 → `UnloadSystemAbility(66528)` 自卸载。`OnIdle()` 为空实现，**不依赖 samgr idle 机制**。

## ExtensionStopReason（停止原因）

`UNKNOWN / SA_CLEAN_IDLE / PROXY_STOP / EXTENSION_TERMINATED / EXTENSION_EXCEPTION_EXIT`（`interfaces/innerkits/include/object_editor_common.h`）。SA 登记于 `extensionStopReasonMap_`，**60 秒后清理**——客户端 `QueryExtensionStopReason` 必须在停止后 60s 内查询。extension 异常死亡时 death recipient 会先查此表，`SA_CLEAN_IDLE` 则静默不打扰客户端。

## OESA_UID

`7061`，`object_editor_service` 进程的固定 UID（`frameworks/kits/extension/src/object_editor_extension.cpp` 常量）。Extension 侧 IPC 以 `GetCallingUid()==OESA_UID` 或 `callingTokenID==callerTokenId` 双通道校验调用方，防止伪造直连。

## 能力位（Capability）

`ContentEmbed_CapabilityCode`：`CE_CAPABILITY_SUPPORT_SNAPSHOT=1<<0`、`CE_CAPABILITY_SUPPORT_DO_EDIT=1<<1`。extension 在 `OnGetCapability` 回调中按位声明；`GetSnapshot` 前框架先查快照位。

## 错误码四族 + 公共错误码

内部四族（`interfaces/innerkits/include/object_editor_err_code.h`）：Client 3531xxx、SA 3532xxx、Extension 3533xxx、Document 3534xxx。对外 C 错误码（`interfaces/kits/c/content_embed_common.h`）：`CE_PERMISSION_DENIED=201`、`CE_ERR_PARAM_INVALID=401`、`CE_ERR_DEVICE_NOT_SUPPORTED=801`、业务码 35300001-35300016。映射表 `frameworks/ndk/src/object_editor_native_common.cpp`；入站校验 `ValidateErrorCode`。

## 注册库（object_editor_info 表）

SA 侧 RDB（`relational_store`），路径 `/data/service/el2/public/object_editor_service/database/<userId>/object_editor.db`，SecurityLevel S2。表字段：oeid(UNIQUE)、bundle/module/ability_name、resource_path、hap_path、version、name_id、description_id、file_exts、icon_id、create_time。name/description/icon 存的是**资源 id**，显示名由 resmgr 按 locale 还原。BMS 安装/卸载/替换公共事件驱动增删改。

## resmgr（资源还原）

`database/src/object_editor_manager_resmgr.cpp`：按 (bundle, module, locale) 缓存 `ResourceManager`，从 `content_embed_config` profile（`$profile:` 定位）解析 oeid/file_exts/name($string)/icon($media)/description，还原成 `ObjectEditorFormat`（含 `PixelMap` 图标）。

## CFB 结构对象

`document/` 引擎的核心对象：`Header`（头部 0x4C+109 DIFAT 项）、`AllocTable`（FAT/MiniFAT，`Eof/Avail/Bat/MetaBat` 特殊值，链跟踪带防环）、`DirEntry`（128B 目录项，UTF-16LE 名 ≤31 字符，红黑树兄弟排序）、`DirTree`（目录树容器）、`StorageIO`（引擎本体，文件/内存双模式）、`StreamImpl`（流读写，块链缓存）、`Storage`（门面，POLE 风格 API，recursive_mutex）。

## Flush 与 RebuildAndFlush

`StorageIO::Flush` 是事务式原地写：快照备份 → 依次落盘目录树/MiniFAT/FAT/DIFAT，**Header 最后写**；失败仅回滚内存状态。`ObjectEditorDocument::Flush` 按临时文件/userTmp 分四支派发。`RebuildAndFlush` 是碎片回收重建（触发条件：文件 >64KB 且浪费 ≥8MB 且浪费×10 ≥ 文件×3），新建内存文档逐流拷贝后整体替换目标文件。

## DLP 拦截（fail-closed）

DLP 沙箱内 OE 能力整体不可用。客户端：所有 NDK C API 入口 `CheckIsInDlp()`（进程自查，`WITH_DLP` 未编译时恒 false）；服务端：stub 的 Start/Stop/StartUIAbility 三处 `CheckCallerInDlpSandbox()`（按调用方 token）。**查询失败按“在 DLP 中”处理**（fail-closed）。编译开关 `object_editor_dlp_part_enabled`（`object_editor.gni`）控制 `WITH_DLP`，仅 `system_ability` 与 `frameworks/ndk` 两个目标定义它。

## documentId

客户端会话建立时生成（`GenRandomUuid`，基于 `getrandom`），随后作为 `PrepareFiles` 的沙箱临时目录名（`GetTempDir()/<documentId>/`）、Extension 侧 objects 表的键、`StopObjectEditorExtension` 的定位参数。会话生命周期以它为线索。
