# apps

This folder is an aggregate CMake project.  
It discovers app subfolders and builds them together so they can share libraries from `../libraries`.

## What Gets Tracked Here

In the `mssm_code` repository, app subfolders are ignored by default (`apps/.gitignore`).  
Only the aggregator files (like `apps/CMakeLists.txt`) are tracked.

## Build

From this folder:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

In Qt Creator, open `apps/CMakeLists.txt` and build normally.

## Packaging (Per App)

Packaging targets are generated per discovered app.  
Examples:

```bash
cmake --build build --target package_cmd_app
cmake --build build --target package_graphics_app
cmake --build build --target package_apps
```

- `package_<app>` builds installers/packages for one app.
- `package_apps` builds all per-app packages.

## Where Output Goes

Packages are written under:

- `apps/build/<build-dir>/distribution/<app>/`

On Windows, per-app packages include `ZIP` and `NSIS` outputs.

## Creating New Apps From Templates

You can scaffold a new app folder from templates like `cmd_app` or `graphics_app`
without copying git history from the template.
When template or destination is omitted, the scripts provide interactive numbered selections.

### Script Entry Points

- Windows: `tools/new_app.ps1`
- macOS/Linux: `tools/new_app.sh`
- Convenience wrappers in this folder:
  - Windows: `apps/new_app.ps1`
  - macOS/Linux: `apps/new_app.sh`

Examples from repo root:

```powershell
.\tools\new_app.ps1 -Template cmd_app -Name assignment01 -Libraries "mssm"
```

```bash
bash tools/new_app.sh --template graphics_app --name assignment01 --libraries "mssm_graphics_nanovg"
```

Examples from `apps/` folder:

```powershell
.\new_app.ps1
```

```bash
bash ./new_app.sh
```

After creating an app folder, run CMake again so the new project is discovered.
