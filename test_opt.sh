#!/bin/bash

set -e

OUT_FILE_NAME=/tmp/opt.out

function logprogress() {
  msg=$1
  shift
  echo -e "\033[32m$msg\033[0m"
}

function check_pattern() {
  pat=$1
  shift
  if ! grep -q "$pat" $OUT_FILE_NAME; then
    echo "Fail test ${FUNCNAME[1]}. Pattern '$pat' not found. Check output file $OUT_FILE_NAME"
    exit 1
  fi
}

function test_reaching_definition() {
  ./opt reaching_definition llinputs/reaching_definition.ll >$OUT_FILE_NAME 2>&1
  check_pattern "^entry in:$"
  check_pattern "^entry out:$"

  check_pattern "^b1 in:$"
  check_pattern "^b1 out: d1 d2 d3$"

  check_pattern "^b2 in: d1 d2 d3 d5 d6 d7$"
  check_pattern "^b2 out: d3 d4 d5 d6$"

  check_pattern "^b3 in: d3 d4 d5 d6$"
  check_pattern "^b3 out: d4 d5 d6$"

  check_pattern "^b4 in: d3 d4 d5 d6$"
  check_pattern "^b4 out: d3 d5 d6 d7$"

  check_pattern "^exit in: d3 d5 d6 d7$"
  check_pattern "^exit out: d3 d5 d6 d7$"

  logprogress "PASS ${FUNCNAME[0]}"
}

function test_dominator_tree() {
  ./opt dominator_tree llinputs/dominator_tree.ll >$OUT_FILE_NAME 2>&1

  check_pattern "^b1 has no dominator$"
  check_pattern "^b2 has imm dom b1$"
  check_pattern "^b3 has imm dom b1$"
  check_pattern "^b4 has imm dom b3$"
  check_pattern "^b5 has imm dom b4$"
  check_pattern "^b6 has imm dom b4$"
  check_pattern "^b7 has imm dom b4$"
  check_pattern "^b8 has imm dom b7$"
  check_pattern "^b9 has imm dom b8$"
  check_pattern "^b10 has imm dom b8$"

  logprogress "PASS ${FUNCNAME[0]}"

}

test_reaching_definition
test_dominator_tree
logprogress "PASS ALL"
