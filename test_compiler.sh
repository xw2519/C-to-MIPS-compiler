#!/bin/bash

echo " Cleaning the temporaries and outputs"
make clean
echo "========================================="
echo " Compiling"
make all
echo "========================================="


input_dir="compiler_tests/"
working="output/"

mkdir -p ${working}
rm ${working}/*

for j in ${input_dir}/* ; do

    test_group=$j;
    echo "TESTING [$(basename $test_group)] :"
    for i in ${test_group}/*.driver.c ; do

        base=$(echo $i | sed -E -e "s|${test_group}/([^.]+)[.]driver[.]c|\1|g");

        # Generate assembly from test program (base.c to base.s)
        bin/c_compiler -S ${test_group}/${base}.c -o ${working}/${base}.s

        # Assemble test program (base.s to base.o)
        mips-linux-gnu-gcc -mfp32 -o ${working}/${base}.o -c ${working}/${base}.s

        # Link test program with driver
        mips-linux-gnu-gcc -mfp32 -static -o ${working}/${base}.out ${working}/${base}.o ${test_group}/${base}_driver.c

        # Test with qemu
        qemu-mips ${working}/${base}.out
        TEST_OUT=$?

        if [[ $TEST_OUT -ne 0 ]] ; then
            echo "$base, Fail, $TEST_OUT"
        else
            echo "$base, Pass"
        fi
    done
done
