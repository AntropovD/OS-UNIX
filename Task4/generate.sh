#!/bin/bash

N=$RANDOM
let "N %= 100"

rm passwd
for i in `seq 1 $N`
do
    echo user$i `cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1` >> passwd    
done
