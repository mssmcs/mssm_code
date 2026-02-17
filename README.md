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
