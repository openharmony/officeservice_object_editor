# AGENTS.md

Compact guidance for OpenCode sessions working in `officeservice_object_editor`.
This repo is the OpenHarmony **object_editor** component (subsystem `officeservice`,
SysCap `SystemCapability.ContentEmbed.ObjectEditor`) — inter-app document embedding
and co-editing ("OE"). See `README.md` for the full architectural overview.

## Build

The `./build.sh` referenced in `README.md` is the **OpenHarmony top-level build script,
not a file in this repo**. It does not exist here. GN/`build.sh` commands must be run
from the OpenHarmony source-tree root, with this repo checked out at
`foundation/officeservice/object_editor` (the canonical path; see `object_editor.gni`).

```bash
# From OpenHarmony source-tree root, not from this repo:
./build.sh --product-name {product_name} --ccache --build-target object_editor
# 64-bit ARM:
./build.sh --product-name {product_name} --ccache --target-cpu arm64 --build-target object_editor
```

GN labels are rooted at `//foundation/officeservice/object_editor/...`. When editing
`BUILD.gn` files, always reference sibling modules via that prefix (the
`object_editor_path` variable in `object_editor.gni` expands to it).

The component is built by default; opting out requires editing
`productdefine/common/inherit/rich.json` (outside this repo).

## Repo layout and key GN targets

| Path | GN target (`part:object_editor`) | Produces |
|---|---|---|
| `system_ability/` | `:content_embed_service` | `libcontent_embed_service.z.so` (the SA) |
| `frameworks/ndk/` | `:content_embed_ndk` | public NDK `.so`, installs to `ndk/` |
| `frameworks/kits/extension/` | `:content_embed_extension`, `:content_embed_extension_module` | OE Extension framework |
| `client/` | `:content_embed_client` | client-side lib |
| `document/` | `:content_embed_document` | OE document (composite file) impl |
| `utils/` | `:content_embed_utils` | shared utilities |
| `sa_profile/` | `:object_editor_service_sa_profile` | SA profile (`66528.json`) |
| `etc/` | `:object_editor_service_etc`, `:object_editor_service_diversion` | init cfg + `diversion_map.json` |
| `test/` | `:test_target` → `unittest:unittest` + `fuzztest:fuzztest` | tests |

`bundle.json` is the source of truth for the component's deps list (25 OH components)
and the `fwk_group`/`service_group`/`test` groupings.

## Service identity

- **SA ID**: `66528` (see `sa_profile/66528.json`)
- **Process / binary**: `object_editor_service`, launched via `sa_main` against
  `/system/profile/object_editor_service.json`
- **Shared lib**: `libcontent_embed_service.z.so`, `run-on-create=false`,
  `auto-restart=false`, **started on-demand by `bindercall`**
- **Idle exit**: service self-exits after ~10 min with zero active sessions and
  zero active OE Extensions (see README lifecycle section) — relevant when
  debugging service behavior
- **UID/GID/SELinux**: `object_editor_service` / `u:r:ohos_object_editor_service:s0`
- **APL**: `system_basic`; permissions include `GET_BUNDLE_INFO_PRIVILEGED`,
  `GRANT_URI_PERMISSION_PRIVILEGED`, `GET_DEFAULT_APPLICATION`,
  `START_ABILITIES_FROM_BACKGROUND`, `MANAGE_LOCAL_ACCOUNTS`
- **Data dir**: `/data/service/el2/public/object_editor_service` (created by init job)
- **IPC interface**: `system_ability/IObjectEditorManager.idl` with
  `option_stub_hooks on;` — changing the IPC surface means editing this IDL so the
  stub gets regenerated. Current ops: `startObjectEditorExtension`,
  `getOEidByFileExtension`, `getIconByOEid`.

## Testing

- Framework: **gtest/gmock** via `ohos_unittest`. `module_out_path = "object_editor/object_editor"`.
- Unit test groups (one per module) under `test/unittest/<module>/`: `client`,
  `common`, `database`, `document`, `extension_ability`, `frameworks` (ndk + extension),
  `package`, `system_ability`, `utils`. Each `BUILD.gn` defines individual
  `ohos_unittest("<name>")` targets — build a single suite by targeting that name.
- Fuzz tests under `test/fuzztest/frameworks/ndk/proxy/`, one fuzzer per NDK entry
  point, named lowercased after the C API (e.g. `ohcontentembedproxyregisteronupdatefunc_fuzzer`).
- **Mocks** live in `test/mock/` (notably `message_parcel/` and
  `ability_manager_client/`). Reuse these rather than hand-rolling substitutes.
- **`private=public` convention**: most `system_ability` tests set
  `defines = [ "private=public" ]` (and the shared `testcase_flags` config adds
  `-Dprivate=public -Dprotected=public`) to reach private members. This is
  intentional, not a smell to "fix."
- All test configs pull in `resources/config/build:coverage_flags` and
  `:testcase_flags`. Coverage is opt-in via `object_editor_feature_coverage`
  (default false).

## Code style constraints

These are mandatory for all C/C++ source changes (production code and tests
alike). Non-compliance must be fixed before declaring work done.

- **C++ standard**: C++17 (uses `std::optional`, `[[nodiscard]]`, `std::make_unique`); **Column limit**: 120; **Indent**: 4 spaces, no tabs
- **Naming**: Classes `PascalCase`, methods `PascalCase`, member vars `camelCase_`, constants `UPPER_SNAKE_CASE`, files `snake_case`
- **Include order**: corresponding header → stdlib → OH framework → `interfaces/` → other internal
- **Line width ≤ 120 characters.** Wrap long lines — OpenHarmony `ColumnLimit: 120` convention. (`-Wall -Werror` in `testcase_flags` is a compiler-diagnostic baseline, not a line-width check.)
- **Functions ≤ 50 lines** of code (excluding blank lines and comments). Split long functions into named helpers — it also keeps cyclomatic complexity down.
- **Average cyclomatic complexity ≤ 20** across the module. No in-repo tool measures this — track it in review and refactor hotspots with early returns, extracted helpers, or table-driven logic.
- **Nesting depth ≤ 5** (no more than four levels of nested blocks inside a function). Flatten with guard clauses, early returns, or extracted helpers.
- **No magic numbers.** Define named `constexpr`/`enum` constants (e.g. in `utils/` or a header near the use site) instead of inline literals. Existing code already follows this (`OBJECT_EDITOR_SERVICE_SA_ID`, `LOAD_TIMEOUT_MS`, `RETRY_TIMES`, `CONNECT_TIMEOUT`) — reuse the defined constants rather than re-introducing literals.
- use the unsafe function: memcpy
- macro argument 'method' should be enclosed in parentheses

## Conventions and gotchas

- **DLP is conditional**. `object_editor.gni` declares
  `object_editor_dlp_part_enabled` (true unless the
  `security_dlp_permission_service` part is absent from `global_parts_info`).
  When enabled, code compiles with `WITH_DLP` and links
  `dlp_permission_service:libdlp_permission_sdk`. Guard any DLP-specific code
  behind `#ifdef WITH_DLP`.
- **Hardening is on by default** for production libs. New `ohos_shared_library`
  targets in service/NDK should keep `branch_protector_ret = "pac_ret"` and the
  `sanitize { cfi = true; cfi_cross_dso = true; debug = false }` block. The
  service lib also sets `use_exceptions = true`.
- **Naming**: source files are `object_editor_*.cpp/.h` (snake_case with the
  `object_editor_` prefix) for cross-module files; class names are PascalCase.
  Follow the existing pattern when adding files.
- **README is the user-facing spec** (Chinese-primary). The four core flows —
  service lifecycle, OE Extension registration, document embedding, document
  editing — are described there with the exact `OH_ContentEmbed_*` API
  sequence. Treat it as authoritative for client/server integration behavior.
- **No `opencode.json`** in this repo; no project-specific OpenCode config to
  inherit.

## Git Commit Rules

- **User approval required**: Ask user before `git commit`. Use `git commit -s` after approval.
- **Angular format**: `type(scope): subject` (feat, fix, docs, style, refactor, test, chore)
- **Co-authored footer**: Append `Co-Authored-By: Agent` to every commit message.