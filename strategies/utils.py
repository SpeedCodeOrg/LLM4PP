import jsonlines
from typing import List
import os

def clean_output(response: str, language: str="c++") -> str:
    """ Extracts the code block from the given response text.
    
    Args:
        response (str): The generated response containing code wrapped in triple backticks.

    Returns:
        code_block (str): The extracted code block, or an empty string if no code block is found.
    """
    # Find the first and second occurrences of the code block delimiter
    start = response.find(f"```{language}")
    if start == -1:
        return ""  # No code block found

    # Find the next delimiter after the first one
    end = response.find("```", start + 3)
    if end == -1:
        return ""  # No closing delimiter found

    # Extract the code block
    code_block = response[start + 3 + len(language):end].strip()
    return code_block

def read_jsonl(path: str) -> List[dict]:
    if not os.path.exists(path):
        raise FileNotFoundError(f"File `{path}` does not exist.")
    elif not path.endswith(".jsonl"):
        raise ValueError(f"File `{path}` is not a jsonl file.")
    items = []
    with jsonlines.open(path) as reader:
        for item in reader:
            items += [item]
    return items

def enumerate_resume(dataset, results_path):
    if not os.path.exists(results_path):
        for i, item in enumerate(dataset):
            yield item
    else:
        count = 0
        with jsonlines.open(results_path) as reader:
            for item in reader:
                count += 1

        for i, item in enumerate(dataset):
            # skip items that have been processed before
            if i < count:
                continue
            yield item

