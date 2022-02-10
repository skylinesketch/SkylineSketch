#!/bin/bash

dataset_prefix="../dataset/expr/p_uniform_"
dataset_suffix="_16000000.txt"

memory=$1

for i in 2 4 6 8 10;
do
    echo $i
    dataset=$dataset_prefix$i$dataset_suffix
    ./performance $memory $i $dataset
done
