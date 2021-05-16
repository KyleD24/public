"""
Winter 2021
Many Benchmarks Project - Heaven Benchmark
Author: Kyle Dyck
"""

from pynput.keyboard import Controller as kbControl                      # https://pynput.readthedocs.io/en/latest/
from pynput.keyboard import Key                                          # Special keyboard keys
from multiprocessing import Process, freeze_support, set_start_method    # To have keyboard input + run benchmark
from time import sleep                                                   # Create delays
from pygetwindow import getWindowsWithTitle as getWindow                 # Work with application windows
from os import path, remove, system, chdir
from shutil import copyfile
from sys import argv
import argparse as ap


# Class to contain arguments for input_keys function
class InputKeyArgs():
    def __init__(self, keyboard_controller, keys_to_input, delays, window_name, pre_delay=0):
        self.kb_ctrl = keyboard_controller
        self.keys = keys_to_input
        self.delays = delays
        self.window_name = window_name
        self.pre_delay = pre_delay


# Press a sequence of keys, each separated by a corresponding time delay in seconds in a specified Window
# Works with specific commands + any other string will be typed
def input_keys(args):
    keyboard_controller = args.kb_ctrl
    keys_to_input = args.keys
    delays = args.delays
    window_name = args.window_name
    pre_delay = args.pre_delay

    # Number of keys must match number of delays
    assert len(keys_to_input) == len(delays)

    # Delay before execution
    sleep(pre_delay)

    for i in range(len(keys_to_input)):
        key_input = keys_to_input[i]
        next_delay = delays[i]
        # Check if valid delay input
        assert delays[i] >= 0

        # Press key

        # Select desired window to put input
        try:
            desired_window = getWindow(window_name)[0]
        except IndexError as IE:
            print('getWindow Error: ' + IE.__str__())
            print(window_name + ' is not open / running')
            return

        desired_window.activate()

        # Keyboard input, either one character or multiple characters
        if len(key_input) == 1:
            keyboard_controller.press(key_input)
            keyboard_controller.release(key_input)
        elif key_input == 'exit':
            # Alt-F4
            keyboard_controller.press(Key.alt)
            keyboard_controller.press(Key.f4)
            keyboard_controller.release(Key.alt)
            keyboard_controller.release(Key.f4)
        elif key_input == 'F9':
            # Press Enter
            keyboard_controller.press(Key.f9)
            keyboard_controller.release(Key.f9)
        elif key_input == 'clear_name':
            # Spam backspace to clear line
            for j in range(50):
                keyboard_controller.press(Key.backspace)
                keyboard_controller.release(Key.backspace)
        elif key_input == 'enter':
            # Press enter
            keyboard_controller.press(Key.enter)
            keyboard_controller.release(Key.enter)
        elif len(key_input) > 1:
            keyboard_controller.type(key_input)

        # Delay
        sleep(next_delay)

    return


# Modifies given file by searching for a given phrase, then replacing its line with a given phrase
# Returns True if successful
def modify_file(file_name, query, new_line):
    query_found = False

    # Check that file exists
    if not path.exists(file_name):
        # Given file doesn't exist
        return False

    # Overwrite old file with new phrase
    # If query not found, file is unchanged
    with open(file_name, encoding="Latin-1") as file, open('temp.js', mode='w', encoding="Latin-1") as temp_file:
        for line in file:
            if query in line:
                line = new_line + '\n'
                query_found = True
            temp_file.write(line)

        file.close()
        temp_file.close()

        if query_found:
            # Overwrite old file
            with open('temp.js', encoding="Latin-1") as temp_file, open(file_name, mode='w', encoding="Latin-1") as old_file:
                # Get lines to write
                to_write = temp_file.readlines()
                # Write lines back to old file
                old_file.writelines(to_write)
                # Done
                temp_file.close()
                old_file.close()

        # Delete temp file
        remove('temp.js')

    return True


# Executes the Heaven benchmark which must exist in the correct directory
# Raises FileNotFoundError
def execute_heaven_program(current_path):
    heaven_batch_location = current_path + '\\Heaven Benchmark 4.0'
    heaven_batch_name = 'heaven.bat'
    if path.exists(heaven_batch_location + '\\' + heaven_batch_name):
        # Good, execute it
        # Change directory
        chdir(heaven_batch_location)
        # Run
        system(heaven_batch_name)
    else:
        raise FileNotFoundError()


# Runs the benchmark and captures results in a file
# Returns nothing, raises FileNotFound errors
def run_heaven(result_file_name, testing=False):
    # Initialize keyboard controller
    keyboard = kbControl()

    current_path = path.dirname(__file__).replace('/', '\\')
    # Must modify .js file to automate benchmark
    path_to_js_file = current_path + '\\Heaven Benchmark 4.0\\data\\launcher\\js\\heaven-ui-logic.js'
    result = modify_file(path_to_js_file,
                         '$(\'#run\').click(startBenchmark);',
                         '\tstartBenchmark();')

    if result:
        # Continue with program

        # Create processes to run in parallel
        heaven_program_process = Process(target=execute_heaven_program, args=(current_path,))
        heaven_program_process.start()
        # Keys that need to be inputted to capture benchmark
        if testing:
            # Shorter delays for testing purposes
            keys_to_press = ['exit']
            delays = [10]
        else:
            keys_to_press = ['F9', 'enter', 'clear_name', result_file_name, 'enter', 'exit']
            delays = [300, 1, 1, 1, 1, 15]
        window_name = 'Unigine Heaven Benchmark 4.0 Basic (Direct3D11)'
        key_input_args = InputKeyArgs(keyboard, keys_to_press, delays, window_name, 15)
        input_keys(key_input_args)
        # Close Heaven settings window
        window_name = 'Unigine Heaven Benchmark 4.0 (Basic Edition)'
        key_input_args = InputKeyArgs(keyboard, ['exit'], [1], window_name)
        input_keys(key_input_args)

    else:
        # File was missing
        raise FileNotFoundError(__str__='Missing Heaven file')


if __name__ == '__main__':
    # Argument parsing
    parser = ap.ArgumentParser()
    parser.add_argument('--f',
                        default='heaven_results.html',
                        required=False,
                        dest='heaven_result_file_name')
    parser.add_argument('--t',
                        default=False,
                        type=bool,
                        required=False,
                        dest='testing')
    temp_namespace = parser.parse_args()

    heaven_result_file_name = temp_namespace.heaven_result_file_name
    testing = temp_namespace.testing

    try:
        set_start_method('spawn')
        run_heaven(heaven_result_file_name, testing)

        # Get required directories
        current_dir = path.dirname(__file__).replace('/', '\\')
        result_file_dir = path.expanduser('~')

        if path.exists(result_file_dir + '\\' + heaven_result_file_name):
            # Good, proceed

            # Copy file to desired directory
            copyfile(result_file_dir + '\\' + heaven_result_file_name, current_dir + '\\' + heaven_result_file_name)

            # Delete old file
            remove(result_file_dir + '\\' + heaven_result_file_name)
            pass

        else:
            raise FileNotFoundError()

    except FileNotFoundError as FE:
        print(FE.__str__)
        print('Missing Heaven file')
