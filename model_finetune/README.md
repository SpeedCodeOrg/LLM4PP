# Finetuning

## Required Libraries
To install all of the necessary libraries, first create a conda environment:
```
conda create -n sft_code_opt python=3.10
```

Then, install all of the necessary libraries by running:
```
pip install -r requirements.txt
```

The versions of these libraries have been tested to work and may not be compatible with the ones installed by the libraries used in model evaluation. While it may be possible to use the same environment to do both fine-tuning and evaluation, it is not recommended.

## Dataset
The sample finetuning code assumes the dataset follows the format described [here](https://huggingface.co/datasets/speedcode/LLM4PP_dataset). We also provide a sample dataset based on Leetcode problems at that link.

## Running Supervised Fine-Tuning
The sample code runs supervised fine-tuning (SFT). It is inspired by code from [PIE](https://github.com/LearningOpt/pie).

To run the finetuning code for code optimization, an example command is:

```
accelerate launch src/sft_code_opt.py \
    --model_name "deepseek-ai/deepseek-coder-6.7b-base" \
    --dataset_path $DATASET_PATH \
    --output_dir $OUTPUT_DIR \
    --num_train_epochs 5 \
    --per_device_train_batch_size 8 \
    --per_device_eval_batch_size 1 \
    --evaluation_strategy "no" \
    --save_strategy "no" \
    --learning_rate 1e-5 \
    --logging_steps 1 \
    --report_to "wandb" \
    --bf16 True \
```
