#!/usr/bin/env python3
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parent
DEST_ROOT = ROOT / "organized-files"

CATEGORIES = {
    "lisp-programs": {".lisp", ".lsp", ".cl", ".el", ".scm", ".ss"},
    "other-programs": {
        ".sh", ".bash", ".zsh", ".py", ".js", ".ts", ".jsx", ".tsx", ".rb", ".php",
        ".java", ".c", ".h", ".cpp", ".hpp", ".cc", ".cs", ".go", ".rs", ".swift",
        ".kt", ".m", ".mm", ".pl", ".sql", ".bat", ".ps1", ".lua"
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


def main() -> None:
    DEST_ROOT.mkdir(exist_ok=True)
    for category in [*CATEGORIES.keys(), "other"]:
        (DEST_ROOT / category).mkdir(exist_ok=True)

    for path in ROOT.iterdir():
        if not path.is_file():
            continue
        if path.name == Path(__file__).name:
            continue

        category = classify(path)
        shutil.copy2(path, DEST_ROOT / category / path.name)


if __name__ == "__main__":
    main()
