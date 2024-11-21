from client.models import LLM4PP_Problem, LLM4PP_Submission
from client.pareval_client import ParEvalDriver

driver = ParEvalDriver()

for problem in driver:
    problem : LLM4PP_Problem

    # TODO: do something to optimize the code.
    optimized_code = problem.source_code
    submission = LLM4PP_Submission(problem=problem,
                                   submitted_code=optimized_code)
    try:
        response = driver.submit(submission)
    except Exception as e:
        print(f"skipping problem due to exception: {e}")
        print("--- ParEval driver stdout ---")
        print(response["stdout"])

driver.save_all_responses("./tmp-pareval-results.json")
driver.evaluate()

