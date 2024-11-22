# Generate

This subdirectory contains scripts for generating the LLM outputs for ParEval.
The main script is `generate.py`. It can be run as follows.

```sh
usage: generate.py [-h] --prompts PROMPTS --model MODEL --output OUTPUT [--restart] [--cache CACHE] [--restore_from RESTORE_FROM] [--max_new_tokens MAX_NEW_TOKENS]
                   [--num_samples_per_prompt NUM_SAMPLES_PER_PROMPT] [--temperature TEMPERATURE] [--top_p TOP_P] [--do_sample] [--batch_size BATCH_SIZE] [--prompted]

Generate code

optional arguments:
  -h, --help            show this help message and exit
  --prompts PROMPTS     Path to the prompt JSON file
  --model MODEL         Path to the language model
  --output OUTPUT       Path to the output JSON file
  --restart             Restart generation from scratch (default: False)
  --cache CACHE         JSONL file to cache intermediate results in. Will be restored from if it already exists and --restart is not specified
  --restore_from RESTORE_FROM
                        JSON file to restore old results from. Will be restored from if it already exists and --restart is not specified. Is different from --cache in that it is a JSON
                        file, not a JSONL file, and it is only used to restore old results where the prompt is equivalent. Cached results are prioritized over restored results.
  --max_new_tokens MAX_NEW_TOKENS
                        Maximum number of new tokens to generate (default: 1024)
  --num_samples_per_prompt NUM_SAMPLES_PER_PROMPT
                        Number of code samples to generate (default: 50)
  --temperature TEMPERATURE
                        Temperature for controlling randomness (default: 0.2)
  --top_p TOP_P         Top p value for nucleus sampling (default: 0.95)
  --do_sample           Enable sampling (default: False)
  --batch_size BATCH_SIZE
                        Batch size for generation (default: 8)
  --prompted            Use prompted generation. See StarCoder paper (default: False)
  --code_opt            Set to True if doing the code optimization task and False for the code completion task.
```

To get outputs in a reasonable amount of time you will most likely need to run
this script on a system with a GPU available. There are the `generate-openai.py`
and `generate-gemini.py` scripts for generating outputs from the respective
APIs. These will require you to set the API keys in environment variables or
pass them as arguments. Use `--help` to see the arguments available.

The `--model` argument in `generate.py` can either be a local model path or the
handle of a HuggingFace model. The cache and restart arguments can be used to
restart from existing outputs. The `--prompted` flag will append _solution_
comments to the front of the prompt as in the StarCoder paper. You likely want
this turned on as it almost always improves the results.

## Adding New LLMs

Since a number of the LLMs have different inference settings and prompt formats,
we utilize an inference configuration abstraction to set these parameters for
different models. These are defined in `utils.py`. To add support for a new LLM
you will need to define an inference config for that LLM in `utils.py`. The
existing examples in that file should suffice as examples.

We have an example called `SpeedcodeConfig` as an example we used when doing inference.

## Translate Tests

Each of the generate scripts has a translation equivalent. These are used for
the translation tasks.

## Prompts
The prompts are stored in the prompts directory. There are three prompts to use for the competition.
The prompts for code completion are taken directly from ParEval and cannot be changed. 
The prompts for code optimization are written by us and thus there is some flexibility in the prompts used for that task.

* `serial-generation-prompts.json` is used for Serial code completion.
* `omp-generation-prompts.json` is used for OMP code completion.
* `code_opt.json` is used for code optimization.
