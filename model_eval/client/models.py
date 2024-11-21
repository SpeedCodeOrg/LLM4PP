from typing import Annotated, Union, Optional, List
from pydantic import BaseModel

class BenchmarkDescription(BaseModel):
    num_cpus : Optional[int] = None

    # provide the runtime of the given source code.
    #   this provides some information about the benchmarks and benchmark inputs.
    source_code_runtime : Optional[float] = None

    # Description of the inputs used for benchmarks and correctness.
    # Example 1: "Square matricies ranging in size from 100 by 100 to 5000 by 5000."
    # Example 2: "Rectangular matrices where both dimensions are guaranteed to be a power of 2."
    input_description : Optional[str] = None

    # Description of the correctness tests.
    # Example 1: "The values in the output matrix must match a reference implementation with 
    #               an absolute error of less than 1e-5."
    # Example 2: "The values in the output matrix must match a reference implementation with 
    #               either an absolute error or relative error of less than 1e-6.
    #             If the input matrix is singular, then the code should return NULL."
    correctness_test_description : Optional[str] = None
    
class LLM4PP_Problem(BaseModel):#
    problem_id : str
    category : str
    source_code : str
    header : str
    # information about the target benchmark
    target_benchmark : Optional[BenchmarkDescription] = None

#class LLM4PP_SubmissionResult(BaseModel):
#    correct : bool # perhaps we provide this per runtime result?
#    num_cpus : int
#    runtime : float
#    runtime_reference : float

 
class LLM4PP_Submission(BaseModel):#
    problem : LLM4PP_Problem
    submitted_code : str

class LLM4PP_SubmissionResponse(BaseModel):
    submission : LLM4PP_Submission
    compiled : bool
    correct : bool
    runtime : float
    reference_runtime : Optional[float] = None
    # used for debugging
    stdout : Optional[str] = None
    stderr : Optional[str] = None

class LLM4PP_SubmissionResponseList(BaseModel):
    responses : List[LLM4PP_SubmissionResponse]

