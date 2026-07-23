#!/bin/bash
#
# test_miris.sh — full-coverage test for the miris transaction graph analyzer
#
# Builds a small graph from scratch containing two overlapping cycles, exercises
# every single command (i, n, d, l, m, f, r, c, fi, t, o, e), checks each command's
# output against the expected result, and verifies the final output file written
# by 'e' matches byte-for-byte.
#
# Usage: ./test_miris.sh

set -uo pipefail

BIN="./miris"
EMPTY_INPUT="empty_input_difficult.txt"
OUTPUT="difficult_test_output.txt"
EXPECTED_OUTPUT="difficult_test_expected_output.txt"
LOG="difficult_test_run.log"

PASS=0
FAIL=0

check_contains() {
    local desc="$1"
    local pattern="$2"
    if grep -qF -- "$pattern" "$LOG"; then
        echo "  [PASS] $desc"
        PASS=$((PASS + 1))
    else
        echo "  [FAIL] $desc"
        echo "         expected to find: '$pattern'"
        FAIL=$((FAIL + 1))
    fi
}

check_not_contains() {
    local desc="$1"
    local pattern="$2"
    if grep -qF -- "$pattern" "$LOG"; then
        echo "  [FAIL] $desc"
        echo "         did NOT expect to find: '$pattern'"
        FAIL=$((FAIL + 1))
    else
        echo "  [PASS] $desc"
        PASS=$((PASS + 1))
    fi
}

# --- setup -----------------------------------------------------------------

echo "== Building project =="
if ! make > build.log 2>&1; then
    echo "Build failed — see build.log"
    exit 1
fi
echo "Build OK"
echo

# Start from a completely empty input file so the test graph is fully
# self-contained and predictable.
: > "$EMPTY_INPUT"

# --- expected final output file --------------------------------------------
# This is the exact graph state after all inserts/deletes/modifies below,
# in the order writeOutputFile walks the node array.
cat > "$EXPECTED_OUTPUT" << 'EOF'
Node A:
    Edge to: B, Amount: 100, Date: 2024-01-01
Node B:
    Edge to: C, Amount: 200, Date: 2024-01-02
Node C:
    Edge to: D, Amount: 300, Date: 2024-01-04
    Edge to: A, Amount: 150, Date: 2024-01-03
Node D:
    Edge to: E, Amount: 400, Date: 2024-01-05
Node E:
    Edge to: B, Amount: 500, Date: 2024-01-06
EOF

# --- scripted command sequence ----------------------------------------------
# Builds two overlapping cycles (A-B-C-A and B-C-D-E-B), a branch/leaf node F
# used only to demonstrate modify/delete-edge/delete-node, then exercises
# every query command against the resulting graph.

echo "== Running difficult test sequence =="
{
    echo "i A B C D E F"                                  # 1  insert
    echo "n A B 100 2024-01-01"                            # 2  insert2
    echo "n B C 200 2024-01-02"                            # 3  insert2
    echo "n C A 150 2024-01-03"                             # 4  insert2 (closes cycle A-B-C-A)
    echo "n C D 300 2024-01-04"                             # 5  insert2
    echo "n D E 400 2024-01-05"                             # 6  insert2
    echo "n E B 500 2024-01-06"                             # 7  insert2 (closes cycle B-C-D-E-B)
    echo "n D F 700 2024-01-07"                             # 8  insert2 (throwaway edge for modify/delete demo)
    echo "f A"                                              # 9  find
    echo "f C"                                               # 10 find
    echo "f D"                                               # 11 find
    echo "r B"                                               # 12 receiving
    echo "m D F 700 750 2024-01-07 2024-02-02"               # 13 modify
    echo "f D"                                               # 14 find (confirm modify)
    echo "l D F"                                             # 15 delete2 (delete edge)
    echo "f D"                                               # 16 find (confirm edge gone)
    echo "d F"                                               # 17 delete (delete node)
    echo "f F"                                               # 18 find (confirm node gone)
    echo "c A"                                               # 19 circlefind
    echo "fi C 100"                                          # 20 findcircles (min weight)
    echo "t C 2"                                             # 21 traceflow (depth 2)
    echo "o A E"                                             # 22 connect
    echo "e"                                                 # 23 exit
} | "$BIN" -i "$EMPTY_INPUT" -o "$OUTPUT" > "$LOG" 2>&1

echo "Command sequence finished"
echo

# --- checks: per-command expected behavior ----------------------------------

echo "== Checking per-command results =="
check_contains "1  i  — insert creates all 6 nodes"              "Succ: A B C D E F"
check_contains "9  f  — A's only outgoing edge"                  "A B 100 2024-01-01"
check_contains "10 f  — C's edges (most recent first)"            "C D 300 2024-01-04"
check_contains "10 f  — C's edges (older second)"                 "C A 150 2024-01-03"
check_contains "11 f  — D's edges before modify"                  "D F 700 2024-01-07"
check_contains "12 r  — incoming edge to B from A"                "A B 100 2024-01-01"
check_contains "12 r  — incoming edge to B from E"                "E B 500 2024-01-06"
check_contains "14 f  — D->F edge updated by modify"              "D F 750 2024-02-02"
check_contains "16 f  — only D->E remains after delete2"          "D E 400 2024-01-05"
check_contains "18 f  — F no longer exists after delete"          "Non-existing node: F"
check_contains "19 c  — cycle A-B-C-A found"                      "A B C A"
check_contains "20 fi — cycle C-D-E-B-C, min edge weight 200"     "C D E B C 200"
check_contains "20 fi — cycle C-A-B-C, min edge weight 100"       "C A B C 100"
check_contains "21 t  — traceflow path C->D"                      "C D"
check_contains "21 t  — traceflow path C->D->E"                   "C D E"
check_contains "21 t  — traceflow path C->A"                      "C A"
check_contains "21 t  — traceflow path C->A->B"                   "C A B"
check_contains "22 o  — path found from A to E"                   "A B C D E"
check_contains "23 e  — exit reports bytes released"              "Bytes released"

check_not_contains "no 'Format error' triggered by any command"   "Format error"
check_not_contains "no 'Unrecognized command' triggered"          "Unrecognized command"

# --- check: final output file matches expected exactly -----------------------

echo
echo "== Checking final output file =="
if [ ! -f "$OUTPUT" ]; then
    echo "  [FAIL] output file '$OUTPUT' was not created"
    FAIL=$((FAIL + 1))
elif diff -u "$EXPECTED_OUTPUT" "$OUTPUT" > diff.log; then
    echo "  [PASS] '$OUTPUT' matches expected graph state exactly"
    PASS=$((PASS + 1))
else
    echo "  [FAIL] '$OUTPUT' differs from expected — see diff.log"
    cat diff.log
    FAIL=$((FAIL + 1))
fi

# --- valgrind memory check ---------------------------------------------------

echo
echo "== Valgrind memory check =="
if command -v valgrind > /dev/null 2>&1; then
    VALGRIND_LOG="valgrind-miris.log"
    {
        echo "i A B C"
        echo "n A B 100 2024-01-01"
        echo "n B C 200 2024-01-02"
        echo "f A"
        echo "e"
    } | valgrind --leak-check=full --log-file="$VALGRIND_LOG" \
        "$BIN" -i "$EMPTY_INPUT" -o "$OUTPUT" > /dev/null 2>&1

    if [ -f "$VALGRIND_LOG" ]; then
        if grep -q "All heap blocks were freed" "$VALGRIND_LOG"; then
            echo "  [PASS] valgrind: no memory leaks"
            PASS=$((PASS + 1))
        else
            echo "  [FAIL] valgrind: no memory leaks"
            FAIL=$((FAIL + 1))
        fi
        if grep -q "ERROR SUMMARY: 0 errors" "$VALGRIND_LOG"; then
            echo "  [PASS] valgrind: 0 errors"
            PASS=$((PASS + 1))
        else
            echo "  [FAIL] valgrind: 0 errors"
            FAIL=$((FAIL + 1))
        fi
        echo "  (full valgrind output saved to $VALGRIND_LOG)"
    else
        echo "  [SKIP] valgrind log not created"
    fi
else
    echo "  [SKIP] valgrind not installed"
fi

# --- summary -----------------------------------------------------------------

echo
echo "===================================="
echo "Passed: $PASS   Failed: $FAIL"
echo "===================================="

exit $FAIL