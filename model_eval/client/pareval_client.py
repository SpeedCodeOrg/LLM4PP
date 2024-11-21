import json
import os
import subprocess
import shlex
import tempfile

from client.driver import LLM4PP_Driver, ProblemIterator, ProblemLoader, SubmissionRunner
from client.models import BenchmarkDescription, LLM4PP_Problem, LLM4PP_Submission, LLM4PP_SubmissionResponse

def create_generation_dict(problem_category: str, problem_unique_id: str, optimized_code: str, prompt: str = ""):
    d = {}
    d["outputs"] = []
    d["outputs"].append(optimized_code)
    d["name"] = problem_unique_id
    # extract problem type. Ex: 17_graph_highest_degree -> graph
    d["problem_type"] = problem_category
    d["parallelism_model"] = "omp"
    d["language"] = "cpp"
    d["prompt"] = prompt

    return [d]

def run_driver(gen_file: str, problem_sizes_file: str = "problem-sizes.json", run_timeout: int = 300, launch_configs: str = "launch-configs-speedcode.json") -> str:
    # delete set to False as need this file in another function
    with tempfile.NamedTemporaryFile(suffix=".json") as output_f:
        args = f"python run-all.py {gen_file} -o {output_f.name} --problem-sizes {problem_sizes_file} --yes-to-all --run-timeout {run_timeout} --launch-configs {launch_configs} --code_opt True"

        subprocess_args = shlex.split(args)
        proc = subprocess.run(subprocess_args, cwd="ParEval/drivers", capture_output=True, text=True)
        output, error = proc.stdout, proc.stderr
        if proc.returncode != 0:
            print(f"output: {output}")
            print(f"error: {error}")

        with open(output_f.name) as driver_f:
            driver_info_dict = json.load(driver_f)

        return driver_info_dict, output, error

def pareval_submit(problem_category: str, problem_unique_id: str, optimized_code: str):
    gen_dict = create_generation_dict(problem_category, problem_unique_id, optimized_code)

    with tempfile.NamedTemporaryFile(suffix=".json") as gen_f:
        with open(gen_f.name, "w") as f:
            json.dump(gen_dict, f, indent=4)

        driver_info_dict, driver_stdout, driver_stderr = run_driver(gen_f.name)

        output_dict = {}

        # assume length of outputs is 1 as only test 1 output at a time
        run_info = driver_info_dict[0]["outputs"][0]

        output_dict["did_build"] = bool(run_info["did_build"])
        output_dict["did_run"] = bool(run_info["did_all_run"])
        output_dict["is_valid"] = bool(run_info["are_all_valid"])

        baseline_runtime = run_info["best_sequential_runtime"]
        if baseline_runtime is None:
            baseline_runtime = 0.0

        output_dict["baseline_runtime"] = baseline_runtime
        # code did not compile or code did not run without errors
        if run_info["runs"] is None:
            output_dict["optimized_runtime"] = baseline_runtime
        else:
            output_dict["optimized_runtime"] = float(run_info["runs"][0]["runtime"])

        output_dict["stdout"] = driver_stdout
        output_dict["stderr"] = driver_stderr

        return output_dict

class ParEvalProblemLoader(ProblemLoader):
    def __init__(self, data_path : str, benchmark_num_cpus=8):
        self.problem_list = []
        with open(data_path) as f:
            lst_problems = json.load(f)
            
            for problem_dict in lst_problems:
                llm4pp_problem = LLM4PP_Problem(problem_id=problem_dict["name"], source_code=problem_dict["src_code"], category=problem_dict["problem_type"], header=problem_dict["prompt"], target_benchmark = BenchmarkDescription(num_cpus=benchmark_num_cpus))

                self.problem_list.append(llm4pp_problem)

        self.problem_iterator = ProblemIterator(self.problem_list)

class ParEvalSubmissionRunner(SubmissionRunner):
    def __init__(self):
        pass

    def submit(self, submission : LLM4PP_Submission):
        # generate fake results.
        problem = submission.problem
        result = pareval_submit(problem.category, problem.problem_id, submission.submitted_code)
        return LLM4PP_SubmissionResponse(submission=submission,\
                                         compiled=result["did_build"],\
                                         correct=result["is_valid"],\
                                         runtime=result["optimized_runtime"],\
                                         reference_runtime=result["baseline_runtime"],\
                                        )

class ParEvalDriver(LLM4PP_Driver):
    def __init__(self, data_path="ParEval/prompts/code_opt.json"):
        self.problem_loader = ParEvalProblemLoader(data_path)
        self.runner = ParEvalSubmissionRunner()
        self.problems = []
        self.responses = []
        self.problem_loader_iterator = iter(self.problem_loader)

