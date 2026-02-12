#!/usr/bin/env python3
"""Copy repository files into category folders.

Categories:
- lisp-programs
- other-programs
- email
- documents
- other

By default, this script copies tracked repository files and preserves each file's
relative path under a category directory inside ./organized-files.
"""

from __future__ import annotations

import argparse
import subprocess
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parent
DEFAULT_DEST_ROOT = ROOT / "organized-files"

CATEGORIES = {
    "lisp-programs": {".lisp", ".lsp", ".cl", ".el", ".scm", ".ss"},
    "other-programs": {
        ".sh", ".bash", ".zsh", ".py", ".js", ".ts", ".jsx", ".tsx", ".rb", ".php",
        ".java", ".c", ".h", ".cpp", ".hpp", ".cc", ".cs", ".go", ".rs", ".swift",
        ".kt", ".m", ".mm", ".pl", ".sql", ".bat", ".ps1", ".lua", ".mak", ".rc",
        ".ide", ".dsp", ".dsw"
    },
    "email": {".eml", ".mbox", ".msg"},
    "documents": {
        ".md", ".txt", ".pdf", ".doc", ".docx", ".rtf", ".html", ".htm", ".odt", ".rst"
    },
}


def classify(path: Path) -> str:
    suffix = path.suffix.lower()
    for category, exts in CATEGORIES.items():
        if suffix in exts:
            return category
    return "other"


def tracked_files(root: Path) -> list[Path]:
    result = subprocess.run(
        ["git", "ls-files"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    files: list[Path] = []
    for rel in result.stdout.splitlines():
        path = root / rel
        if path.is_file():
            files.append(path)
    return files


def copy_files(dest_root: Path) -> int:
    dest_root.mkdir(exist_ok=True)
    for category in [*CATEGORIES.keys(), "other"]:
        (dest_root / category).mkdir(parents=True, exist_ok=True)

    copied_count = 0
    for path in tracked_files(ROOT):
        rel = path.relative_to(ROOT)

        # Avoid recursive copies if destination is committed in the future.
        if rel.parts and rel.parts[0] == dest_root.name:
            continue

        category = classify(path)
        destination = dest_root / category / rel
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, destination)
        copied_count += 1

    return copied_count


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Organize repository files by category.")
    parser.add_argument(
        "--output",
        default=str(DEFAULT_DEST_ROOT),
        help="Destination directory (default: ./organized-files)",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    destination = Path(args.output).resolve()
    copied = copy_files(destination)
    print(f"Copied {copied} files into {destination}")


if __name__ == "__main__":
    main()
