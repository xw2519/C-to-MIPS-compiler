#!/bin/bash

mkdir -p working

echo "========================================"
echo " Cleaning the temporaries and outputs"
make clean


echo "========================================="

bin/compiler compiler_tests/functions/call_constant_external.c \
    > working/output/compiled.txt
