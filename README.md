# mssm_code

## Build

The `apps/` folder is the aggregate CMake project that discovers and builds app subprojects.

### Linux

```bash
cd apps
cmake -S . -B build -G Ninja
cmake --build build
```

### Windows (Qt Creator / CLI)

Qt Creator can configure and build `apps/CMakeLists.txt` directly.

Equivalent CLI pattern:

```powershell
& "C:/Qt/Tools/CMake_64/bin/cmake.exe" -S apps -B apps/build/<build-dir> -G Ninja
& "C:/Qt/Tools/CMake_64/bin/cmake.exe" --build apps/build/<build-dir>
```

### Windows Helper Scripts

From repo root:

```powershell
# Configure + build via Qt MinGW/Ninja toolchain
powershell -ExecutionPolicy Bypass -File tools/build_apps.ps1

# Configure only
powershell -ExecutionPolicy Bypass -File tools/build_apps.ps1 -ConfigureOnly
```

`tools/dev_env.ps1` sets a process-local toolchain environment (CMake, Ninja, MinGW)
used by `tools/build_apps.ps1`.

Build output location with scripts:

- default: `apps/build/agent`
- customizable with: `-BuildDir`
- extra CMake options with: `-CMakeArgs @("...")`

Qt Creator normally uses kit-specific folders such as:

- `apps/build/Desktop_Qt_6_10_1_MinGW_64_bit-Debug`

So by default they do **not** step on each other.  
To keep them isolated, use different build directories per workflow and avoid sharing
the same `-BuildDir` with a Qt Creator build directory.

### Crash Diagnostics and Sanitizers

`mssm` now installs crash diagnostics in its runtime wrapper (`std::set_terminate` plus
basic fatal-signal reporting) so students get clearer messages for uncaught failures.

For deeper memory/UB diagnostics, enable sanitizers during configure:

```powershell
powershell -ExecutionPolicy Bypass -File tools/build_apps.ps1 `
  -ConfigureOnly `
  -BuildDir "apps/build/agent_san" `
  -CMakeArgs @("-DMSSM_ENABLE_SANITIZERS=ON")
```

Notes:

- Sanitizers are intended for `Debug` / `RelWithDebInfo` builds.
- Current setup targets GCC/Clang style sanitizer flags.
- On Windows with Qt MinGW, `libasan` is often unavailable; this config warns and
  skips sanitizer flags to avoid link failures.

## Packaging

Packaging is per app (not one monolithic installer by default).

From `apps/` build directory:

```bash
cmake --build build --target package_cmd_app
cmake --build build --target package_graphics_app
cmake --build build --target package_apps
```

On Windows, each package target can produce `ZIP` and `NSIS` outputs.

## Notes

- `apps/` subfolders (student/app repos) are ignored by `mssm_code` Git config.
- Shared packaging assets live under `shared/packaging/`.

## Unit Tests With Googletest

`googletest` is available as a shared library entry under `libraries/`.

1. Add it to your app `CMakeLists.txt` library list:

```cmake
set(LIBRARIES mssm googletest)
```

2. In your app CMake, add a test target and link it:

```cmake
enable_testing()

add_executable(my_app_tests
    tests/main.cpp
    tests/sample_test.cpp
)

target_link_libraries(my_app_tests PRIVATE
    googletest_main
    mssm_utils
)

include(GoogleTest)
gtest_discover_tests(my_app_tests)
```

`googletest` links `GTest::gtest`, and `googletest_main` links `GTest::gtest_main`.
Use whichever is appropriate for your test executable.

## New App Scaffolding

Use scaffold scripts to create a new app folder from a template without inheriting
the template's git history.

If you omit `Template`/`--template` and destination (`-Destination` / `--dest`),
the scripts prompt with numbered lists:
- template IDs from `templates/templates.csv`
- eligible top-level destination folders under `mssm_code` excluding:
  `assets`, `libraries`, `packaging`, `shared`, `templates`, `tools`

### Windows

```powershell
.\tools\new_app.ps1 -Template cmd_app -Name my_new_app
.\tools\new_app.ps1 -Template graphics_app -Name my_graphics_app
```

### macOS/Linux

```bash
bash tools/new_app.sh --template cmd_app --name my_new_app
bash tools/new_app.sh --template graphics_app --name my_graphics_app
```

Template can be:
- template id from `templates/templates.csv`
- local path
- git repo URL (optionally with `TemplateSubdir` / `--template-subdir`)

Optional library override while scaffolding:

- Windows: `-Libraries "mssm_graphics_nanovg"`
- macOS/Linux: `--libraries "mssm_graphics_nanovg"`
- Example with OpenCV: `-Libraries "mssm opencv"` / `--libraries "mssm opencv"`

OpenCV library behavior:

- add `opencv` to your app `set(LIBRARIES ...)`
- CMake first tries `find_package(OpenCV ...)` (system/vcpkg install)
- if not found, it falls back to CPM and builds a minimal OpenCV module set
- CPM fallback builds OpenCV as static libraries (`BUILD_SHARED_LIBS=OFF`) so
  Qt Creator/CLI runs do not depend on OpenCV DLL path setup
- optional strict Windows behavior (disable source-build fallback):
  - `-DMSSM_OPENCV_ALLOW_CPM_ON_WINDOWS=OFF`
- reference example app: `apps/opencv_demo`

OpenCV bridge library:

- add `opencv_bridge` to your app `set(LIBRARIES ...)` for conversion helpers
- key APIs in `libraries/opencv_bridge/opencv_bridge.h`:
  - `toMatBGR(const mssm::Image&)`
  - `toMatRGBA(const mssm::Image&)`
  - `fromMat(const cv::Mat&, mssm::Image&)`
  - `applyMatToImage(const cv::Mat&, mssm::Image&)`
  - `rgbBufferToMat(const uint8_t* rgb, int width, int height)` (OpenPnP-style RGB buffer)
- `apps/opencv_demo` shows a full roundtrip:
  - synthetic RGB buffer -> OpenCV processing -> `mssm::Image` update -> output PNG

Library discovery is automatic from `libraries/*/CMakeLists.txt` (no library CSV).
You can list discovered libraries with:

- Windows: `.\tools\new_app.ps1 -ListLibraries`
- macOS/Linux: `bash tools/new_app.sh --list-libraries`

Optional per-library metadata:
- add a comment line near the top of a library `CMakeLists.txt`:
  - `# MSSM_LIB_DESC: Short description shown in library listings`

Template catalog helpers:

- Windows: `.\tools\new_app.ps1 -ListTemplates`
- macOS/Linux: `bash tools/new_app.sh --list-templates`
