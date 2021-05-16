"""
Winter 2021
Many Benchmarks Project - Main
Authors: Kyle Dyck, Angelina Lam
"""

from cinebench_benchmark import run_cinebench
from furmark_benchmark import run_furmark_with_scores
from data_parsing import parse_data
from os import path, system, chdir
from subprocess import run


PROGRAM_PATH = path.dirname(__file__).replace('/', '\\')
OUTPUT_FILE_NAME = "many_benchmarks_results.txt"
BENCHMARK_OUTPUT_FILES = {
    'cinebench': "cinebench_results.txt",
    'furmark_score': "FurMark_score.txt",
    'furmark_fps': "FurMark_fps.txt",
    'heaven': "heaven_results.html"
}
HEADERS = {
    'cinebench': """---------------------------
CINEBENCH BENCHMARK RESULTS
--------------------------- \n""",
    'furmark': """---------------------------
FURMARK BENCHMARK RESULTS
--------------------------- \n""",
    'heaven': """---------------------------
HEAVEN BENCHMARK RESULTS
--------------------------- \n"""
}

run_furmark_with_scores(PROGRAM_PATH, BENCHMARK_OUTPUT_FILES)
run_cinebench(PROGRAM_PATH, BENCHMARK_OUTPUT_FILES)
chdir(PROGRAM_PATH)
run(['python', 'heaven_benchmark.py', '--f', BENCHMARK_OUTPUT_FILES['heaven']])
parse_data(PROGRAM_PATH, OUTPUT_FILE_NAME, BENCHMARK_OUTPUT_FILES, HEADERS)
input('Benchmarking complete. Results can be found in: ' + OUTPUT_FILE_NAME + '. Press ENTER to exit program.')
