bin/compiler compiler_tests/functions/call_constant_external.c \
    > working/output/compiled.txt

# Black         0;30     Dark Gray     1;30
# Red           0;31     Light Red     1;31
# Green         0;32     Light Green   1;32
# Brown/Orange  0;33     Yellow        1;33
# Blue          0;34     Light Blue    1;34
# Purple        0;35     Light Purple  1;35
# Cyan          0;36     Light Cyan    1;36
# Light Gray    0;37     White         1;37

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

#!/bin/bash
TEST_DIR="compiler_tests"
OUTPUT_DIR="temp"

echo "=================================================================================="
echo "                      Cleaning the temporaries and outputs                        "
echo "=================================================================================="

make clean
rm -r ${OUTPUT_DIR}

echo "=================================================================================="
echo "                             Compiling test compiler                              "
echo "=================================================================================="

make all
mkdir -p ${OUTPUT_DIR}

COMPILER=bin/c_compiler

# Formatting
FORMAT_PASS="%-5s %-40s ${GREEN} %-20s ${NC} %-5s \n"
FORMAT_FAIL="%-5s %-40s ${RED} %-20s ${NC} %-5s \n"

# Test tracker
PASS_TRACKER=0
FAIL_TRACKER=0

for TEST_FOLDER in ${TEST_DIR}/* ; do

echo "=================================================================================="
echo "Testing folder:" ${TEST_FOLDER}
echo "=================================================================================="

    for i in ${TEST_FOLDER}/*_driver.c ; do

        # Extract variables
        INDEX=$((INDEX+1));  
        TEST=$(basename $i _driver.c)
        TEST_FILE=${TEST_FOLDER}/${TEST}.c

        # ${COMPILER} < ${TEST_FILE} > ${OUTPUT_DIR}/${TEST}.s

        # Compile test function with compiler under test 
        ${COMPILER} -S ${TEST_FILE} -o ${OUTPUT_DIR}/${TEST}.s

        # Compile driver with normal GCC
        mips-linux-gnu-gcc -mfp32 -o ${OUTPUT_DIR}/${TEST}.o -c ${OUTPUT_DIR}/${TEST}.s

        # Link driver object and assembly into executable
        mips-linux-gnu-gcc -mfp32 -static -o ${OUTPUT_DIR}/${TEST}_qemu ${OUTPUT_DIR}/${TEST}.o ${TEST_FOLDER}/${TEST}_driver.c

        # Run the actual executable
        qemu-mips ${OUTPUT_DIR}/${TEST}_qemu

        # https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
        # https://stackoverflow.com/questions/26130909/bash-output-column-formatting

        RETURN=$?
        if [[ $RETURN -ne 0 ]]; then
            RESULT="FAIL"
            FAIL_TRACKER=$((FAIL_TRACKER+1))
            printf  "${FORMAT_FAIL}" $INDEX $TEST $RESULT $RETURN    
        else
            RESULT="PASS"
            PASS_TRACKER=$((PASS_TRACKER+1))
            printf  "${FORMAT_PASS}" $INDEX $TEST $RESULT $RETURN    
        fi

    done

done

echo "=================================================================================="
echo "Testbench report:" ${TEST_FOLDER}
echo "=================================================================================="
echo "Total tests:" ${INDEX}
echo "Passed tests:" ${PASS_TRACKER}
echo "Failed tests:" ${FAIL_TRACKER}





