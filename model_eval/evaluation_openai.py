from client.models import LLM4PP_Problem, LLM4PP_Submission
from client.pareval_client import ParEvalDriver
from fastcoder.chatapi import MessageHistory, ChatAPI
import json

optimizer_prompt ="""You are a coding expert that writes very fast code. You write parallel C and C++ code using OpenMP and always strive to make the code as fast as possible. The user will give you code and you will provide a modified version of the user's code that is as fast as possible.

# Prompt format

The user will provide you a JSON dictionary in the following format:

```json
{
    "source_code" : <Initial code>
}
```

# Response format

You will respond with a JSON dictionary in the following format:

```json
{
    "updated_code" : <Optimized code>
}
```
"""

driver = ParEvalDriver()
chatAPI = ChatAPI()

for problem in driver:
    problem : LLM4PP_Problem

    messages = MessageHistory()
    messages.add_message("system", optimizer_prompt)
    messages.add_message("user", json.dumps({"solution.cpp": problem.source_code}))
    response = chatAPI.get_response('gpt-4o-mini', messages, json_format=True)
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


# Example output
#"""
#+-----------+------------+-------------+-----------------+
#|  category | % compiled | correctness | geomean speedup |
#+-----------+------------+-------------+-----------------+
#|   graph   |    1.00    |     0.60    |       1.09      |
#| histogram |    1.00    |     1.00    |       4.69      |
#|    scan   |    1.00    |     0.40    |       1.26      |
#| transform |    0.80    |     0.80    |       1.19      |
#| sparse_la |    1.00    |     0.80    |       2.63      |
#|   reduce  |    1.00    |     1.00    |       5.50      |
#|    fft    |    1.00    |     0.60    |       1.69      |
#|  geometry |    1.00    |     0.60    |       1.23      |
#|  stencil  |    1.00    |     1.00    |       2.84      |
#|  dense_la |    1.00    |     1.00    |       2.37      |
#|    sort   |    0.80    |     0.40    |       1.00      |
#|   search  |    0.80    |     0.60    |       1.12      |
#|    all    |    0.95    |     0.73    |       1.86      |
#+-----------+------------+-------------+-----------------+
#"""


