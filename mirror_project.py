#!/usr/bin/env python3
"""
Creates a filtered mirror copy of files and directories specified in a
configuration file or listed as arguments, from the current working directory.

Includes relevant files directly within the current working directory if specified
or if they match inclusion criteria when processing the root.

Designed to extract relevant source code, config, and documentation
for analysis or sharing, excluding build artifacts, dependencies,
large data/media files, and VCS metadata.

NEW: Added support for "exclude_paths" in the config file, which overrides
     "include_paths" and internal filters.

Example Usage (run from project root):
  # Use a configuration file
  python mirror_project.py --config mirror_config_soft_constraints.json --dest ../mirror_soft_constraints

  # Specify items directly (legacy/alternative mode)
  python mirror_project.py scheduler tests README.md --dest ../mirror_manual
"""

import os
import shutil
import argparse
import fnmatch
import sys
from pathlib import Path
import logging
import stat # Import stat for checking permissions
import json # For loading config file
from typing import List, Dict, Any, Set # Added Set typing

# --- Configuration: Define what to include and exclude (Defaults) ---
# These act as filters *within* included directories or when processing root files.
# They don't determine the top-level items to include if using a config file.
INCLUDED_EXTENSIONS = {
    # Source Code
    ".py", ".pyw", ".java", ".js", ".jsx", ".ts", ".tsx", ".html", ".htm",
    ".css", ".scss", ".sass", ".c", ".cpp", ".h", ".hpp", ".cs", ".go",
    ".rs", ".swift", ".kt", ".kts", ".php", ".rb", ".pl", ".sh", ".bash",
    ".sql",".vue",".cmake",
    # Config & Data Formats (Text-based)
    ".json", ".yaml", ".yml", ".xml", ".ini", ".toml", ".cfg", ".conf",
    ".properties", ".tf", ".hcl", ".gradle", ".mvn", ".tfvars",
    # Documentation & Text
    ".md", ".txt", ".rst",
    # Requirements / Env / Setup
    ".env", # Be cautious with sensitive data
    # Data (selectively)
    ".csv",
}
INCLUDED_FILENAMES = {
    "requirements.txt", "Pipfile", "Pipfile.lock", "pyproject.toml",
    "setup.py", "setup.cfg", "pom.xml", "build.gradle", "settings.gradle",
    "README", "LICENSE", "CONTRIBUTING", "NOTICE", "SECURITY.md",
    "Makefile", "Dockerfile", "docker-compose.yml", "docker-compose.yaml",
    ".dockerignore", ".gitignore", ".gitattributes", ".editorconfig",
    "constraints.json", "parameters.json",
}
EXCLUDED_DIR_PATTERNS = {
    ".git", ".svn", ".hg", ".idea", ".vscode", ".settings", # VCS & IDE metadata
    "__pycache__", ".pytest_cache", ".mypy_cache", ".ruff_cache", # Caches
    "node_modules", "vendor", "target", "build", "dist", "out", # Dependencies & Build output
    "bin", "obj", "*.egg-info", "site-packages",
    "venv", ".venv", "env", ".env", # Virtual Env (ignore .env dir, allow .env file)
    "*.ipynb_checkpoints", # Jupyter checkpoints
    "htmlcov", # Coverage reports
    "mirror", # IMPORTANT: Exclude the default mirror directory itself!
}
EXCLUDED_FILE_EXTENSIONS = {
    # Compiled Code / Intermediates
    ".pyc", ".pyo", ".pyd", ".class", ".jar", ".war", ".ear", ".o", ".a",
    ".so", ".dylib", ".dll", ".exe", ".nupkg",
    # Archives / Compressed
    #".zip", # Temporarily allow .zip to be included if specified, will be excluded if not.
    ".tar", ".gz", ".bz2", ".7z", ".rar", ".tgz", ".whl",
    # Large Data Files (optional - could include specific small ones via config)
    ".tsv", ".xls", ".xlsx", ".ods", ".parquet", ".avro", ".orc",
    ".db", ".sqlite", ".sqlite3", ".mdb", ".dump", ".sqlitedb",
    # Model/Binary Data
    ".pkl", ".pickle", ".joblib", ".h5", ".hdf5", ".onnx", ".pb",
    ".npy", ".npz", ".bin", ".dat", ".model", ".pt", ".pth", ".ckpt",
    # Media Files
    ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".svg", ".ico", ".webp",
    ".mp3", ".wav", ".ogg", ".flac", ".aac", ".m4a",
    ".mp4", ".avi", ".mov", ".wmv", ".mkv", ".webm", ".flv",
    # Fonts
    ".ttf", ".otf", ".woff", ".woff2", ".eot",
    # Logs (consider including based on task)
    # ".log",
    # Temporary / Backups
    ".tmp", ".temp", ".bak", ".~*", "*~", ".swp", ".swo",
    # OS Specific
    ".DS_Store", "Thumbs.db",
}

# --- Logging Setup ---
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)

# --- Helper Functions ---

def should_exclude_dir(dir_path: Path):
    """Check if a directory path or name matches any exclusion pattern."""
    base_name = dir_path.name
    for pattern in EXCLUDED_DIR_PATTERNS:
        if fnmatch.fnmatch(base_name, pattern):
            return True
    return False

def should_include_file(filename):
    """Check if a file should be included based on its name and extension."""
    if not filename: return False
    base_name = os.path.basename(filename)
    _, ext = os.path.splitext(filename)
    ext = ext.lower()
    if ext in EXCLUDED_FILE_EXTENSIONS and ext != ".zip": # Allow .zip for the archive
        print(f"Excluding {filename} due to extension: {ext}")
        return False
    if base_name in INCLUDED_FILENAMES: 
        print(f"Including {filename} due to filename: {base_name}")
        return True
    if ext in INCLUDED_EXTENSIONS: 
        print(f"Including {filename} due to extension: {ext}")
        return True
    print(f"Excluding {filename} due to filename: {base_name} not included and {ext} not included")
    return False



def handle_remove_readonly(func, path, exc_info):
    """Error handler for shutil.rmtree to handle read-only files."""
    excvalue = exc_info[1]
    if func in (os.rmdir, os.remove, os.unlink) and isinstance(excvalue, PermissionError):
        if os.path.exists(path):
            logger.debug(f"Attempting to remove read-only flag from {path}")
            os.chmod(path, stat.S_IWRITE)
            func(path)
        else: logger.warning(f"Path {path} not found during rmtree error handling.")
    else:
        logger.error(f"Error during rmtree operation: func={func}, path={path}", exc_info=True)


# --- Core Logic ---

def collect_files_from_path(source_root: Path, item_path_relative: Path, verbose: bool) -> Set[Path]:
    """
    Recursively collects relative paths of files to be copied from a given item path,
    applying *internal* inclusion/exclusion filters (extensions, filenames, dir patterns).
    Does NOT apply the top-level config "exclude_paths" here.

    Args:
        source_root: The absolute path to the project's source root directory.
        item_path_relative: The relative path (from source_root) of the item (file or dir) to process.
        verbose: Whether to print verbose logs.

    Returns:
        A set of relative paths (as Path objects) of files identified for potential copying.
    """
    files_to_copy_set: Set[Path] = set()
    item_path_abs = (source_root / item_path_relative).resolve()

    if not item_path_abs.exists():
        logger.warning(f"Specified include path '{item_path_relative}' not found. Skipping.")
        return files_to_copy_set

    if item_path_abs.is_dir():
        if should_exclude_dir(item_path_abs):
            if verbose: logger.info(f"Skipping directory matching EXCLUDED_DIR_PATTERNS: {item_path_relative}")
            return files_to_copy_set

        logger.info(f"Processing included directory: {item_path_relative}")
        for root, dirs, files in os.walk(str(item_path_abs), topdown=True):
            current_source_root = Path(root)
            dirs[:] = [d for d in dirs if not should_exclude_dir(Path(d))]

            for filename in files:
                source_file = current_source_root / filename
                if source_file.is_symlink():
                    if verbose: logger.info(f"  Skipping symlink: {source_file.relative_to(source_root)}")
                    continue
                if should_include_file(filename):
                    files_to_copy_set.add(source_file.relative_to(source_root))
                    if verbose: logger.debug(f"  Adding file (passes internal filters): {source_file.relative_to(source_root)}")
                elif verbose:
                    logger.debug(f"  Skipping file (fails internal filters): {source_file.relative_to(source_root)}")

    elif item_path_abs.is_file():
        if should_include_file(item_path_abs.name):
            files_to_copy_set.add(item_path_relative)
            if verbose: logger.debug(f"Adding file (passes internal filters): {item_path_relative}")
        elif verbose:
            logger.info(f"Skipping explicitly listed file (fails internal filters): {item_path_relative}")
    else:
        logger.warning(f"Specified include path '{item_path_relative}' is not a file or directory. Skipping.")

    return files_to_copy_set


def create_mirror(source_dir: Path, target_dir: Path, files_to_copy: Set[Path], verbose: bool):
    """Copies the specified files to the target directory, preserving structure."""
    logger.info(f"\nCreating mirror copy in '{target_dir}'...")
    count = 0
    if not files_to_copy:
        logger.warning("No files identified to copy after all filters.")
        return

    sorted_files = sorted(list(files_to_copy))

    for relative_path in sorted_files:
        source_file = source_dir / relative_path
        target_file = target_dir / relative_path
        log_rel_path = target_file.relative_to(target_dir.parent)

        if source_file.is_file():
            try:
                target_file.parent.mkdir(parents=True, exist_ok=True)
                should_copy = True
                if target_file.exists():
                     source_mtime = source_file.stat().st_mtime
                     target_mtime = target_file.stat().st_mtime
                     if source_mtime <= target_mtime:
                         should_copy = False
                         if verbose: logger.debug(f"  Skipping (up-to-date): {log_rel_path}")

                if should_copy:
                    shutil.copy2(str(source_file), str(target_file))
                    if verbose: logger.info(f"  Copied: {log_rel_path}")
                    count += 1
            except Exception as e:
                logger.warning(f"  Error copying {log_rel_path}: {e}")
        elif not source_file.exists():
            logger.warning(f"  Source file not found during copy phase, skipping: {source_file}")

    logger.info(f"Successfully copied {count} files.")


# --- Main Function ---
def main():
    parser = argparse.ArgumentParser(
        description="Create a filtered mirror copy of project files based on a configuration file or specified items.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "source_items",
        nargs='*',
        help="Optional: Relative paths (from CWD) of source files/dirs to explicitly include. "
             "Ignored if --config is used."
    )
    parser.add_argument(
        "--config",
        metavar='CONFIG_FILE',
        help="Path to a JSON configuration file specifying 'include_paths' and optionally 'exclude_paths'. "
             "If provided, 'source_items' arguments are ignored."
    )
    parser.add_argument(
        "--dest",
        metavar='DEST_DIR',
        default="../mirror",
        help="Path to the destination directory. Defaults to '../mirror'. "
             "Will be cleaned and recreated.",
    )
    parser.add_argument(
        "-v", "--verbose",
        action="store_true",
        help="Enable verbose output, showing every file/directory action.",
    )

    args = parser.parse_args()

    if args.verbose:
        logger.setLevel(logging.DEBUG)
        logger.info("Verbose logging enabled.")

    cwd = Path.cwd()
    source_root_abs = cwd.resolve()
    logger.info(f"Source root directory: {source_root_abs}")

    dest_path = Path(args.dest)
    if not dest_path.is_absolute():
        dest_path = (cwd / dest_path).resolve()

    try:
        if dest_path.exists():
            logger.warning(f"Destination '{dest_path}' exists. Removing it completely before mirroring.")
            shutil.rmtree(dest_path, onerror=handle_remove_readonly)
        dest_path.mkdir(parents=True)
        logger.info(f"Created clean destination directory: {dest_path}")
    except Exception as e:
        logger.error(f"Error preparing destination directory '{dest_path}': {e}")
        sys.exit(1)

    initial_files_to_copy: Set[Path] = set()
    exclude_paths: List[str] = []

    if args.config:
        config_path = Path(args.config)
        if not config_path.is_absolute():
            config_path = (cwd / config_path).resolve()

        logger.info(f"Using configuration file: {config_path}")
        if not config_path.is_file():
            logger.error(f"Configuration file not found: {config_path}")
            sys.exit(1)

        try:
            with open(config_path, 'r', encoding='utf-8') as f:
                config_data = json.load(f)
            include_paths = config_data.get("include_paths", [])
            exclude_paths = config_data.get("exclude_paths", [])
            if not isinstance(include_paths, list):
                raise TypeError("'include_paths' must be a list in the config file.")
            if not isinstance(exclude_paths, list):
                raise TypeError("'exclude_paths' must be a list in the config file.")

            logger.info(f"Found {len(include_paths)} include paths in config.")
            for rel_path_str in include_paths:
                item_rel_path = Path(rel_path_str)
                files_from_item = collect_files_from_path(source_root_abs, item_rel_path, args.verbose)
                initial_files_to_copy.update(files_from_item)

        except json.JSONDecodeError as e:
            logger.error(f"Error parsing JSON config file '{config_path}': {e}")
            sys.exit(1)
        except TypeError as e:
            logger.error(f"Error in config file structure '{config_path}': {e}")
            sys.exit(1)
        except Exception as e:
            logger.error(f"Error processing config file '{config_path}': {e}")
            sys.exit(1)

    elif args.source_items:
        logger.info("Using source items specified on command line.")
        for item_name in args.source_items:
            item_rel_path = Path(item_name)
            files_from_item = collect_files_from_path(source_root_abs, item_rel_path, args.verbose)
            initial_files_to_copy.update(files_from_item)
    else:
        logger.warning("No configuration file or source items specified. Mirror will be empty.")

    final_files_to_copy: Set[Path] = set()
    if exclude_paths:
        normalized_exclude_paths_abs = [
            (source_root_abs / p).resolve() for p in exclude_paths
        ]
        logger.info(f"Applying {len(exclude_paths)} exclude paths from config.")
        if args.verbose: logger.debug(f"Normalized absolute exclude paths: {[str(p) for p in normalized_exclude_paths_abs]}")

        excluded_count = 0
        for relative_path in initial_files_to_copy:
            source_file_abs = (source_root_abs / relative_path).resolve()
            source_file_abs_str = str(source_file_abs)
            is_excluded = False
            for excluded_path_abs in normalized_exclude_paths_abs:
                excluded_path_abs_str = str(excluded_path_abs)
                if source_file_abs_str.startswith(excluded_path_abs_str):
                    if excluded_path_abs.is_dir() or source_file_abs == excluded_path_abs:
                        is_excluded = True
                        break
            if not is_excluded:
                final_files_to_copy.add(relative_path)
            elif args.verbose:
                excluded_count += 1
                logger.debug(f"  Excluding file due to exclude_paths: {relative_path}")
        if excluded_count > 0 : logger.info(f"Excluded {excluded_count} files based on 'exclude_paths'.")
    else:
        final_files_to_copy = initial_files_to_copy
        logger.info("No 'exclude_paths' found or specified in config.")

    create_mirror(source_root_abs, dest_path, final_files_to_copy, args.verbose)

    # --- Create Zip Archive ---
    if final_files_to_copy: # Only create zip if there were files mirrored
        zip_file_name = dest_path.name
        # Place the zip file adjacent to the destination folder
        # e.g., if dest_path is /path/to/mirror_folder, zip is /path/to/mirror_folder.zip
        zip_file_base_path = dest_path.parent / zip_file_name
        try:
            logger.info(f"Creating zip archive of '{dest_path.name}' at '{zip_file_base_path}.zip'...")
            shutil.make_archive(str(zip_file_base_path), 'zip', root_dir=dest_path.parent, base_dir=dest_path.name)
            logger.info(f"Successfully created zip archive: {zip_file_base_path}.zip")
        except Exception as e:
            logger.error(f"Error creating zip archive: {e}")
    # --- End Create Zip Archive ---


    print("=" * 30)
    print("Selective mirroring complete.")
    print(f"  Configuration File Used: {args.config if args.config else 'N/A'}")
    print(f"  Files Copied: {len(final_files_to_copy)}")
    print(f"  Output location: {dest_path}")
    if final_files_to_copy :
        print(f"  Zip archive: {zip_file_base_path}.zip")
    print("=" * 30)

if __name__ == "__main__":
    main()