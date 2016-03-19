#!/bin/bash

dd if=/dev/zero of=sparce.img bs=1 count=0 seek=64M &>/dev/null 
mkfs.reiserfs -f -q sparce.img
mkdir folder
sudo mount -o loop sparce.img folder

for i in {1..1000}
do
	touch folder/tmp$i		
	head -c 1000 /dev/urandom >folder/tmp$i
done

sudo umount folder
rmdir folder
