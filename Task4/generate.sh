#!/bin/bash

rm passwd
for i in `seq 1 30`
do
    echo user_$i `cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1` >> passwd    
done
