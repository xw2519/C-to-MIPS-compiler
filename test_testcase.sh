
bin/compiler compiler_tests/functions/call_constant_external.c \
    > working/output/compiled.txt

#!/bin/bash

echo "=================================================================================="
echo "                   Cleaning the temporaries and outputs                           "
echo "=================================================================================="

make clean

echo "=================================================================================="
echo "                               Compiling                                          "
echo "=================================================================================="

make all

echo "=================================================================================="
echo "                              Running test                                        "
echo "=================================================================================="

COMPILER=bin/c_compiler

input_dir="compiler_tests/default/test_CALL"
output_dir="temp/"
rm -r ${output_dir}
mkdir -p ${output_dir}

# Compile test function with compiler under test 
${COMPILER} -S ${input_dir}.c -o ${output_dir}testcase.s

# Compile driver with normal GCC
mips-linux-gnu-gcc -mfp32 -o ${output_dir}object_file.o -c ${output_dir}testcase.s

# Link driver object and assembly into executable
mips-linux-gnu-gcc -mfp32 -static -o ${output_dir}EXEC ${output_dir}object_file.o ${input_dir}_driver.c

# Run the actual executable
qemu-mips ${output_dir}EXEC

# Check result output
ret=$?
if [[ $ret -ne 0 ]]; then
    echo "FAILED! Testcase returned $ret, but expected 0."

else
    echo "PASSED!"

fi
