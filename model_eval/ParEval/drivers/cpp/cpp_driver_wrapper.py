""" Wrapper for calling c++ drivers
    author: Daniel Nichols
    date: October 2023
"""
# std imports
import copy
import logging
import os
from os import PathLike, environ
import shlex
import subprocess
import sys
import tempfile
from typing import List

# local imports
sys.path.append("..")
from drivers.driver_wrapper import DriverWrapper, BuildOutput, RunOutput, GeneratedTextResult
from util import run_command

import time
import re

""" Map parallelism models to driver files """
DRIVER_MAP = {
    "serial": "serial-driver.o",
    "omp": "omp-driver.o",
    "mpi": "mpi-driver.o",
    "mpi+omp": "mpi-omp-driver.o",
    "kokkos": "kokkos-driver.o",
    "cuda": "cuda-driver.o",
    "hip": "hip-driver.o",
}

""" Compiler settings """
COMPILER_SETTINGS = {
    # use c++20 as fast random number generation requires it
    "serial": {"CXX": "g++", "CXXFLAGS": "-std=c++20 -O3"},
    "omp": {"CXX": "g++", "CXXFLAGS": "-std=c++20 -O3 -fopenmp"},
    "mpi": {"CXX": "mpicxx", "CXXFLAGS": "-std=c++17 -O3"},
    "mpi+omp": {"CXX": "mpicxx", "CXXFLAGS": "-std=c++17 -O3 -fopenmp"},
    "kokkos": {"CXX": "g++", "CXXFLAGS": "-std=c++17 -O3 -fopenmp -I../tpl/kokkos/build/include ../tpl/kokkos/build/lib64/libkokkoscore.a ../tpl/kokkos/build/lib64/libkokkoscontainers.a ../tpl/kokkos/build/lib64/libkokkossimd.a"},
    "cuda": {"CXX": "nvcc", "CXXFLAGS": "-std=c++17 --generate-code arch=compute_80,code=sm_80 -O3 -Xcompiler \"-std=c++17 -O3\""},
    "hip": {"CXX": "hipcc", "CXXFLAGS": "-std=c++17 -O3 -Xcompiler \"-std=c++17\" -Xcompiler \"-O3\" -Wno-unused-result"}
}

# There are certain problems that don't play nice with code optimization.
# baseline code sometimes defines helper functions that will conflict with a LLM's output.
# This only affects a few problems below, so this is a quick and dirty hack to solve some compiler errors that arise for these problems.
def check_code_compile_errors(output):
    if "dfs(" in output:
        output = output.replace("dfs(", "dfs_helper(")

    if "void fft(std::vector<std::complex<double>> &x)" in output:
        output = output.replace("fft(", "fft_helper(")
        output = output.replace("ifft_helper(", "ifft(")

    return output

def check_duplicate_function_names(output):
    function_names = get_cpp_function_names(output)

    # only check for repeating the same function name
    # this is a hack and assumes this scenario only occurs when there is 1 function defined and repeated multiple times
    if len(set(function_names)) == 1 and len(function_names) > 1:
        output = get_code_until_first_function(output)

    return output

def build_kokkos(driver_src: PathLike, output_root: PathLike, problem_size: str = "(1<<20)"):
    """ Custom steps for the Kokkos programs, since they require cmake """
    # cp cmake file into the output directory
    cmake_path = "cpp/KokkosCMakeLists.txt"
    cmake_dest = os.path.join(output_root, "CMakeLists.txt")
    run_command(f"cp {cmake_path} {cmake_dest}", dry=False)

    # run cmake and make
    pwd = os.getcwd()
    cmake_flags = f"-DKokkos_DIR=../tpl/kokkos/build -DDRIVER_PATH={pwd} -DDRIVER_SRC_FILE={driver_src} -DDRIVER_PROBLEM_SIZE=\"{problem_size}\""
    cmake_out = run_command(f"cmake -B{output_root} -S{output_root} {cmake_flags}", dry=False)
    if cmake_out.returncode != 0:
        return cmake_out
    return run_command(f"make -C {output_root}", dry=False)

class CppDriverWrapper(DriverWrapper):

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.model_driver_file = os.path.join("cpp", "models", DRIVER_MAP[self.parallelism_model])

    def write_source(self, content: str, fpath: PathLike) -> bool:
        """ Write the given c++ source to the given file. """
        with open(fpath, "w") as fp:
            fp.write(content)
        return True

    def patch_prompt(self, content: str) -> str:
        """ Add NO_INLINE to the given source code. """
        # the last line of content should be: return_type function_name(args) {
        # we want to add NO_INLINE after the return_type
        parts = content.split("\n")[-1].split(" ")
        assert len(parts) > 1, f"Could not parse return type from {parts}"
        parts.insert(1, "NO_INLINE")
        return "\n".join(content.split("\n")[:-1] + [" ".join(parts)])

    def compile(
        self, 
        *binaries: PathLike, 
        output_path: PathLike = "a.out", 
        CXX: str = "g++", 
        CXXFLAGS: str = "-std=c++17 -O3",
        problem_size: str = "(1<<20)"
    ) -> BuildOutput:
        """ Compile the given binaries into a single executable. """
        if self.parallelism_model == "kokkos":
            driver_src = [b for b in binaries if b.endswith(".cc")][0]
            compile_process = build_kokkos(driver_src, os.path.dirname(output_path), problem_size=problem_size)
        else:
            binaries_str = ' '.join(binaries)
            macro = f"-DUSE_{self.parallelism_model.upper()}"
            cmd = f"{CXX} {CXXFLAGS} -Icpp -Icpp/models {macro} {binaries_str} -o {output_path}"
            try:
                compile_process = run_command(cmd, timeout=self.build_timeout, dry=self.dry)
            except subprocess.TimeoutExpired as e:
                return BuildOutput(-1, str(e.stdout), f"[Timeout] {str(e.stderr)}")
        return BuildOutput(compile_process.returncode, compile_process.stdout, compile_process.stderr)

    def run(self, executable: PathLike, **run_config) -> RunOutput:
        """ Run the given executable. """
        launch_format = self.launch_configs["format"]
        launch_cmd = launch_format.format(exec_path=executable, args="", **run_config).strip()
        try:
            run_process = run_command(launch_cmd, timeout=self.run_timeout, dry=self.dry)
        except subprocess.TimeoutExpired as e:
            print("--- RUN TIMEOUT ---")
            return RunOutput(-1, str(e.stdout), f"[Timeout] {str(e.stderr)}", config=run_config)
        except UnicodeDecodeError as e:
            assert False
            logging.warning(f"UnicodeDecodeError: {str(e)}\nRunnning command: {launch_cmd}")
            return RunOutput(-1, "", f"UnicodeDecodeError: {str(e)}", config=run_config)

        return RunOutput(run_process.returncode, run_process.stdout, run_process.stderr, config=run_config)

    def test_single_output(self, prompt: str, output: str, test_driver_file: PathLike, problem_size: str) -> GeneratedTextResult:
        """ Test a single generated output. """
        code_opt = True

        logging.debug(f"Testing output:\n{output}")
        with tempfile.TemporaryDirectory(dir=self.scratch_dir) as tmpdir:
            # write out the prompt + output
            src_ext = "cuh" if self.parallelism_model in ["cuda", "hip"] else "hpp"
            src_path = os.path.join(tmpdir, f"generated-code.{src_ext}")

            # include the entire C++ standard library as well as header for vectorization
            include_header = "#include <bits/stdc++.h>\n#include <immintrin.h>\n"
            if self.code_opt:
                output = check_code_compile_errors(output)
                output = check_duplicate_function_names(output)
                write_success = self.write_source(include_header+"\n"+output, src_path)
            else:
                prompt = self.patch_prompt(prompt)
                write_success = self.write_source(include_header+"\n"+prompt+"\n"+output, src_path)

            logging.debug(f"Wrote source to {src_path}.")

            # compile and run the output
            exec_path = os.path.join(tmpdir, "a.out")
            compiler_kwargs = copy.deepcopy(COMPILER_SETTINGS[self.parallelism_model])
            compiler_kwargs["problem_size"] = problem_size  # for kokkos
            compiler_kwargs["CXXFLAGS"] += f" -I{tmpdir} -DDRIVER_PROBLEM_SIZE=\"{problem_size}\""
            build_result = self.compile(self.model_driver_file, test_driver_file, output_path=exec_path, **compiler_kwargs)
            if build_result.exit_code != 0:
                print(f"----- DID NOT BUILD ---- build result stderr: {build_result.stderr}")
                print("--- CODE FILE ---")
                print(output)

                print("--- PROMPT ---")
                print(prompt)

            logging.debug(f"Build result: {build_result}")
            if self.display_build_errors and build_result.stderr and not build_result.did_build:
                logging.debug(build_result.stderr)

            # run the code
            configs = self.launch_configs["params"]
            if build_result.did_build:
                run_results = []
                for c in configs:
                    start = time.time()
                    run_result = self.run(exec_path, **c)
                    end = time.time()
                    print(f"one run time: {end - start}")
                    run_results.append(run_result)

                    if run_result.is_valid:
                        speedup = run_result.best_sequential_runtime / run_result.runtime
                        print(f"valid run runtime: {run_result.runtime}, best sequential runtime: {run_result.best_sequential_runtime}, speedup: {run_result.best_sequential_runtime / run_result.runtime}")
                        if speedup > 20:
                            print("--- FAST OUTPUT ---")
                            print(prompt+"\n"+output)
                            print("--- RUN RESULT STDOUT ---")
                            print(run_result.stdout)
                            print("--- RUN RESULT STDERR ---")
                            print(run_result.stderr)
                    else:
                        print("--- INCORRECT ---")
                        print(run_result.stdout)

                    if self.display_runs:
                        logging.debug(run_result.stderr)
                        logging.debug(run_result.stdout)

                    if self.early_exit_runs and (run_result.exit_code != 0 or not run_result.is_valid):
                        break
            else:
                run_results = None
            logging.debug(f"Run result: {run_results}")
            if run_results:
                for run_result in run_results:
                    if run_result.exit_code != 0:
                        logging.debug(f"Ouputs:\n\tstdout: {run_result.stdout}\n\tstderr: {run_result.stderr}")
        
        return GeneratedTextResult(write_success, build_result, run_results)

# ---- Helper functions for parsing ----
def get_cpp_function_names(code: str):
    """
    Extracts all function names from a C++ source file.

    Args:
        file_path (str): Path to the C++ file.

    Returns:
        list: A list of function names found in the file.
    """
    # Regular expression to match C++ function signatures
    pattern = r'\b(\w+)\s+(\w+)\s*\([^)]*\)\s*\{'

    function_names = []

    # Find all matches of the regex in the file
    matches = re.findall(pattern, code)

    # Extract only the function names (second group)
    function_names = [match[1] for match in matches]

    return function_names

def get_code_until_first_function(lines : str):
    """
    Extracts all C++ code up to and including the first function with balanced braces.

    Args:
        file_path (str): Path to the C++ file.

    Returns:
        str: The code up to and including the first function.
    """

    # Variables to track the function code and brace count
    function_code = []
    brace_count = 0
    in_function = False

    # Iterate over the lines to find the first function
    for line in lines:
        function_code.append(line)

        # Check if this line marks the start of a function
        if '{' in line:
            brace_count += line.count('{')
            in_function = True  # We've entered the function body

        if '}' in line and in_function:
            brace_count -= line.count('}')

        # If braces are balanced, we've reached the end of the function
        if in_function and brace_count == 0:
            break

    return ''.join(function_code)  # Join the lines into a single string
