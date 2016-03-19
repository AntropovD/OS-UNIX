#!/bin/bash

./create.sh
gcc main.c -o main
gzip -c sparce.img | gzip -d -c | ./main output
