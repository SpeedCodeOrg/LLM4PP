# LLM4PP @ PPoPP 2025

Welcome to the PPoPP Contest on LLM-based Parallel Programming (LLM4PP @ PPoPP 2025)!

* [Introduction](#introduction)
* [Objective](#objective)
* [Problem Definition](#problem-definition)
* [Starting Toolkit](#starting-toolkit)
* [Submission Guidelines](#submission-guidelines)
* [References](#references)
* [Contact](#contact)
  
<!--Starting Toolkit for the LLM4PP competition, modified from the starting toolkit from [LLM4HWDesign](https://nvlabs.github.io/LLM4HWDesign/problem.html) is [here](https://github.com/GATECH-EIC/LLM4HWDesign_Starting_Toolkit).-->

## Introduction

Large Language Models (LLMs) have demonstrated remarkable capabilities in generating high-quality content from natural language prompts, sparking growing interest in their application to parallel programming. 

<!--
Despite the significant potential and community excitement, current state-of-the-art pretrained LLMs, such as OpenAI's GPT-4 [1], still struggle to produce practical parallel code without extensive human intervention in their original forms. 
In parallel code generation, for example, these models tend to either generate non-compilable or non-functional code, necessitating human correction, or produce overly simplistic or inefficient parallel implementations. 
This issue can primarily stem from the LLMs' limited exposure to parallel code data during pretraining. A pioneering attempt in HPC-Coder [2] demonstrates that using a large-scale parallel code dataset can improve LLMs' PP abilities. 
However, models trained with publicly available datasets are still far behind human experts. Thus, developing open-source, high-quality, PP-specific code datasets is essential for unlocking the full potential of LLM-based PP.
This year's contest seeks to address this challenge by asking you to help build a large-scale, high-quality parallel code generation dataset. 
By open-sourcing this dataset, we aim to establish critical infrastructure for advancing LLM-based parallel programming workflows. 
Participants will be invited to author a short paper summarizing our efforts, insights, and lessons learned, thereby paving the way for future initiatives.

Unfortunately, the development of LLM for PP is severely hindered by the scarcity of high-quality, publicly accessible parallel code datasets. 
Specifically, the lack of adequate datasets prevents effective fine-tuning of LLMs, a critical method for equipping them with PP domain knowledge and mitigating their limited exposure to PP-specific data during pretraining. 
This shortage thus significantly impedes progress in LLM-based parallel code generation.
-->

## Objective
The goal of this contest is to 
(1) explore new prompting strategies or develop new workflows.
(2) develop new LLM fine-tuning strategies.
(3) collect or generate new code samples.

<!--enrich the current parallel code dataset to a large-scale, high-quality open-source dataset, facilitating the development of more effective LLM-based parallel programming through fine-tuning. 
Participants are asked to (1) collect or generate parallel code samples and (2) enhance the dataset quality through data cleaning and label generation techniques. 
Participants' contributions will be evaluated based on the improvement their data brings to the fine-tuned LLM.-->

## Problem Definition
The task is code optimization. Given a piece of source code in C++, you are asked to optimize the code to make it run faster. We want to use LLMs to aid us in this task. The task can be tackled in two different ways.
* Use a closed-source LLM such as one from OpenAI, and obtain the optimized code using the provided APIs.
* Use an open-source LLM such as CodeLlama or Deepseek-Coder, and obtain the optimized code by running inference locally.

## Starting Toolkit
To get started, participants are provided the starting toolkit, which is this github repository. It contains two directories.
(1) `model_eval` which includes code for evaluating your submission on the ParEval benchmark.
(2) `model_finetune` which includes sample code for finetuning an open-source LLM.

<!--
and (5) the deduplication codebase we will use to duplicate the repeated data samples. 
Participants are expected to just replace the example submission dataset with their own collected datasets and get the corresponding metric from the starting toolkit to further improve their datasets during Phase I.
-->

## Using a Closed-Source LLM
See this [README](model_eval/README.md) for how to run a sample evaluation script on closed-source LLMs from OpenAI.

## Using an Open-Source LLM
One strategy to improve model performance on the code optimization task is to finetune a model on a code optimization dataset. We provide sample code for finetuning and evaluation described below.

### Finetuning
See this [README](model_finetune/README.md) for how to run a finetuning script on open-source LLMs.

### Model Evaluation
See this [README](model_eval/README.md) for how to run a sample evaluation script on closed-source LLMs from OpenAI.

## Submission Guidelines
For each phase, submit a python file similar to `model_eval/evaluation.py` which is described in more detail [README](model_eval/README.md). If using open-source models, please also upload them to HuggingFace and make them public.

For the first phase of using closed-source LLMs, please make sure that the costs incurred by running the script are reasonable. Specifics will be released at a later date.

For the second phase of using open-source LLMs, please make sure that the code can be run on a single reasonably-sized GPU. Specifics will be released at a later date.

## References
1. [Gpt-4 technical report](https://arxiv.org/abs/2303.08774)
2. [HPC-Coder](https://arxiv.org/html/2306.17281v2): Modeling Parallel Programs using Large Language Models, ISC 2024
3. [Can Large Language Models Write Parallel Code?](https://arxiv.org/pdf/2401.12554.pdf) [[ParEval](https://github.com/parallelcodefoundry/ParEval)], HPDC 2024
4. [Learning Performance Improving Code-Edits](https://arxiv.org/abs/2302.07867). [[PIE](https://pie4perf.com/)], ICLR 2024

## Contact
* Xuhao Chen, cxh@mit.edu 
* Ryan Deng, ryandeng@mit.edu
* Tim Kaler, tfk@mit.edu
