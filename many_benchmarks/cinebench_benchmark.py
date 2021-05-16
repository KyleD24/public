"""
    Winter 2021
    cinebench_benchmark
    Author: Angelina Lam
"""

from os import path, chdir, system
from subprocess import run


def run_cinebench(program_path, benchmark_output_files):
    # joins the program path to the name of the doc to be created
    txt_path = path.join(program_path, benchmark_output_files['cinebench'])
    exe_path_folder = path.join(program_path, "CinebenchR23")

    if path.exists(exe_path_folder):
        chdir(exe_path_folder)
        # command to run cinebench in command line
        cinebench_run = "Cinebench.exe g_CinebenchAllTests=true g_CinebenchMinimumTestDuration=100 >" + "\"" + str(txt_path) + "\""
        system(cinebench_run)
    else:
        print("Cinebench file missing or in the wrong location.")

    return
