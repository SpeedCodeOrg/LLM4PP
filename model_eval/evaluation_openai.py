from client.models import LLM4PP_Problem, LLM4PP_Submission
from client.pareval_client import ParEvalDriver
from fastcoder.chatapi import MessageHistory, ChatAPI
import json
optimizer_prompt ="""You are a GPT that helps users iteratively optimize specific C and C++ functions in multiple steps. You optimize code contained in a function without changing its function signature or input/output behavior.

The user will provide you a JSON dictionary that contains a source code file
'solution.cpp' and a header file 'solution.hpp'

You should follow the following rules:

* You may **only** modify the file 'solution.cpp'. You may add include statements to 'solution.cpp' to include standard C or C++ libraries that are part of the standard library.
* Always provide code that is correct and compiles.
* Explain your decisions, in detail, when requested.
* Never change the function signature or input/output behavior of the program. Assume that the code is ''correct'' but ''slow''.
* Do not assume that code the user provides you is well optimized.
* You do not need to optimize the code in one step. if you are unable to **confidently** optimize the code you should perform additional analysis of the code or restructure it to be easier to analyze.

# Response format.

# Initial file contents

The initial file contents of solution.hpp and solution.cpp are below. Use the contents of solution.cpp as a reference when verfiying that your code transformations do not change the function signature or input/output behavior of the function.

## 'solution.hpp'
```hpp
$(SOLUTION_HPP)
```

## 'solution.cpp'
```cpp
$(SOLUTION_CPP)
```

# Correctness tests.

The following 'ProblemInput' class is used for testing correctness by comparing the results of the solution.cpp code with a known-correct reference implementation. The `check(ProblemInput& reference)` function verifies that the submitted code has the same input/output behavior as a correct reference implementation. **Reference 'input.hpp' to interpret error messages due to failed correctness tests**

## input.hpp
```hpp
$(INPUT_HPP)
```

# User prompts

The user will ask you to perform one of three tasks.

## Task 1: "task":'optimize'
For the 'optimize' task the user will provide you a JSON dictionary in the following format:

```json
{
    "task": 'optimize',
    "solution.cpp": '<current contents of solution.cpp>'
}
```

### Task 1 Response Format
For the 'optimize' task, you must respond with a JSON dictionary containing a two keys: 'analysis', 'updated_code', 'next_steps'.

* 'analysis' must contain your analysis of the 'solution.cpp' file for performance characteristics formatted in markdown style.
* 'updated_code' must contain your updated code that compiles, is correct, and makes progress towards the goal of improving performance.
* 'next_steps' describes the next step that should be taken to optimize the code.


## 'bugfix' task Prompt Format
For the 'bugfix' task, the user will ask you to correct either a compilation error, a program crash, or a failed correctness test in the code that you wrote. 

You must carefully analyze the *original* content of 'solution.cpp' and the most recent version of 'solution.cpp' that was in **your last message** (the last "assistant" message). Analyze the differences between the two versions of 'solution.cpp' to identify the issue and correct the problem.

```json
{
    "task": 'bugfix',
    "errors" : ['<error 1>', '<error 2>', '<error 3>']
}
```

### 'bugfix' task Response Format
For the 'bugfix' task, you must respond with a JSON dictionary containing a two keys: 'analysis', 'updated_code', 'next_steps'.

* 'analysis' must contain your analysis of the most recent 'solution.cpp' file shared with you by the user. Compare with the original contents of 'solution.cpp' to identify the bug that was introduced.
* 'updated_code' must contain your updated code that compiles, is correct, and makes progress towards the goal of fixing the bug.
* 'next_steps' describes the next step that should be taken to ensure that the code is correct.
"""

driver = ParEvalDriver()


chatAPI = ChatAPI()



for problem in driver:
    problem : LLM4PP_Problem


    messages = MessageHistory()

    system_prompt = optimizer_prompt.replace("$(INPUT_HPP)", "// Not given")\
                                    .replace("$(SOLUTION_HPP)", "// Not given")\
                                    .replace("$(SOLUTION_CPP)", problem.source_code)
    messages.add_message("system", system_prompt)
    messages.add_message("user", json.dumps({"task": "optimize",\
                                             "solution.cpp": problem.source_code}))
    response = chatAPI.get_response('gpt-4o-mini', messages, json_format=True)
    #print(json.loads(response)['updated_code'])
    #print('##########')
    #print(problem.source_code)


    # TODO: do something to optimize the code.
    optimized_code = json.loads(response)['updated_code']

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
print(chatAPI.get_cost())

prior_results = """
+-----------+------------+-------------+-----------------+
|  category | % compiled | correctness | geomean speedup |
+-----------+------------+-------------+-----------------+
|   graph   |    1.00    |     1.00    |       1.00      |
| histogram |    0.80    |     0.80    |       1.14      |
|    scan   |    1.00    |     0.60    |       1.30      |
| transform |    0.60    |     0.60    |       1.01      |
| sparse_la |    1.00    |     0.40    |       1.00      |
|   reduce  |    1.00    |     0.80    |       1.60      |
|    fft    |    0.80    |     0.40    |       1.00      |
|  geometry |    0.80    |     0.60    |       2.60      |
|  stencil  |    1.00    |     1.00    |       1.37      |
|  dense_la |    1.00    |     1.00    |       1.05      |
|    sort   |    0.40    |     0.20    |       1.00      |
|   search  |    1.00    |     1.00    |       1.00      |
|    all    |    0.87    |     0.70    |       1.20      |
+-----------+------------+-------------+-----------------+
"""


