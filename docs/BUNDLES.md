# Project bundles

A **bundle** is a top-level folder under `mssm_code` that you open as **one CMake project** in Qt Creator or the command line. Bundles are not a different build system — they share the same library discovery, dependency resolution, and optional packaging logic.

## Bundles today

| Bundle | Path | What it builds |
|--------|------|----------------|
| **apps** | [`apps/`](../apps/) | Student and course applications |
| **examples** | [`examples/`](../examples/) | Teaching and library demos (`layout_example`, …) |
| **tests** | [`tests/`](../tests/) | Library unit tests only (no apps) |

Each bundle has its **own** build directory (for example `examples/build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug`). Bundles do not share build trees.

## Git: workspace layout vs `mssm_code` repo

The **`mssm_code` repo** ([`.gitignore`](../.gitignore)) uses opt-in at the root: only whitelisted top-level trees are tracked (`libraries/`, `assets/`, `docs/`, `tests/`, …). **Any other folder** in the workspace — including cloned bundle repos — is ignored automatically. No bundle names appear in the root `.gitignore`.

| Folder | Git home | Role |
|--------|----------|------|
| `libraries/`, `assets/`, `shared/`, … | **`mssm_code`** | Core libraries and shared assets |
| [`tests/`](../tests/) | **`mssm_code`** | In-repo CMake/`ctest` harness; test sources live under `libraries/<lib>/tests/` |
| [`apps/`](../apps/) | **Separate repo** | Clone beside `mssm_code`; not tracked by `mssm_code` |
| [`examples/`](../examples/) | **[`mssmcs/mssm_examples`](https://github.com/mssmcs/mssm_examples)** | Clone beside `mssm_code` |
| [`grading/`](../grading/) | **[`mssmcs/grading`](https://github.com/mssmcs/grading)** | Clone beside `mssm_code` |

Each cloned bundle uses its own `.gitignore` (for example [`apps/.gitignore`](../apps/.gitignore)) to opt in only shared scaffolding and ignore student projects and `build/` output.

## Thin `CMakeLists.txt`

Every bundle starts with `cmake_minimum_required`, **`project()`** (so Qt Creator shows `apps` / `examples` / `tests`, not the default name `Project`), optional bundle-local settings, then includes the shared aggregate:

```cmake
cmake_minimum_required(VERSION 3.22)

get_filename_component(MSSM_BUNDLE_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)
string(REPLACE " " "_" MSSM_BUNDLE_NAME "${MSSM_BUNDLE_NAME}")
project(${MSSM_BUNDLE_NAME} LANGUAGES CXX C)

# Optional: skip subfolders that are not part of this bundle
set(MSSM_AGGREGATE_EXCLUDE_SUBDIRS
    opencv_demo
)

include("${CMAKE_CURRENT_SOURCE_DIR}/../libraries/cmake/MssmAggregateRoot.cmake")
```

Shared logic lives in [`libraries/cmake/MssmAggregateRoot.cmake`](../libraries/cmake/MssmAggregateRoot.cmake):

- Bundle `project()` name comes from the folder name in each bundle’s `CMakeLists.txt` (not from the shared include)
- Scan child folders for apps (flat or `course/student/` layout)
- OS / Qt filters, `ALL_LIBRARIES` union, `resolve_library_dependencies`
- Library and app `add_subdirectory` loops
- `MSSM_BUILD_TESTS`, `MSSM_TEST_LIBRARIES`, CPack

Bundle-specific overrides go **before** the include (for example `FETCHCONTENT_QUIET OFF` on `apps`, or `MSSM_BUILD_TESTS ON` on `tests`).

## Apps vs libraries vs tests

- **Libraries** live under `libraries/<name>/` and are pulled in by app `LIBRARIES` lists or by `MSSM_TEST_LIBRARIES` when tests are enabled.
- **Apps** are subfolders under a bundle with their own `CMakeLists.txt`.
- **Unit tests** live under `libraries/<lib>/tests/` and are built when `MSSM_BUILD_TESTS=ON`. See [`libraries/TESTING.md`](../libraries/TESTING.md).

## Qt Creator workflow

1. **File → Open File or Project** → choose `apps/CMakeLists.txt`, `examples/CMakeLists.txt`, or `tests/CMakeLists.txt`.
2. Pick a kit; configure once per build directory.
3. **Build → Build All** builds every app in that bundle plus required libraries.
4. To run library unit tests, open the **`tests`** bundle or add `-DMSSM_BUILD_TESTS=ON` to an existing `examples` or `apps` build (see [`tests/README.md`](../tests/README.md)).

## Adding a new bundle

1. Create a **separate git repo** (or use an existing org repo) and clone it into `mssm_code/<bundle>/` beside the core tree.
2. Add `<bundle>/CMakeLists.txt` using the thin pattern above.
3. Add `<bundle>/.gitignore` using the whitelist pattern in [`apps/.gitignore`](../apps/.gitignore): ignore `/*`, then un-ignore aggregator files and tracked subfolders.
4. Add app or demo subfolders as needed (each with `CMakeLists.txt`).
5. Set `MSSM_AGGREGATE_EXCLUDE_SUBDIRS` if some sibling folders should not be discovered.
6. Open `<bundle>/` in Qt Creator.

No edit to the root [`.gitignore`](../.gitignore) is required — cloned bundle folders are ignored by `/*` automatically.

## Related docs

- [`libraries/TESTING.md`](../libraries/TESTING.md) — unit test tiers, `MSSM_BUILD_TESTS`, `ctest`
- [`apps/README.md`](../apps/README.md) — apps bundle specifics (packaging, scaffolding)
- [`examples/README.md`](../examples/README.md) — examples bundle
- [`tests/README.md`](../tests/README.md) — tests bundle
