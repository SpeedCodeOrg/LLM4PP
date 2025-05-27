# Introduction

This repo contains a `c++` code optimization benchmark modified from the [ParEval](https://github.com/parallelcodefoundry/ParEval) and [PolyBench](https://github.com/MatthiasJReisinger/PolyBenchC-4.2.1/tree/master) benchmarks. The modification provides a baseline code for each problem and the task is to write a version that runs faster than the baseline. Because of the uniformity of the two benchmarks, we combine them together, which can be referenced as the ParEval-PolyBench-Code-Opt benchmark. One may use the ParEval part or the PolyBench part separately (see instructions below). Currently, this benchmark supports only two programming modes: serial and OpenMP.

# Installation

## Setup Environment
Create a conda environment. (We have tested on an environment with `python = 3.11`.)
* Run `conda create -n code-opt-bench python=3.11 -y`.
* Then `conda activate code-opt-bench`.
* Install a `C++` compiler using the command: `conda install -c conda-forge cxx-compiler`. The C++ compiler must support C++20.
* Run `pip install -r requirements.txt`.

## Setup Benchmark
To setup the benchmark, execute the following command:

```
cd ParEval-PolyBench-Code-Opt/drivers/cpp && make
```
This will compile the baseline codes for benchmarking.

# Run Evaluation

`evaluate.py` is the driver for evaluating optimized codes. Here is an example to use gpt-4o-mini to generate optimized codes for the ParEval benchmark on the serial mode:

```bash
python evaluate.py strategy@_global_=evaluation_openai benchmark=ParEval mode=serial logging.loggers.main_logger.level=INFO model=gpt-4o-mini
```

We use [`hydra`](https://hydra.cc) to control different configurations, elaborated in the following.


## Choose a Model
We provide scripts to evaluate on different models. To evaluate on OpenAI models, first run `export OPENAI_API_KEY=<your api key here>`, then use the following configuration to evaluate on `gpt-4o-mini`:

```
strategy@_global_=evaluation_openai model=gpt-4o-mini
```

The script evaluating on openai models uses `ChatAPI` class defined in `utils/chatapi.py` to manage OpenAI model inferences. Currently, three models are available: `gpt-4o-mini`, `gpt-4o` and `gpt-3.5-turbo`.


Similarly, to evaluate on open source models (e.g. `Qwen/Qwen2.5-Coder-14B-Instruct`) on huggingface using vLLM, first run `export HF_HOME=<YOUR/PATH>` to set up the directory where the models will be downloaded, then use the following configuration:

```
strategy@_global_=evaluation_vllm model=MODEL_PATH
```

`MODEL_PATH` should be any huggingface model path, e.g. `Qwen/Qwen2.5-Coder-14B-Instruct`, and [here](https://huggingface.co/Qwen/Qwen2.5-Coder-14B-Instruct) is the corresponding huggingface model card.


## Choose a Benchmark
To run ParEval, use the following configuration:

```
benchmark=ParEval
```

Similarly, for PolyBench:

```
benchmark=PolyBench
```

## Choose a Mode
To evaluate on the serial mode, use the following configuration:

```
mode=serial
```

Similarly, for the OpenMP mode:

```
mode=OpenMP
```

## Choose a Logging Option
To only see default information in `stdout`, use the following configuration:

```
logging.loggers.main_logger.level=INFO
```

Similarly, to see addtional information like input prompts and optimized codes, do:

```
logging.loggers.main_logger.level=DEBUG
```

## Notes

Each problem has a different input size. Check `ParEval-PolyBench-Code-Opt/drivers/problem-sizes.json` for detailed input sizes.

The settings of timeout and OpenMP threads are located in `client/pareval_client.py` (ParEval) and `client/polybench_client.py` (PolyBench). We fix timeout to 300 seconds and 8 threads for OpenMP.


# Evaluation Metrics

The benchmarking results under different metrics are directly printed in `stdout`. The optimized codes are saved in the `evaluator_results` folder.

The metrics include:
* **compile**: percentage of compilable codes.
* **correctness**: percentage of correct codes.
* **geomean speedup**: geometric mean of speedup.
* **mean speedup**: mean of speedup.
* **speedup>=2**: percentage of optimized codes with speedup greater than or equal to 2.

# Custom Evaluation Implementation

You may implement your own evaluation strategy based on this benchmark.

We include two evaluation examples: `strategies/evaluation_openai.py` and `strategies/evaluation_vllm.py`, both of which contains the `run` function. 
Note that the user should still run `evaluate.py` as the main driver, which will use `hydra` to call the `run` functions respectively. Also, check `config/strategy` for configuration yaml files.

To add a custom strategy, the name of your configuration under `config/strategy` has to be aligned with the `.py` file in the `strategy` folder. For instance, `config/strategy/evaluation_vllm.yaml` and `strategies/evaluation_vllm.py` go hand in hand. Each custom yaml file under `config/strategy` has to include `_target_: strategies.<your-strategy>.run`. For instance, inside `config/strategy/evaluation_vllm.yaml`, the `_target_` configuration `_target_: strategies.evaluation_vllm.run` is included.


# Additional Information

This benchmark is adapted from `ParEval` and `PolyBench` for code optimization and speedup evaluation. The original `ParEval` is located at https://github.com/parallelcodefoundry/ParEval and the original `PolyBench` is located at https://github.com/MatthiasJReisinger/PolyBenchC-4.2.1/tree/master.

If you have any questions, please do not hesitate to contact us.
