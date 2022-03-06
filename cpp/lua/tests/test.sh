#!/bin/bash
set -euo pipefail

FIX_PARENTHESES='tests/gcc6.c
tests/gcc7.c
tests/gcc17.c
tests/std2.c
tests/std3.c
tests/std4.c
tests/std5.c
'
FIX_RECURSION='tests/gcc14.c
'
FIX_SPACES=tests/gcc19.c
FIX="$FIX_PARENTHESES
$FIX_RECURSION
$FIX_SPACES
"

main() {
    [[ "$#" -eq 0 ]] && set -- tests/*.c
    local f e out ret=0
    for f; do
        grep --quiet --line-regexp --file <(echo "$FIX") <<< "$f" && continue
        e=tests/expected/$(basename "$f")
        out=$(lua cpp.lua "$f" || ret=$?)
        check_expected "$f" "$e" || ret=$?
        check_output "$e" <<< "$out" || ret=$?
    done
    return "$ret"
}

check_expected() {
    local f=$1 e=$2 ret=0
    local diff='diff --unified --ignore-blank-lines'
    if ! cpp -P "$f" | $diff - "$e"; then
        echo >&2 unexpected cpp output
        ret=1
    fi
    if ! clang-cpp -P "$f" | $diff - "$e"; then
        echo >&2 unexpected clang-cpp output
        ret=1
    fi
    return "$ret"
}

check_output() {
    local e=$1
    diff --unified --ignore-blank-lines - "$e"
}

main "$@"
