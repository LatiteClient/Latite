#!/usr/bin/env bash

set -u

if ! command -v clang-format >/dev/null 2>&1; then
    echo "clang-format was not found on PATH."
    echo "Install clang-format or add it to PATH before committing C++ changes."
    exit 1
fi

failed=0
found=0
tmp_files=()

cleanup() {
    if [[ "${#tmp_files[@]}" -gt 0 ]]; then
        rm -f "${tmp_files[@]}"
    fi
}
trap cleanup EXIT

while IFS= read -r -d '' file; do
    case "$file" in
        *.c|*.cc|*.cxx|*.cpp|*.h|*.hpp)
            ;;
        *)
            continue
            ;;
    esac

    found=1
    if ! checkout_output="$(git checkout-index --temp -- "$file")"; then
        echo "Unable to read staged contents for $file."
        failed=1
        continue
    fi

    tmp_file="${checkout_output%%$'\t'*}"
    if [[ -z "$tmp_file" || ! -f "$tmp_file" ]]; then
        echo "Unable to read staged contents for $file."
        failed=1
        continue
    fi

    tmp_files+=("$tmp_file")

    if ! clang-format --dry-run --Werror --assume-filename="$file" "$tmp_file"; then
        failed=1
    fi
done < <(git diff --cached --name-only --diff-filter=ACMR -z)

if [[ "$found" -eq 0 ]]; then
    exit 0
fi

if [[ "$failed" -ne 0 ]]; then
    echo
    echo "clang-format found staged C++ contents that need formatting."
    echo "Format individual files with:"
    echo "  clang-format -i path/to/file.cpp"
    echo
    echo "Or format all Latite sources from PowerShell with:"
    echo "  Get-ChildItem src -Recurse -Include *.h,*.hpp,*.cpp,*.cxx,*.cc | ForEach-Object { clang-format -i \$_.FullName }"
    echo
    echo "Or from Git Bash with:"
    echo "  find src -type f \\( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cxx' -o -name '*.cc' \\) -print0 | xargs -0 clang-format -i"
    echo
    exit 1
fi

exit 0
