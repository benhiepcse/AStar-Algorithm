import argparse
from pathlib import Path
import re
import subprocess
import sys


def inspect(names, student):
    required = {f"{student}/{name}" for name in (
        "PathNode.h", "PathNode.cpp", "Algo.h", "Algo.cpp", "main.cpp",
        f"{student}.pdf",
    )}
    required_folded = {name.casefold() for name in required}
    entries = [name.replace("\\", "/") for name in names]
    issues = []
    seen = set()

    for name in entries:
        parts = name.split("/")
        if any(part in ("", ".", "..") or ":" in part for part in parts):
            issues.append(("STRUCTURE", name))
            continue

        folded = name.casefold()
        if folded in seen:
            issues.append(("STRUCTURE", f"duplicate {name}"))
        seen.add(folded)

        if len(parts) == 1:
            issues.append(("STRUCTURE", name))
        elif parts[0] != student:
            issues.append(("FOLDER_NAME", name))
        elif len(parts) != 2:
            issues.append(("STRUCTURE", name))
        elif name not in required:
            code = "FILE_NAME" if (
                folded in required_folded or name.lower().endswith(".pdf")
            ) else "EXTRA"
            issues.append((code, name))

    for name in sorted(required - set(entries)):
        issues.append(("MISSING", name))
    return issues


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("student")
    parser.add_argument("archive", type=Path)
    parser.add_argument("--seven-zip", default="7z")
    args = parser.parse_args()

    if not re.fullmatch(r"[0-9]+", args.student):
        parser.error("student must contain digits only")

    path = args.archive.resolve()
    issues = []
    if path.name != f"{args.student}.rar":
        issues.append(("FILE_NAME", path.name))

    try:
        with path.open("rb") as stream:
            magic = stream.read(8)
    except OSError as error:
        print(f"NOT_RUN: {error}")
        return 2

    rar_signatures = (b"Rar!\x1a\x07\x00", b"Rar!\x1a\x07\x01\x00")
    if not magic.startswith(rar_signatures):
        issues.append(("FORMAT", "RAR signature missing"))
    else:
        try:
            listing = subprocess.run(
                [args.seven_zip, "l", "-slt", "-sccUTF-8", str(path)],
                capture_output=True, text=True, encoding="utf-8", errors="replace",
            )
            integrity = subprocess.run(
                [args.seven_zip, "t", str(path)], capture_output=True,
            )
        except OSError as error:
            for code, detail in issues:
                print(f"{code}: {detail}")
            print(f"NOT_RUN: {error}")
            return 2

        if listing.returncode or integrity.returncode:
            issues.append(("INTEGRITY", "7-Zip could not list or test the archive"))
        else:
            _, separator, entries = listing.stdout.partition("----------")
            if not separator:
                print("NOT_RUN: archive listing format not recognized")
                return 2

            names = []
            for record in re.split(r"\n\s*\n", entries.strip()):
                fields = dict(line.split(" = ", 1) for line in record.splitlines()
                              if " = " in line)
                name = fields.get("Path")
                if not name:
                    continue

                if fields.get("Symbolic Link") or fields.get("Hard Link"):
                    issues.append(("STRUCTURE", f"link {name}"))

                attributes = fields.get("Attributes", "")
                if fields.get("Folder") != "+" and not attributes.startswith("D"):
                    names.append(name)
                else:
                    directory = name.replace("\\", "/").rstrip("/")
                    if directory != args.student:
                        code = "STRUCTURE" if "/" in directory else "FOLDER_NAME"
                        issues.append((code, directory))

            issues.extend(inspect(names, args.student))

    for code, detail in issues:
        print(f"{code}: {detail}")
    failed = any(code != "EXTRA" for code, _ in issues)
    print("FAIL" if failed else "PASS: archive listing and integrity")
    return int(failed)


if __name__ == "__main__":
    sys.exit(main())
