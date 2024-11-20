
from client.driver import LLM4PP_Driver
from client.models import LLM4PP_Problem, LLM4PP_Submission


# import base classes for mock implementations
from client.driver import ProblemLoader, SubmissionRunner
def get_mock_driver():
    problem_loader = ProblemLoader("fake")
    runner = SubmissionRunner()
    return LLM4PP_Driver(problem_loader, runner)


driver = get_mock_driver()

for problem in driver:
    problem : LLM4PP_Problem

    # do something to optimize the code.
    modified_source_code = f"// Optimized code!\n{problem.source_code}"
    submission = LLM4PP_Submission(problem=problem,
                                   submitted_code = modified_source_code)
    driver.submit(submission)

driver.save_all_responses("./tmp-mock-results.json")
driver.evaluate()
    