# Finetuning a Model

## Code Optimization

### Required Libraries
To install all of the necessary libraries for finetuning a model to do code optimization, create a conda environment:
```
conda create -n sft_code_opt python=3.10
```

Then, install all of the necessary libraries
```
pip install -r requirements.txt
```

To run the finetuning code, an example command is:
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

