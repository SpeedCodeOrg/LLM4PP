from client.driver import LLM4PP_Driver
from client.models import LLM4PP_Problem, LLM4PP_Submission


# import base classes for mock implementations
from client.driver import ProblemLoader, SubmissionRunner
from client.pareval_client import ParEvalDriver

def get_mock_driver():
    problem_loader = ProblemLoader("fake")
    runner = SubmissionRunner()
    return LLM4PP_Driver(problem_loader, runner)

def get_pareval_driver():
    return ParEvalDriver()

# driver = get_mock_driver()
driver = get_pareval_driver()

for idx, problem in enumerate(driver):
    problem : LLM4PP_Problem

    if idx >= 2:
        break

    # do something to optimize the code.
    # modified_source_code = f"// Optimized code!\n{problem.source_code}"
    modified_source_code = problem.source_code
    submission = LLM4PP_Submission(problem=problem,
                                   submitted_code=modified_source_code)
    driver.submit(submission)

driver.save_all_responses("./tmp-mock-results.json")
driver.evaluate()

