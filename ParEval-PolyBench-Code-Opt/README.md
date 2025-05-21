# ParEval

[![HPDC 2024](https://img.shields.io/badge/Paper-HPDC'24-e87053.svg?style=flat)](https://pssg.cs.umd.edu/assets/papers/2024-06-pareval-hpdc.pdf)&nbsp;[![arXiv](https://img.shields.io/badge/arXiv-2401.12554-b31b1b.svg)](https://arxiv.org/abs/2401.12554)&nbsp;[![GitHub license](https://badgen.net/github/license/parallelcodefoundry/ParEval)](https://github.com/parallelcodefoundry/ParEval/blob/develop/LICENSE)

This repo contains a fork of the Parallel Code Evaluation (ParEval) Benchmark for
evaluating the ability of Large Language Models to write parallel code. We are using a modified version of the benchmark focused on code optimization.

The original repo is found [here](https://github.com/parallelcodefoundry/ParEval).

## Overview

The organization of the repo is as follows.

- `prompts/` -- the prompts in ParEval alongside some utility scripts
- `generate/` -- scripts for generating LLM outputs
- `drivers/` -- scripts to evaluate LLM outputs
- `analysis/` -- scripts to analyze driver results and compute metrics

Each subdirectory has further documentation on its contents. The general
workflow is to use `generate/generate.py` to generate LLM outputs, run
`drivers/run-all.py` to evaluate outputs, and `analysis/metrics.py` to
post-process the results.

<!---

## Setup and Installation

A couple core systems software are assumed to be installed: Python >=3.7, a C++
compiler that supports C++20 and OpenMP, Make, CMake, and an MPI implementation.

To install a C++ compiler via conda, run:
```
conda install -c conda-forge cxx-compiler
```

You need to install the Python dependencies. `requirements.txt` has
the set of dependencies pinned at the version they were tested with. Other
versions may also work. Note that some of these are only required for parts of
the pipeline i.e. PyTorch and Transformers are only needed for generating LLM
outputs.

```sh
pip install -r requirements.txt
```
-->

## Citing ParEval

```
@misc{nichols2024large,
      title={Can Large Language Models Write Parallel Code?}, 
      author={Daniel Nichols and Joshua H. Davis and Zhaojun Xie and 
              Arjun Rajaram and Abhinav Bhatele},
      year={2024},
      publisher = {Association for Computing Machinery},
      address = {New York, NY, USA},
      booktitle = {Proceedings of the 33rd International Symposium on High-Performance Parallel and Distributed Computing},
      series = {HPDC '24}
}
```

## License

ParEval is distributed under the terms of the [MIT license](/LICENSE).
