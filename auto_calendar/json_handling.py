import json
import jsonpickle
import os.path
import pathlib        # Used in main file
from MasterClass import *


# Encodes a Python object into a json string
# Takes object, returns json string
def __encode_obj_to_json(object):
    json_temp = jsonpickle.encode(object)

    return json_temp


# Decodes a json string into a Python object
# Takes json string, returns object(s)
def __decode_json_to_obj(string):
    json_temp = jsonpickle.decode(string)

    return json_temp


# Reads JSON file and returns json string
# Must be decoded to turn back to Python object
def __read_from_file(file_name):
    file = open(file_name, "r")
    text = file.read()
    file.close()

    if len(text) <= 1:
        # File blank, return empty string
        return ""
    else:
        with open(file_name, 'r') as json_file:
            temp = json.load(json_file)
            return temp


# Saves a json string to a file
def js_write_to_file(file_name, json_string, strip_quotes=False):
    with open(file_name, 'w') as outfile:
        json.dump(json_string, outfile)
        outfile.close()


# Loads the named json file, creates new file if none exists
# Returns a Python object, empty MasterClass if new file was created
def jh_load_from_file(file_name, program_path, default_calendar, app_version):
    # Check if file exists first
    if os.path.isfile(program_path + "\\" + file_name):
        # File exists, check if file is blank
        file = open(file_name, "r")
        text = file.read()
        file.close()
        if len(text) <= 1:
            # File blank, delete it and create a new one (below)
            os.remove(file_name)
        else:
            # Not blank, read file and get object
            json_string = __read_from_file(file_name)
            loaded_object = __decode_json_to_obj(json_string)
            return loaded_object
    else:
        pass

    # File does not exist, create new one
    new_file = open(file_name, "w+")
    new_file.close
    # Return empty MasterClass object
    new_MasterClass = MasterClass(default_calendar, app_version)
    return new_MasterClass


# Saves MasterClass object to .json file
def jh_save_to_file(MasterClass_object, file_name):
    json_string = __encode_obj_to_json(MasterClass_object)
    js_write_to_file(file_name, json_string)
