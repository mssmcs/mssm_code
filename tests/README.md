# tests bundle

This folder is the **canonical** CMake project for **library unit tests**. It discovers **no apps** — only libraries listed in `MSSM_TEST_LIBRARIES` (plus `googletest`) when tests are enabled.

See [`docs/BUNDLES.md`](../docs/BUNDLES.md) and [`libraries/TESTING.md`](../libraries/TESTING.md).

## Configure

Tests are on by default in [`CMakeLists.txt`](CMakeLists.txt):

```powershell
cmake -S C:/github/mssm_code/tests -B C:/github/mssm_code/tests/build/agent -G Ninja
```

In Qt Creator, open **`tests/CMakeLists.txt`** as the project file.

## Build and run

```powershell
cmake --build C:/github/mssm_code/tests/build/agent --target layout_sizebound_tests
cd C:/github/mssm_code/tests/build/agent
ctest --output-on-failure
```

Build **all** to compile every enabled test target.

## Current test targets

| Target | Library | Notes |
|--------|---------|--------|
| `layout_sizebound_tests` | `layout` | `SizeBound` / `distributeSizes` (minimal link: `sizebound.cpp`) |
| `window_event_mask_tests` | `window` (headers) | `Event` mouse-move button bitmask helpers |

When more libraries add `tests/` subdirectories, append their names to `MSSM_TEST_LIBRARIES` in [`libraries/cmake/MssmAggregateRoot.cmake`](../libraries/cmake/MssmAggregateRoot.cmake).

## Alternative: tests from examples or apps

You can enable the same tests without switching projects:

```powershell
cmake -S C:/github/mssm_code/examples -B .../examples/build/... -DMSSM_BUILD_TESTS=ON
```

That also builds all example or app targets in that bundle — use the **tests** bundle when you only want fast headless runs or CI.
