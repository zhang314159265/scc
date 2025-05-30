#!/bin/bash

set -e

function logprogress() {
  msg=$1
  shift
  echo -e "\033[32m$msg\033[0m"
}

cnt=0

function testit() {
  cnt=`expr $cnt + 1`
  testname=$1
  shift
  expectedMsg=$1
  shift

  ./scc < inputs/$testname.c >/tmp/scclog 2>&1
  clang /tmp/gen.ll >/tmp/clanglog 2>&1
  ./a.out > /tmp/log
  if ! grep -q "$expectedMsg" /tmp/log; then
    echo "Fail test $testname. Pattern '$expectedMsg' not found. Check output file /tmp/log"
    exit 1
  fi

  logprogress "PASS $testname"
}

# make scc
testit showmsg "LLVM is cool!"
testit sum "Sum is 5050"
testit ifelse "d is 5"
testit 1d_fp_arr "sqrt of 8 is 2.83"
testit ident_mat "sum is 10.000000"
testit matmul "[[8, 5], [5, 3]]"
testit factorial "24"
testit nqueen "8 queen solution: 92"

logprogress "SUMMARY: PASS $cnt TESTS"
