#!/bin/bash

dataset_prefix="../dataset/expr/p_uniform_"
dataset_suffix="_16000000.txt"

for i  in 2 4 6 8 10;
do
    echo $i
    dataset=$dataset_prefix$i$dataset_suffix
    ./global_size_expr $1 $i $dataset
done
