# SkylineSketch

## Introduction

Skyline queries are essential and common in mining data with multi-dimension attributes. In the era of big data, finding skyline in real time in high speed data streams becomes more and more important. To the best of our knowledge, no existing work can achieve fast update speed because they store all items in the data streams to keep 100% accuracy. We propose a new sketch, a dynamic probabilistic algorithm, to accurately detect skyline in data streams, namely SkylineSketch. Our first technique is called potential evaluation: it only maintains items that have high probability to become a skyline point, improving memory efficiency. Our second technique is called representative team: it reduces comparison operations in evaluating the potential of each incoming item, raising update speed. Experimental results show that our sketch’s throughput is 9.87 million insertions per second (MIPS) and query time is 0.345 millisecond (ms) on average.

## About the source code

* `dataset`：contains the source code for generating synthetic datasets and our generated datasets that are used in the evaluations.
* `src`: contians the source code for the implementation of SkylineSketch and the strawman solution.

## About the synthetic datasets

Please refer to the README in `dataset`.

## How to run evaluations

* change directory to `src` and compile the source code

  ```bash
  cd src; make
  ```

* In order to run evaluations with the provided datasets, we provide scripts to automatically run the evaluations and print the results in the standard output.

  * Effects of potential zone bucket size $\theta$​ (Figure 3):

    ```bash
    ./bucket_size_expr.sh 4
    ./bucket_size_expr.sh 8
    ./bucket_size_expr.sh 10
    ./bucket_size_expr.sh 12
    ```

  * Effects of representative bucket size $\lambda$ (Figure 4):

    ```bash
    ./global_size_expr.sh 8
    ./global_size_expr.sh 12
    ./global_size_expr.sh 16
    ./global_size_expr.sh 20
    ```

  * Experiments on small memory (Figure 5):

    ```bash
    ./performance.sh 5
    ```

  * Experiments on median memory (Figure 6):

    ```shell
    ./performance.sh 10
    ```

  * Experiments on large memory (Figure 7):

    ```bash
    ./performance.sh 20
    ```

  * Experiments on the co-related dataset (Figure 8):

    ```shell
    ./cor.sh 10
    ```

  * Experiments on the anti-related dataset (Figure 9):

    ```shell
    ./anti.sh 10
    ```

    


