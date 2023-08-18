#! /usr/bin/bash

for i in `seq 1 $1`
do
    ./mhultistone -n=$2 -t=10.0 -j=12
done

