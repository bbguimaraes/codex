#!/bin/bash
set -euo pipefail

main() {
    if [[ "$#" -lt 1 ]]; then
        echo >&2 "Usage: $0 commands..."
        return 1
    fi
    random "$RANDOM" "$@"
    random 1024 "$@"
    random $((1024 * 1024)) "$@"
    random $((1024 * 1024 * 1024)) "$@"
}

random() {
    local size=$1; shift
    local input=/tmp/base64_input.txt
    local output=/tmp/base64_output.txt
    local cmd
    echo random "$size"
    head -c "$size" < /dev/urandom > "$input"
    for cmd; do
        echo "$cmd"
        random_run $cmd
    done
}

random_run() {
    time -p $* < "$input" | $* -d > "$output"
    md5sum < "$output" | head -c 32
    echo
}

main "$@"
