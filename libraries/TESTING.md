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

**Tests bundle (recommended for `ctest`):**

```powershell
cmake -S C:/github/mssm_code/tests -B C:/github/mssm_code/tests/build/agent -G Ninja
cmake --build C:/github/mssm_code/tests/build/agent --target layout_sizebound_tests
```

On Windows with **Qt Creator + MinGW**, prefer the kit build tree Qt already configured (example kit folder name):

```text
C:/github/mssm_code/tests/build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug
```

Do not rely on a fresh `cmake -G Ninja` configure from a bare shell unless `g++` is on `PATH` — agents and CI shells often lack the compiler until a kit is selected.

**Examples with tests:**

```powershell
cmake -S C:/github/mssm_code/examples -B C:/github/mssm_code/examples/build/agent -DMSSM_BUILD_TESTS=ON -G Ninja
cmake --build C:/github/mssm_code/examples/build/agent --target layout_sizebound_tests
```

In Qt Creator: **Projects → Build → CMake** → add `-DMSSM_BUILD_TESTS=ON`, reconfigure, then build target `layout_sizebound_tests` or **all**.

## Running from Qt Creator or an agent shell (Windows MinGW)

Use the **existing Qt Creator build directory** for the `tests/` or `examples/` bundle. Kit folder names vary; look under `tests/build/` or `examples/build/` for `Desktop_Qt_*`.

Build and run all discovered tests:

```powershell
$build = "C:/github/mssm_code/tests/build/Desktop_Qt_6_11_0_MinGW_64_bit-Debug"

& "C:/Qt/Tools/CMake_64/bin/cmake.exe" --build $build --target all
& "C:/Qt/Tools/CMake_64/bin/cmake.exe" -E chdir $build ctest --output-on-failure
```

Run one executable directly:

```powershell
& "$build/local_library/layout/tests/layout_sizebound_tests.exe"
```

**MinGW runtime DLLs:** On Windows MinGW builds, [`cmake/MssmWindowsRuntime.cmake`](cmake/MssmWindowsRuntime.cmake) copies `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, and `libwinpthread-1.dll` next to each app and gtest executable at link time. You do **not** need `C:/Qt/Tools/mingw1310_64/bin` on `PATH` to run tests or examples from their output folder.

If you see `libgcc_s_seh-1.dll was not found`, rebuild the target so POST_BUILD copy runs, or check that the exe and DLLs sit in the same directory.

The same pattern applies to example apps (e.g. `layout_example`) under `examples/build/Desktop_Qt_.../`.

## Run tests

**Run one executable** (Qt Run configuration or shell):

```powershell
& "C:/github/mssm_code/tests/build/agent/local_library/layout/tests/layout_sizebound_tests.exe"
```

Exact path depends on kit and generator; use the link line from the build log.

**`ctest` (all discovered tests in the build tree):**

```powershell
cd C:/github/mssm_code/tests/build/agent
ctest --output-on-failure
ctest -R layout_sizebound
```

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
