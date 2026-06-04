# Unit testing in mssm_code

GoogleTest is the standard framework. Tests are **opt-in** at configure time so student builds stay fast by default.

## Test tiers

| Tier | What | Typical deps | Examples |
|------|------|--------------|----------|
| **T1 — Pure unit** | Functions with no I/O | None or a single `.cpp` from the library | `SizeBound`, `distributeSizes` |
| **T2 — Library unit** | One library, small fakes | Static lib or header-only helpers | `Event` mouse bitmask helpers |
| **T3 — App integration** | One executable’s wiring | App `LIBRARIES` + gtest | Parsers, loaders on sample files |
| **T4 — Interactive** | Manual or future golden | GLFW, NanoVG, Qt | `layout_example` (not in `ctest` yet) |

Default new work: **T1 and T2**. Keep teaching examples as demos, not the primary test host.

## Where tests live

| Location | Use for |
|----------|---------|
| `libraries/<lib>/tests/*.cpp` | Reusable library behavior (preferred) |
| `<app>/tests/*.cpp` | Behavior specific to one assignment |

## CMake switches

Defined in [`cmake/MssmAggregateRoot.cmake`](cmake/MssmAggregateRoot.cmake):

- **`MSSM_BUILD_TESTS`** (default `OFF`) — enables `enable_testing()`, includes [`MssmTest.cmake`](cmake/MssmTest.cmake), appends `googletest` and **`MSSM_TEST_LIBRARIES`** to the library build list.
- **`MSSM_TEST_LIBRARIES`** (default `layout`) — libraries that expose a `tests/` subdirectory.

Each library enables its tests at the bottom of its `CMakeLists.txt`:

```cmake
if(MSSM_BUILD_TESTS)
    add_subdirectory(tests)
endif()
```

Test targets use **`mssm_add_gtest()`** from `MssmTest.cmake` (links `googletest_main`, runs `gtest_discover_tests`).

## Two ways to build tests

| Approach | When to use |
|----------|-------------|
| Open **`tests/`** bundle | CI, running all library tests, minimal configure (tests on by default) |
| **`examples`** or **`apps`** with `-DMSSM_BUILD_TESTS=ON` | App development with occasional unit test runs |

Test sources are shared; each build tree produces its own `*_tests` executables under that tree’s output directory.

## Configure and build

**Default on this machine (Qt Creator + MinGW, `tests` project):**

```powershell
& "C:\Qt\Tools\CMake_64\bin\cmake.exe" --build C:/github/mssm_code/tests/build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug --target all
```

Open the **`tests/`** bundle in Qt Creator first so that build directory exists. Kit folder names vary; look under `tests/build/` for `Desktop_Qt_*`. Agents and scripts should use this same `--build …/Desktop_Qt_…-Debug --target all` pattern — not a fresh `cmake -G Ninja` configure from a bare shell.

**Alternate (headless CI / no Qt kit):**

```powershell
cmake -S C:/github/mssm_code/tests -B C:/github/mssm_code/tests/build/agent -G Ninja
cmake --build C:/github/mssm_code/tests/build/agent --target all
```

Requires `g++` on `PATH` (or an explicit `CMAKE_CXX_COMPILER`).

**Examples with tests:**

```powershell
& "C:\Qt\Tools\CMake_64\bin\cmake.exe" --build C:/github/mssm_code/examples/build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug --target all
```

Or configure from scratch with `-DMSSM_BUILD_TESTS=ON` if the kit tree is not present yet.

In Qt Creator: open the **`tests`** project → **Build** → **Build All** (same as `--target all` above).

## Running from Qt Creator or an agent shell (Windows MinGW)

**Build** (always use `--target all` on the Qt kit tree):

```powershell
& "C:\Qt\Tools\CMake_64\bin\cmake.exe" --build C:/github/mssm_code/tests/build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug --target all
```

**Run all discovered tests:**

```powershell
& "C:\Qt\Tools\CMake_64\bin\cmake.exe" -E chdir C:/github/mssm_code/tests/build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug ctest --output-on-failure
```

**Run one executable directly** (9 tests in `layout_sizebound_tests` only — not the full suite):

```powershell
& "C:/github/mssm_code/tests/build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug/local_library/layout/tests/layout_sizebound_tests.exe"
```

There are **two** gtest executables in the layout library (`layout_sizebound_tests`, `window_event_mask_tests`). Running one exe reports 9 tests; **`ctest` runs all 12** discovered cases.

### Qt Creator: run all tests with `ctest`

The default Run configuration for a single `*_tests` target only runs that executable. To run the full suite from Qt Creator:

**Projects → Run → Add → Custom Executable** (name it e.g. “All tests (ctest)”):

| Field | Value |
|--------|--------|
| **Executable** | `%{CMake:Executable:Path}/ctest` |
| **Working directory** | `%{buildDir}` |
| **Command line arguments** | `--output-on-failure` |
| **Run in terminal** | **checked** |

Use the **(Variables)** button next to each field to pick `%{CMake:Executable:Path}` and `%{buildDir}` from the list. Kit folder names vary; those macros stay correct when the build tree moves.

**Run in terminal** is required for reliable `ctest` output (progress and pass/fail lines). Without it, Application Output may truncate or hide results.

Per-exe macros (for the **active** run config only): `%{RunConfig:Executable:FilePath}`, `%{RunConfig:Executable:Path}`. Build paths to other test exes from `%{buildDir}`, e.g. `%{buildDir}/local_library/layout/tests/window_event_mask_tests.exe`.

**MinGW runtime DLLs:** On Windows MinGW builds, [`cmake/MssmWindowsRuntime.cmake`](cmake/MssmWindowsRuntime.cmake) copies `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, and `libwinpthread-1.dll` next to each app and gtest executable at link time. You do **not** need `C:/Qt/Tools/mingw1310_64/bin` on `PATH` to run tests or examples from their output folder.

If you see `libgcc_s_seh-1.dll was not found`, rebuild the target so POST_BUILD copy runs, or check that the exe and DLLs sit in the same directory.

The same pattern applies to example apps (e.g. `layout_example`) under `examples/build/Desktop_Qt_.../`.

## Run tests

**`ctest` (after `--target all` build):**

```powershell
& "C:\Qt\Tools\CMake_64\bin\cmake.exe" -E chdir C:/github/mssm_code/tests/build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug ctest --output-on-failure
ctest -R layout_sizebound
```

Exact exe paths depend on kit; use the link line from the build log if the kit folder name differs.

## Reference: layout tests

- Sources: [`layout/tests/`](layout/tests/)
- Targets: `layout_sizebound_tests` (compiles `sizebound.cpp` only), `window_event_mask_tests` (header helpers in `windowevents.h`)
- See [`layout/ARCHITECTURE.md`](layout/ARCHITECTURE.md) for layout-specific notes.

## Adding tests to another library

1. Add `libraries/<lib>/tests/` with `CMakeLists.txt` calling `mssm_add_gtest`.
2. Add `if(MSSM_BUILD_TESTS) add_subdirectory(tests)` to `libraries/<lib>/CMakeLists.txt`.
3. Append `<lib>` to `MSSM_TEST_LIBRARIES` in `MssmAggregateRoot.cmake` (or override the cache variable when configuring).

## Related docs

- [`docs/BUNDLES.md`](../docs/BUNDLES.md) — bundle model
- [`tests/README.md`](../tests/README.md) — tests bundle entry point
- Root [`README.md`](../README.md) — short overview and links
