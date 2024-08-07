#!/bin/bash

START=11810
END=11932

for (( r=$START; r<=$END; r=r+20 ))
do
    echo $r
    source make-list.sh $r
    source run-wfdumper.sh $r
    root -l "check_baselines.C(${r})"
done
 
echo "Boom!"
