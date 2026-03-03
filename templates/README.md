# Templates

Place reusable app templates here and register them in `templates/templates.csv`.

Each template entry has:

- `id`: short name used by scaffold scripts
- `source`: `local` or `git`
- `location`: path (for local) or repo URL (for git)
- `branch`: optional git branch/tag
- `subdir`: optional subfolder inside cloned repo
- `description`: short text shown in template listings

Use:

- Windows: `tools/new_app.ps1 -ListTemplates`
- macOS/Linux: `bash tools/new_app.sh --list-templates`
