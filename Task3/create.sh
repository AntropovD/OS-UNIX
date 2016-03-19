#!/bin/bash

count=$RANDOM
let "count %= 100"
echo $count

for i in `seq 1 $count`
do
	dd if=/dev/zero of=sparce.img bs=1 count=0 seek=30M > /dev/null 2>&1
	cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1 > sparce.img
done

