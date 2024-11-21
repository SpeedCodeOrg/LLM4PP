from client.models import *
from dataclasses import dataclass
from typing import List, Iterator, Set
import copy
import random
import math
from prettytable import PrettyTable

class ProblemIterator:
    problem_list : List[LLM4PP_Problem]
    #iterator_index : int = 0
    def __init__(self, problem_list : List[LLM4PP_Problem]):
        self.problem_list = copy.deepcopy(problem_list)
        # validate
        for i in range(len(self.problem_list)):
            self.problem_list[i] = self.problem_list[i].parse_obj(self.problem_list[i])
        #self.iterator_index = 0 

    def __iter__(self) -> Iterator[LLM4PP_Problem]:
        #x = copy.deepcopy(self)
        #x.iterator_index = 0
        return iter(self.problem_list)

    #def __next__(self):
    #    if self.iterator_index >= len(self.problem_id_list):
    #        raise StopIteration
    #    problem = self.problem_list[self.iterator_index] 
    #    self.iterator_index += 1
    #    return problem 

# should be subclassed. The default implementation is a mock implementation.
class ProblemLoader:
    problem_list : List[LLM4PP_Problem]
    problem_iterator : ProblemIterator
    def __init__(self, fake_path : str):
        fake_categories = ["cat-A", "cat-B", "cat-C"]
        self.problem_list = []
        for i in range(0,100):
            problem = LLM4PP_Problem(problem_id = f"fake-{i}",\
                           source_code = f"fake source code {i}",\
                           header = f"fake header {i}",
                           category = f"{fake_categories[i%len(fake_categories)]}",\
                           target_benchmark = BenchmarkDescription(num_cpus = 8))
            self.problem_list.append(problem)
        self.problem_iterator = ProblemIterator(self.problem_list)

    def __iter__(self) -> Iterator[LLM4PP_Problem]:
        return iter(self.problem_iterator)
    

# Should be subclassed. Default implementation is a mock. 
class SubmissionRunner:
    def __init__(self):
        pass

    def submit(self, submission : LLM4PP_Submission):
        # generate fake results.
        return LLM4PP_SubmissionResponse(submission = submission,\
                                         compiled = True,\
                                         correct=True,\
                                         runtime=random.uniform(0,1),\
                                         reference_runtime=random.uniform(0,1),\
                                        )

class LLM4PP_Driver:
    problem_loader : ProblemLoader
    #problem_loader_iterator : Iterator[ProblemLoader]
    problems : List[LLM4PP_Problem]
    responses : List[LLM4PP_SubmissionResponse]
    runner : SubmissionRunner

    def __init__(self, problem_loader : ProblemLoader, runner : SubmissionRunner):
        assert isinstance(problem_loader, ProblemLoader)
        assert isinstance(runner, SubmissionRunner)

        self.problem_loader = problem_loader
        self.runner = runner
        self.problem_loader_iterator = iter(self.problem_loader)
        self.problems = []
        self.responses = []

    def __iter__(self) -> Iterator[LLM4PP_Problem]:
        return self
    
    def __len__(self):
        return len(self.problem_loader_iterator)
    
    def __next__(self) -> LLM4PP_Problem:
        n = next(self.problem_loader_iterator)
        self.problems.append(copy.deepcopy(n))
        return n
    
    def submit(self, submission : LLM4PP_Submission) -> None:
        submission = LLM4PP_Submission.parse_obj(submission) # validate
        response = self.runner.submit(submission)
        self.responses.append(response)
        return response

    def save_all_responses(self, filename):
        open(filename, 'w+').write(LLM4PP_SubmissionResponseList(responses=self.responses).json())

    def sanity_check(self) -> bool:
        # verify that there exists one submission for each problem.
        problem_ids : Set[str] = set()
        for x in self.problems:
            assert x.problem_id not in problem_ids
            problem_ids.add(x.problem_id)
        
        response_problem_ids : Set[str] = set()
        for x in self.responses:
            assert x.submission.problem.problem_id in problem_ids
            assert x.submission.problem.problem_id not in response_problem_ids
            response_problem_ids.add(x.submission.problem.problem_id)

    def evaluate(self):
        sanity_check_passed = False
        #try:
        sanity_check_passed = self.sanity_check()
        #except Exception:
        #    print("Failed to pass Evaluation sanity check... there's probably something wrong.")
        
        # compute the geomean of speedup over the reference code.

        @dataclass
        class statistics:
            addone : bool
            total : int
            total_correct : int
            logsum : float
            total_compiled : int




        category_statistics = dict()
        all_statistics = statistics(addone=False, total=0, total_correct=0, logsum=0, total_compiled=0)

        for x in self.responses:
            problem_id = x.submission.problem.problem_id
            problem_cat = x.submission.problem.category
            if problem_cat not in category_statistics:
                category_statistics[problem_cat] = statistics(addone=False,\
                                                              total=0,\
                                                              total_correct=0,\
                                                              logsum=0,\
                                                              total_compiled=0)

            category_statistics[problem_cat].total += 1
            all_statistics.total += 1

            if not x.correct:
                logratio = math.log(1.0)
            else:
                all_statistics.total_correct += 1
                category_statistics[problem_cat].total_correct += 1
                logratio = math.log(max(1.0,x.reference_runtime / x.runtime))

            if x.compiled:
                category_statistics[problem_cat].total_compiled += 1
                all_statistics.total_compiled += 1

            category_statistics[problem_cat].logsum += logratio
            all_statistics.logsum += logratio

        table = PrettyTable()
        table.field_names = ["category", "% compiled", "correctness", "geomean speedup"]
        table.float_format = '.2'
        for category in category_statistics.keys():
            table.add_row([category,\
                       category_statistics[category].total_compiled / category_statistics[category].total,\
                       category_statistics[category].total_correct / category_statistics[category].total,\
                       math.exp(category_statistics[category].logsum/category_statistics[category].total)])
        table.add_row(["all",\
                   all_statistics.total_compiled / all_statistics.total,\
                   all_statistics.total_correct / all_statistics.total,\
                   math.exp(all_statistics.logsum/all_statistics.total)])
        print(table)
         














     
    

#def test(x : LLM4PP_Problem):
#    x = LLM4PP_Problem.parse_obj(x) # validation.
#    print(x)





#test(LLM4PP_Problem(problem_id=str(1), source_code="", header=""))

            
        


