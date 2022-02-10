# Dataset

## About the source code

* `c`: contains the source C++ codes for generating anti-related, co-related distributions.
* `python`: contains the source python codes for generating synthetic datasets.
* `expr`: contains the synthetic datasets that are used in the evaluations.

## How to generate datasets

We have provided all datasets that we used in the evaluations in `expr`.

Next, we describe how we generate synthetic datasets.

* First, generate co-related and anti-related distributions

  * ```shell
    cd c; make
    ```

  * generate co-related distributions with 1000000 items:

    ```shell
    ./main -t 1 -n 1000000 -a {attribute_num} -o {output_file}
    ```

    generate anti-related distributions with 1000000 items:

    ```shell
    ./main -t 2 -n 1000000 -a {attribute_num} -o {output_file}
    ```

* Second, generate three types of synthetic datasets. If the parameter of the datasets should be adjusted, directly change the parameter in the python file.

  * independent datasets:

    ```shell
    python3 p_uniform.py
    ```

  * co-related datasets:

    ```shell
    python3 p_corr.py
    ```

  * anti-related datasets:

    ```shell
    python3 p_anti.py
    ```

    

