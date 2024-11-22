from client.models import LLM4PP_Problem, LLM4PP_Submission
from client.pareval_client import ParEvalDriver
from vllm import LLM, SamplingParams
import json

def generate_code_opt_prompt_code_alpaca(src_code : str, fast_code : str ="") -> str:
    prompt_template = (
                "Below is an instruction that describes a task, paired with an input that provides further context. "
                "Write a response that appropriately completes the request.\n\n"
                "### Instruction:\n{instruction}\n\n### Input:\n{input}\n\n### Response:\n{response}"
            )

    instruction = "Below is a program. Optimize the program and provide a more efficient version."
    prompt = prompt_template.format_map({"instruction" : instruction, "input" : src_code, "response": fast_code})
    return prompt

driver = ParEvalDriver()

MODEL_PATH = ""
llm = LLM(model=MODEL_PATH)
sampling_params = SamplingParams(temperature=0.2, top_p=0.95, max_tokens=4096)

for problem in driver:
    problem : LLM4PP_Problem

    prompt = generate_code_opt_prompt_code_alpaca(problem.source_code)
    output = llm.generate(prompt, sampling_params)
    optimized_code = output[0].outputs[0].text

    submission = LLM4PP_Submission(problem=problem,
                                   submitted_code=optimized_code)

    try:
        response = driver.submit(submission)
    except Exception as e:
        print(f"skipping problem due to exception: {e}")
        print("--- ParEval driver stdout ---")
        print(response.stdout)

driver.save_all_responses("./tmp-pareval-results.json")
driver.evaluate()

