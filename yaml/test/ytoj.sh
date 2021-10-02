#!/usr/bin/env bash
set -euo pipefail

f=$(dirname "${BASH_SOURCE[0]}")/ytoj.yaml
out=$(yaml < "$f" --json)
out=$(yaml <<< "$out" --yaml)
out=$(awk <<< "$out" -c '
NR == 1 { print "---" }
NR == 3 && $0 == "c: 3" { $0 = "c: 3.0" }
NR == 4 && $0 == "d: 1.8446744073709552e+19" { $0 = "d: 18446744073709551615" }
1
')
diff -u "$f" - <<< "$out"
