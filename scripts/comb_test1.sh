#!/bin/bash

NAME=$0

echo "Started the $NAME...." > $NAME.log
echo `pwd` >> $NAME.log
while :
do
        date >> $NAME.log
        sleep 2
done

rm $NAME.log