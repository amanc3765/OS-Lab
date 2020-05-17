#!/bin/bash
if [ "$#" -ne 1 ]; then 
    echo "Usage: ./runTestcase.sh testcase"
    exit
fi
testcase=$1
cp $testcase testcase.c
gcc testcase.c simplefs-ops.c simplefs-disk.c
./a.out
rm -f testcase.c
rm -f a.out
