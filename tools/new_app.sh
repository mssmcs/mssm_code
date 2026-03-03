#!/usr/bin/env bash
set -euo pipefail

template=""
name=""
dest=""
template_subdir=""
libraries=""
list_templates=0
list_libraries=0
git_init=1
force=0
template_set=0
dest_set=0

usage() {
  cat <<'EOF'
Usage:
  tools/new_app.sh [options]

Options:
  --template <name|path|repo-url>   Template source (default: cmd_app)
  --name <folder-name>              New project folder name
  --dest <path>                     Destination root (default: apps)
  --template-subdir <path>          Subfolder inside cloned template repo
  --libraries <list>                Override set(LIBRARIES ...) in new CMakeLists.txt
  --list-templates                  List templates from templates/templates.csv
  --list-libraries                  List libraries discovered under libraries/
  --no-git-init                     Skip git init in destination
  --force                           Replace destination if it exists
  -h, --help                        Show help
EOF
}

prompt_select() {
  local prompt="$1"
  shift
  local -a options=("$@")
  if [[ ${#options[@]} -eq 0 ]]; then
    echo "No options available for selection." >&2
    exit 1
  fi

  echo "$prompt"
  local i=1
  for opt in "${options[@]}"; do
    echo "  $i) $opt"
    ((i++))
  done

  while true; do
    read -r -p "Enter number: " idx
    if [[ "$idx" =~ ^[0-9]+$ ]] && (( idx >= 1 && idx <= ${#options[@]} )); then
      echo "${options[$((idx-1))]}"
      return 0
    fi
    echo "Invalid selection. Enter a number from the list." >&2
  done
}

is_git_template() {
  local value="$1"
  [[ "$value" =~ ^(https?|ssh|git|file):// ]] || [[ "$value" =~ ^git@ ]] || [[ "$value" =~ \.git$ ]]
}

resolve_template_path() {
  local repo_root="$1"
  local value="$2"

  if [[ -d "$value" ]]; then
    (cd "$value" && pwd)
    return
  fi

  local candidate
  for candidate in \
    "$repo_root/templates/$value" \
    "$repo_root/apps/$value" \
    "$repo_root/$value"; do
    if [[ -d "$candidate" ]]; then
      (cd "$candidate" && pwd)
      return
    fi
  done

  echo "Template '$value' not found. Try a path, git URL, or --list-templates." >&2
  exit 1
}

read_catalog_entry() {
  local repo_root="$1"
  local template_id="$2"
  local catalog="$repo_root/templates/templates.csv"
  if [[ ! -f "$catalog" ]]; then
    return 1
  fi

  local line id source location branch subdir description
  while IFS=, read -r id source location branch subdir description; do
    [[ "$id" == "id" ]] && continue
    [[ -z "$id" ]] && continue
    if [[ "$id" == "$template_id" ]]; then
      printf '%s\n' "$source|$location|$branch|$subdir|$description"
      return 0
    fi
  done < "$catalog"

  return 1
}

list_catalog_templates() {
  local repo_root="$1"
  local catalog="$repo_root/templates/templates.csv"
  if [[ ! -f "$catalog" ]]; then
    echo "No template catalog found at templates/templates.csv"
    return 0
  fi

  echo "Available templates:"
  local id source location branch subdir description
  while IFS=, read -r id source location branch subdir description; do
    [[ "$id" == "id" ]] && continue
    [[ -z "$id" ]] && continue
    echo "  $id [$source] - $description"
  done < "$catalog"
}

choose_template_from_catalog() {
  local repo_root="$1"
  local catalog="$repo_root/templates/templates.csv"
  if [[ ! -f "$catalog" ]]; then
    echo "No template catalog found at templates/templates.csv" >&2
    exit 1
  fi

  local -a options=()
  local -a ids=()
  local id source location branch subdir description
  while IFS=, read -r id source location branch subdir description; do
    [[ "$id" == "id" ]] && continue
    [[ -z "$id" ]] && continue
    ids+=("$id")
    options+=("$id [$source] - ${description:-no description}")
  done < "$catalog"

  local selected_line
  selected_line="$(prompt_select "Select a template:" "${options[@]}")"
  local idx=0
  for opt in "${options[@]}"; do
    if [[ "$opt" == "$selected_line" ]]; then
      echo "${ids[$idx]}"
      return 0
    fi
    ((idx++))
  done
  echo "${ids[0]}"
}

choose_destination_folder() {
  local repo_root="$1"
  local -a excluded=("assets" "libraries" "packaging" "shared" "templates" "tools" ".git")
  local -a eligible=()
  local dir base skip

  while IFS= read -r dir; do
    base="$(basename "$dir")"
    [[ "$base" == .* ]] && continue
    skip=0
    for ex in "${excluded[@]}"; do
      if [[ "$base" == "$ex" ]]; then
        skip=1
        break
      fi
    done
    [[ "$skip" -eq 1 ]] && continue
    eligible+=("$base")
  done < <(find "$repo_root" -mindepth 1 -maxdepth 1 -type d | sort)

  local selected
  selected="$(prompt_select "Select destination folder:" "${eligible[@]}")"
  echo "$repo_root/$selected"
}

list_available_libraries() {
  local repo_root="$1"
  local libs_root="$repo_root/libraries"
  if [[ ! -d "$libs_root" ]]; then
    echo "No libraries directory found at $libs_root"
    return 0
  fi

  local found=0
  echo "Available libraries:"
  while IFS= read -r cmake_file; do
    found=1
    local lib_dir
    lib_dir="$(dirname "$cmake_file")"
    local lib_name
    lib_name="$(basename "$lib_dir")"
    local desc
    desc="$(head -n 80 "$cmake_file" | sed -n 's/.*MSSM_LIB_DESC[[:space:]]*:[[:space:]]*//p' | head -n 1 | xargs || true)"
    if [[ -n "$desc" ]]; then
      echo "  $lib_name - $desc"
    else
      echo "  $lib_name"
    fi
  done < <(find "$libs_root" -mindepth 2 -maxdepth 2 -type f -name CMakeLists.txt | sort)

  if [[ "$found" -eq 0 ]]; then
    echo "  (none)"
  fi
}

library_exists() {
  local repo_root="$1"
  local lib_name="$2"
  [[ -f "$repo_root/libraries/$lib_name/CMakeLists.txt" ]]
}

set_project_libraries() {
  local target_root="$1"
  local raw="$2"
  [[ -z "$raw" ]] && return 0

  local cmake_file="$target_root/CMakeLists.txt"
  [[ -f "$cmake_file" ]] || return 0

  local normalized
  normalized="$(echo "$raw" | tr ',;' '  ' | xargs)"
  [[ -z "$normalized" ]] && return 0

  local replacement="set(LIBRARIES $normalized)"
  local tmp_file
  tmp_file="$(mktemp)"
  local replaced=0
  while IFS= read -r line; do
    if [[ "$replaced" -eq 0 && "$line" =~ ^[[:space:]]*set[[:space:]]*\([[:space:]]*LIBRARIES.*\)[[:space:]]*$ ]]; then
      echo "$replacement" >> "$tmp_file"
      replaced=1
    else
      echo "$line" >> "$tmp_file"
    fi
  done < "$cmake_file"

  if [[ "$replaced" -eq 0 ]]; then
    echo >> "$tmp_file"
    echo "$replacement" >> "$tmp_file"
  fi
  mv "$tmp_file" "$cmake_file"
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --template) template="$2"; template_set=1; shift 2 ;;
    --name) name="$2"; shift 2 ;;
    --dest) dest="$2"; dest_set=1; shift 2 ;;
    --template-subdir) template_subdir="$2"; shift 2 ;;
    --libraries) libraries="$2"; shift 2 ;;
    --list-templates) list_templates=1; shift ;;
    --list-libraries) list_libraries=1; shift ;;
    --no-git-init) git_init=0; shift ;;
    --force) force=1; shift ;;
    -h|--help) usage; exit 0 ;;
    *)
      echo "Unknown argument: $1" >&2
      usage
      exit 1
      ;;
  esac
done

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/.." && pwd)"

if [[ "$list_templates" -eq 1 ]]; then
  list_catalog_templates "$repo_root"
  exit 0
fi

if [[ "$list_libraries" -eq 1 ]]; then
  list_available_libraries "$repo_root"
  exit 0
fi

if [[ "$template_set" -eq 0 || -z "$template" ]]; then
  template="$(choose_template_from_catalog "$repo_root")"
fi

if [[ -z "${name}" ]]; then
  read -r -p "Enter new project folder name: " name
fi

if [[ -z "${name}" ]]; then
  echo "Project name cannot be empty." >&2
  exit 1
fi

if [[ "$name" == *"/"* || "$name" == *"\\"* ]]; then
  echo "Project name must be a folder name only (no path separators)." >&2
  exit 1
fi

if [[ "$dest_set" -eq 0 || -z "$dest" ]]; then
  dest_root="$(choose_destination_folder "$repo_root")"
elif [[ "$dest" = /* ]]; then
  dest_root="$dest"
else
  dest_root="$repo_root/$dest"
fi

mkdir -p "$dest_root"

target_path="$dest_root/$name"
if [[ -e "$target_path" ]]; then
  if [[ "$force" -ne 1 ]]; then
    echo "Destination already exists: $target_path (use --force to replace)." >&2
    exit 1
  fi
  rm -rf "$target_path"
fi

temp_clone=""
cleanup() {
  if [[ -n "${temp_clone}" && -d "${temp_clone}" ]]; then
    rm -rf "${temp_clone}"
  fi
}
trap cleanup EXIT

catalog_entry=""
if catalog_entry="$(read_catalog_entry "$repo_root" "$template")"; then
  source_type="$(echo "$catalog_entry" | cut -d'|' -f1)"
  location="$(echo "$catalog_entry" | cut -d'|' -f2)"
  branch="$(echo "$catalog_entry" | cut -d'|' -f3)"
  catalog_subdir="$(echo "$catalog_entry" | cut -d'|' -f4)"
  if [[ -z "$template_subdir" && -n "$catalog_subdir" ]]; then
    template_subdir="$catalog_subdir"
  fi

  if [[ "$source_type" == "git" ]]; then
    template="$location"
    template_branch="$branch"
  else
    template_path="$(resolve_template_path "$repo_root" "$location")"
    template_branch=""
  fi
else
  template_branch=""
fi

if [[ -z "${template_path:-}" ]] && is_git_template "$template"; then
  if ! command -v git >/dev/null 2>&1; then
    echo "Git is required to use a template repository URL." >&2
    exit 1
  fi
  temp_clone="$(mktemp -d)"
  if [[ -n "$template_branch" ]]; then
    git clone --depth 1 --branch "$template_branch" "$template" "$temp_clone" >/dev/null
  else
    git clone --depth 1 "$template" "$temp_clone" >/dev/null
  fi
  template_path="$temp_clone"
  if [[ -n "$template_subdir" ]]; then
    template_path="$template_path/$template_subdir"
  fi
  if [[ ! -d "$template_path" ]]; then
    echo "Template subdir not found: $template_subdir" >&2
    exit 1
  fi
else
  if [[ -z "${template_path:-}" ]]; then
    template_path="$(resolve_template_path "$repo_root" "$template")"
  fi
fi

mkdir -p "$target_path"
cp -a "$template_path"/. "$target_path"

# Remove source-control/build artifacts from the scaffold.
rm -rf "$target_path/.git" "$target_path/.qtcreator" "$target_path/build" "$target_path/CMakeFiles"
find "$target_path" -name CMakeCache.txt -type f -delete
find "$target_path" -name .git -type d -prune -exec rm -rf {} +
find "$target_path" -name .qtcreator -type d -prune -exec rm -rf {} +
find "$target_path" -name build -type d -prune -exec rm -rf {} +

set_project_libraries "$target_path" "$libraries"
if [[ -n "$libraries" ]]; then
  for lib in $(echo "$libraries" | tr ',;' '  '); do
    if [[ -n "$lib" ]] && ! library_exists "$repo_root" "$lib"; then
      echo "Warning: library '$lib' was not found under libraries/. Keeping it as specified." >&2
    fi
  done
fi

if [[ "$git_init" -eq 1 ]]; then
  if command -v git >/dev/null 2>&1; then
    (cd "$target_path" && git init >/dev/null)
  else
    echo "Git not found; skipped git init." >&2
  fi
fi

echo "Created new app at: $target_path"
echo "Template source: $template"
if [[ -n "$libraries" ]]; then
  echo "Libraries: $libraries"
fi
if [[ "$git_init" -eq 1 ]]; then
  echo "Initialized new git repo."
fi
echo "Next step: re-run CMake for your container folder (for example: apps/CMakeLists.txt)."
