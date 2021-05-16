"""
    Winter 2021
    furmark_benchmark
    Author: Angelina Lam
"""

from os import path, chdir, system, remove
from shutil import copyfile

def run_furmark_with_scores(program_path,benchmark_output_files):
    # creates files for program to output to
    txt_path_score = path.join(program_path, benchmark_output_files['furmark_score'])
    txt_path_fps = path.join(program_path, benchmark_output_files['furmark_fps'])
    exe_folder_path = path.join(program_path, "FurMark")

    # checks to see if exe for furmark exsits
    if path.exists(exe_folder_path + '\\FurMark.exe'):
        chdir(exe_folder_path)
        #comand for durmark to run in cmd line
        furmark_cmd = "FurMark.exe /nogui /max_time=240000 /run_mode=1 /log_temp /log_temperature /log_score /noscore"
        system(furmark_cmd)
    else:
        raise FileNotFoundError(__str__='Furmark file is missing')

    # makes a file that we can copy furmark scores to this file
    with open(txt_path_score, "w+") as file:
        file.close()

    # makes a file that we can copy furmark fps to this file
    with open(txt_path_fps, "w+") as file:
        file.close()

    # copies file content into created files above
    copyfile("furmark-gpu-monitoring.xml", txt_path_fps)
    copyfile("FurMark-Scores.txt", txt_path_score)

    # deletes score file
    original_furmark_score_txt = path.join(program_path, 'FurMark', 'FurMark-Scores.txt')
    remove(original_furmark_score_txt)

    return




