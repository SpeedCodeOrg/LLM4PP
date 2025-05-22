from client.models import LLM4PP_Problem, LLM4PP_Submission
from client.pareval_client import ParEvalDriver
from client.polybench_client import PolyBenchDriver
from fastcoder.chatapi import MessageHistory, ChatAPI

from openai import OpenAI
import json
import hydra
import jsonlines
import logging
import os
from omegaconf import DictConfig, OmegaConf
from strategies.utils import *

def generate_code_opt_prompt_code(src_code : str, language : str ="c++", additional_package: str="") -> str:
    prompt_template = (
                "{instruction}\n\n// Code:\n{input}\n\n"
            )

    instruction = f"You will be given a piece of code written in {language}. Your task is to rewrite it in the same language to improve its performance (i.e., execution time). {additional_package} Do not change the input/output behaviors of the function. Include the generated code between ```{language} and ```."
    prompt = prompt_template.format_map({"instruction" : instruction, "input" : src_code})
    return prompt

def run(cfg: DictConfig) -> None:
    benchmark = cfg.benchmark
    mode = cfg.mode
    
    logging.config.dictConfig(cfg.logging)
    # A logger for this file
    logger = logging.getLogger("main_logger")

    if benchmark == "ParEval":
        driver = ParEvalDriver(mode)
        evaldriver = ParEvalDriver(mode)
    elif benchmark == "PolyBench":
        driver = PolyBenchDriver(mode)
        evaldriver = PolyBenchDriver(mode)
    else:
        logger.info("Unknown Benchmark, program exits.")
        exit(0)

    if mode != "serial": # assume mode is a parallel package can be integrated in c++ only
        additional_package = f"You should use {mode} to parallelize the code."
    else: #serial
        additional_package = ""

    #model = "gpt-4o"
    model = cfg.model
    chatAPI = ChatAPI()

    savename = f"openai-{model}-{benchmark}_mode-{mode}"
    os.makedirs("evaluator_results", exist_ok=True)
    evaluator_save_path = f"evaluator_results/{savename}.jsonl"
    logger.info(f"Evaluating `{model}`")
    logger.info(f"Results will be saved at `{evaluator_save_path}`")
    
    for problem in enumerate_resume(driver, evaluator_save_path):
        problem : LLM4PP_Problem
        logger.info(problem.problem_id)
        messages = MessageHistory()
        
        prompt = generate_code_opt_prompt_code(problem.source_code, additional_package=additional_package)
        logger.debug("Prompt: ")
        logger.debug(prompt)
        
        messages.add_message("user", prompt)
        
        response = chatAPI.get_response(model, messages, json_format=False)
        optimized_code = clean_output(response)

        logger.debug("Optimized Code: ")
        logger.debug(optimized_code)
        
        if optimized_code == "":
            logger.info("No code block found.")
        submission = LLM4PP_Submission(problem=problem,
                                    submitted_code=optimized_code)
        try:
            response = driver.submit(submission)
        except Exception as e:
            logger.info(f"skipping problem due to exception: {e}")
            logger.info("--- ParEval driver stdout ---")
            logger.info(response.stdout)
        #logger.info(response.type)
        driver.save_one_response_jsonl(evaluator_save_path, [response.dict()], append=True)
        #log, tag, speedup = pareval_process_execution_feedback(log=response.stdout)
    # driver.save_all_responses(f"./evaluator_results/{savename}.json")
    driver.evaluate()

    logger.info(f"Completed! Results are saved at `{evaluator_save_path}`")