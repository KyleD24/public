"""
Winter 2021
Many Benchmarks Project - Data Parsing
Authors: Angelina Lam, Kyle Dyck
"""
from os import path, chdir, remove
from math import floor


def parse_data(program_path, output_file_name, result_files, headers):
    # Create program output file
    output_file_path = path.join(program_path, output_file_name)
    result_file = open(output_file_path, "w+")
    # Change directory to current directory
    chdir(program_path)

    # Cinebench
    try:
        # Writes Cinebench results to program output file
        result_file.write(headers['cinebench'])
        query = "CB"
        query_counter = 2
        with open(result_files['cinebench']) as cinebench_results:
            for line in cinebench_results:
                if query in line and query_counter == 2:
                    cinebench_multi_core_list = line.split()
                    cinebench_multi_core = str(cinebench_multi_core_list[1])
                    result_file.write("Multiple CPU Render Test results = " + cinebench_multi_core + "\n")
                    query_counter = 1
                elif query in line:
                    cinebench_single_core_list = line.split()
                    cinebench_single_core = str(cinebench_single_core_list[1])
                    result_file.write("Single CPU Render Test results = " + cinebench_single_core + "\n")
        cinebench_results.close()

        # removes the output file created by cinebench
        remove(path.join(program_path, result_files['cinebench']))

    except FileNotFoundError as FE:
        print(FE.__str__())
        result_file.write("N/A\n")

    # FurMark
    try:
        result_file.write(headers['furmark'])
        fps_list = []

        with open(result_files['furmark_score']) as furmark_scores, open(result_files['furmark_fps']) as furmark_fps:
            # Get fps values
            for line in furmark_fps:
                if "fps=" in line:
                    fps_index = line.index('fps=')
                    # Extracts value from line
                    fps_value = int(line[fps_index:].split('"')[1])
                    fps_list.append(fps_value)
            # Calculate average FPS
            avg_fps = str(floor(sum(fps_list)/len(fps_list)))

            # Get score value
            for line in furmark_scores:
                if "Score=" in line:
                    score_index = line.index("Score=")
                    # Extracts value
                    furmark_score = line[score_index:].split(']')[0].split('=')
                    furmark_score = str(furmark_score[1])
        # Write average fps and score to results file
        result_file.write("FurMark score = " + furmark_score + "\n")
        result_file.write("Average fps = " + avg_fps + "\n")

        # Save changes
        furmark_scores.close()
        furmark_fps.close()

        # deletes the old output files from FurMark
        remove(path.join(program_path, result_files['furmark_score']))
        remove(path.join(program_path, result_files['furmark_fps']))

    except FileNotFoundError as E:
        print(E.__str__())
        result_file.write("N/A\n")

    # Heaven
    try:
        # Write heaven header to file
        result_file.write(headers['heaven'])
        # Values to extract
        score_value = None
        avg_fps_value = None
        min_fps = None
        max_fps = None

        with open(result_files['heaven']) as heaven_results:
            # Search every line for desired values
            for line in heaven_results:
                if "Min FPS:" in line:
                    # Extract value from line
                    min_fps = line.split('strong')[1][1:-2]
                elif "Score:" in line:
                    # Extract value from line
                    score_value = line.split('strong')[1][1:-2]
                elif "Max FPS:" in line:
                    # Extract value from line
                    max_fps = line.split('strong')[1][1:-2]
                elif "FPS:" in line:
                    # Extract value from line
                    avg_fps_value = line.split('strong')[1][1:-2]

        heaven_results.close()
        remove(result_files['heaven'])

        # Write values to results file
        result_file.write("Heaven Score: " + score_value + "\n")
        result_file.write("Average FPS: " + avg_fps_value + "\n")
        result_file.write("Max FPS: " + max_fps + "\n")
        result_file.write("Min FPS: " + min_fps + "\n")

    except FileNotFoundError as FE:
        print(FE.__str__())
        result_file.write("N/A\n")

    result_file.close()
