#!/bin/bash
if [ "$#" -ne 2 ]; then 
    echo "Usage: ./generateOutput.sh testdir outputdir"
    exit
fi
echo "Testcases: $1"
echo "Output: $2"
TESTDIR=$1
OUTDIR=$2
rm -rf $OUTDIR
mkdir $OUTDIR
for filename in $TESTDIR/*.c; do
    name=`basename $filename`
    name="${name%.*}"
    outfile=$OUTDIR/$name.out
    echo "Running testcase $filename: Output stored in $outfile"
    cp $filename testcase.c
    gcc testcase.c simplefs-ops.c simplefs-disk.c
    ./a.out > $outfile
    rm -f testcase.c
    rm -f a.out
done
