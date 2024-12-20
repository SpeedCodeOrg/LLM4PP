import copy
import jsonlines
import re
import torch
import transformers

from accelerate import Accelerator, PartialState
from dataclasses import dataclass, field
from datasets import concatenate_datasets, Dataset, load_dataset
from transformers import (
    AutoTokenizer,
    AutoModelForCausalLM,
    DataCollatorForSeq2Seq,
    GenerationConfig,
    HfArgumentParser,
    Trainer,
    TrainingArguments,
)

from trl import SFTTrainer
from typing import cast, Sequence, Dict, Optional

@dataclass
class ModelArguments:
    model_name: str

@dataclass
class DataArguments:
    dataset_path: str = field(metadata={"help": "Path to the training data."})

def generate_code_opt_prompt_code_alpaca(src_code, fast_code=""):
    prompt_template = (
                "Below is an instruction that describes a task, paired with an input that provides further context. "
                "Write a response that appropriately completes the request.\n\n"
                "### Instruction:\n{instruction}\n\n### Input:\n{input}\n\n### Response:\n{response}"
            )

    instruction = "Below is a program. Optimize the program and provide a more efficient version."
    prompt = prompt_template.format_map({"instruction" : instruction, "input" : src_code, "response": fast_code})
    return prompt

def tokenize(prompt, tokenizer):
    cutoff_len = 4096
    result = tokenizer(
        prompt,
        truncation=True,
        max_length=cutoff_len,
        padding=False,
        return_tensors=None,
    )

    if result["input_ids"][-1] != tokenizer.eos_token_id and len(result["input_ids"]) < cutoff_len:
        result["input_ids"].append(tokenizer.eos_token_id)
        result["attention_mask"].append(1)

    result["labels"] = result["input_ids"].copy()

    return result

def cleanup_code(code_string):
    # Split the string into lines and filter out "#pragma once" and "#include solution.hpp"
    # which are not really relevant to the program.
    cleaned_code = '\n'.join(
        line for line in code_string.splitlines() if "solution.hpp" not in line and "#pragma once" not in line
    )

    cleaned_code = cleaned_code.lstrip()

    return cleaned_code

# this implementation is taken from https://github.com/LearningOpt/pie/blob/main/finetuning/finetune.py
def train_tokenize_function(data, tokenizer, add_eos_token=True, clean_code=False, train_on_inputs=False):
    src_code = cleanup_code(data["src_code"])
    tgt_code = cleanup_code(data["tgt_code"])

    full_prompt = generate_code_opt_prompt_code_alpaca(src_code, tgt_code)

    tokenized_full_prompt = tokenize(full_prompt, tokenizer)

    # this is training on completions only
    if not train_on_inputs:
        user_prompt = generate_code_opt_prompt_code_alpaca(src_code)
        tokenized_user_prompt = tokenize(user_prompt, tokenizer)

        user_prompt_len = len(tokenized_user_prompt["input_ids"])
        if add_eos_token:
            user_prompt_len -= 1

        tokenized_full_prompt["labels"] = [
            -100
        ] * user_prompt_len + tokenized_full_prompt["labels"][
            user_prompt_len:
        ]  # could be sped up, probably

    return tokenized_full_prompt

def get_dataset(data_path: str, tokenizer: transformers.PreTrainedTokenizer):
    train_dataset = load_dataset(data_path, split="train")

    train_dataset = train_dataset.map(
        train_tokenize_function,
        # batched=true,
        # batch_size=512,
        # num_proc=1,
        remove_columns=train_dataset.column_names,
        desc="running tokenizer on train dataset",
        fn_kwargs={"tokenizer": tokenizer, "clean_code": True},
    )

    # val dataset not used
    val_dataset = copy.deepcopy(train_dataset)

    return train_dataset, val_dataset

def finetune_model(model_name: str, dataset_path: str, output_dir: str, training_args):
    model = AutoModelForCausalLM.from_pretrained(
            model_name,
            use_cache=False,
            # attn_implementation="flash_attention_2",
        )

    model.config.use_cache = False

    tokenizer = AutoTokenizer.from_pretrained(model_name, trust_remote_code=True)

    tokenizer.pad_token_id = 0
    tokenizer.padding_side = "left"

    train_dataset, val_dataset = get_dataset(dataset_path, tokenizer)

    trainer = SFTTrainer(model=model, tokenizer=tokenizer, train_dataset=train_dataset, eval_dataset=val_dataset, args=training_args,
                         data_collator=DataCollatorForSeq2Seq(tokenizer, pad_to_multiple_of=8, return_tensors="pt", padding=True),
    )

    if trainer.is_fsdp_enabled:
        trainer.accelerator.state.fsdp_plugin.set_state_dict_type("FULL_STATE_DICT")

    trainer.train()
    trainer.save_model(output_dir=output_dir)

def main():
    parser = transformers.HfArgumentParser((ModelArguments, DataArguments, TrainingArguments))
    model_args, data_args, training_args = parser.parse_args_into_dataclasses()
    finetune_model(model_args.model_name, data_args.dataset_path, training_args.output_dir, training_args)

if __name__ == "__main__":
    main()

