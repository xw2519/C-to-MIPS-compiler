
bin/compiler compiler_tests/functions/call_constant_external.c \
    > working/output/compiled.txt

#!/bin/bash

echo "========================================="
echo " Cleaning the temporaries and outputs"
echo "========================================="
make clean

echo "========================================="
echo " Compiling"
echo "========================================="
make all

COMPILER=bin/c_compiler

input_dir="compiler_tests/basic/function_empty"
output_dir="temp/"
mkdir -p ${output_dir}

# Compile test function with compiler under test 
$COMPILER < compiler_tests/control_flow/if_true.c > add_driver.s 

# Compile driver with normal GCC
mips-linux-gnu-gcc -mfp32 -o add_driver.o -c add_driver.s

# Link driver object and assembly into executable
mips-linux-gnu-gcc -mfp32 -static -o EXEC add_driver.o compiler_tests/control_flow/if_true_driver.c

# Run the actual executable
qemu-mips EXEC

# Check result output
ret=$?
if [[ $ret -ne 0 ]]; then
    echo "FAILED! Testcase returned $ret, but expected 0."

else
    echo "PASSED!"

fi
