from client.models import LLM4PP_Problem, LLM4PP_Submission
from client.pareval_client import ParEvalDriver

driver = ParEvalDriver()

for problem in driver:
    problem : LLM4PP_Problem

    # do something to optimize the code.
    modified_source_code = problem.source_code
    submission = LLM4PP_Submission(problem=problem,
                                   submitted_code=modified_source_code)
    response = driver.submit(submission)

driver.save_all_responses("./tmp-pareval-results.json")
driver.evaluate()

