#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
tool_script="$script_dir/../tools/new_app.sh"

if [[ ! -f "$tool_script" ]]; then
  echo "Cannot find tools/new_app.sh at: $tool_script" >&2
  exit 1
fi

exec bash "$tool_script" "$@"
