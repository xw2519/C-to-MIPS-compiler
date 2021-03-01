
bin/compiler compiler_tests/functions/call_constant_external.c \
    > working/output/compiled.txt

#!/bin/bash

echo "========================================="
echo " Cleaning the temporaries and outputs"
make clean
echo "========================================="

echo "========================================="
echo " Compiling"
make all
echo "========================================="


input_dir="compiler_tests/basic/function_empty"
working="output/basic"

mkdir -p ${working}
rm ${working}/*

for j in ${input_dir}/* ; do

    test_group=$j;
    if [ -z "$(ls -A ${test_group})" ]; then
        echo "NO TESTS FOR [$(basename $test_group)]"
    else
        echo "TESTING [$(basename $test_group)] :"
        for i in ${test_group}/*.driver.c ; do

            base=$(echo $i | sed -E -e "s|${test_group}/([^.]+)[.]driver[.]c|\1|g");

         	# Compile the driver to .o
            mips-linux-gnu-gcc -march=mips1 -mfp32  -S $i -o $working/$base.driver.s

            if [[ ${have_compiler} -eq 1 ]] ; then
                # Compile to .c to .s
                $compiler --compile ${test_group}/$base.c -o ${working}/$base-slave.s
                
                # Link driver and slave
                mips-linux-gnu-gcc -std=c89 -march=mips1 -mfp32 --static -O0 ${working}/$base-slave.s $working/$base.driver.s -o $working/$base-test
                
                # Run driver
                qemu-mips $working/$base-test
                TEST_OUT=$?    
            fi

            if [[ $TEST_OUT -ne 0 ]] ; then
                echo "$base, Fail, $TEST_OUT"
            else
                echo "$base, Pass"
            fi
        done
    fi

done