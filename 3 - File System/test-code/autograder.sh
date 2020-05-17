#!/bin/bash
if [ "$#" -ne 2 ]; then 
    echo "Usage: ./autograder.sh expected_output your_output"
    exit
fi
echo "Expected output: $1"
echo "Your actual output: $2"
EXPECTED=$1
ACTUAL=$2
correct=0
total=0
for expected in $EXPECTED/*.out; do
    name=`basename $expected`
    actual=$ACTUAL/$name
    total=$((total+1))
    echo "Comparing $expected and $actual"
    diff -w -B $expected $actual
		if [ "$?" == "0" ]; then
			echo "Test Case Passed"
			correct=$((correct+1))
		else
			echo "Test Case Failed"
		fi
		echo ""
		echo ""
done
echo "Test Cases Passed: $correct"
echo "Test Cases Total: $total"

