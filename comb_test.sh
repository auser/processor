#!/bin/bash

# Program to test the comb_process

echo "Started the comb_process...." > comb_process.log
echo `pwd` >> comb_process.log
while :
do
        date >> comb_process.log
        sleep 2
done