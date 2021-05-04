import sys                                  # Used for exiting program
from json_handling import *                 # File to handle all data storage
from google_calendar import *               # Using Google Calendar
from auto_calendar_help_functions import *  # Helper functions

PROGRAM_PATH = str(pathlib.Path(__file__).parent.absolute())
JSON_FILE_NAME = "data.json"
APP_VERSION = "1.0"
JSON_CRED_DICT = {"installed":
                      {"client_id":"607984928992-0fkrbihfnln9b48r1d1o7jv2atf3n62r.apps.googleusercontent.com",
                       "project_id":"marine-bison-307901",
                       "auth_uri":"https://accounts.google.com/o/oauth2/auth",
                       "token_uri":"https://oauth2.googleapis.com/token",
                       "auth_provider_x509_cert_url":"https://www.googleapis.com/oauth2/v1/certs",
                       "client_secret":"P_d7RuE61xcZPtRQZpaw_jed",
                       "redirect_uris":["urn:ietf:wg:oauth:2.0:oob","http://localhost"]}}

# Calendar API Variables
SCOPES = ['https://www.googleapis.com/auth/calendar']   # Allows reading + writing of calendars
TOKEN_FILE = 'token.json'
CRED_FILE = 'credentials.json'                          # Downloaded from GitHub if not present
DEFAULT_CALENDAR = "Auto Calendar"                      # Default secondary calendar for this app
INCLUDE_FLAG = "AUTO_CAL"                               # Flag in event descriptions to be included


# MAIN LOOP

def main_loop(cal_service, master, json_file_name, include_flag):
    program_running = True
    while program_running:
        user_input = input('Main Menu: ')

        if '--' in user_input:
            user_input = user_input.split('--')
        else:
            user_input = user_input.split(' ')

        # Help command
        if user_input[0] == "help" or user_input[0] == "h":
            # Display help menu
            ac_help_menu(master.app_version)

        # Quit command
        elif user_input[0] == "quit" or user_input[0] == "q" or user_input[0] == 'exit':
            # Save to json and quit
            jh_save_to_file(master, json_file_name)
            program_running = False

        # Change calendar command
        elif user_input[0] == 'change_cal ' or user_input[0] == 'change_cal':
            # Validate input formatting
            valid_input_format = False
            if len(user_input) == 2:
                title_and_arg = user_input[1].split(' ')
                if len(title_and_arg) >= 2:
                    if title_and_arg[0] == 'title':
                        # Good format, proceed
                        valid_input_format = True

                        new_title = ''
                        for i in range(len(title_and_arg[1:])):
                            new_title = new_title + title_and_arg[i+1]
                            if i < len(title_and_arg) - 2:
                                new_title = new_title + ' '

                    else:
                        # Bad
                        pass
                else:
                    # Bad
                    pass
            else:
                # Bad
                pass

            # Execute change_cal command
            if valid_input_format:
                # Appropriate number of arguments, attempt to choose default calendar
                user_change_cal(cal_service, master, new_title, json_file_name)
                # Update master class with new calendar
                master.delete_course_list()
                ac_download_calendar(cal_service, master, include_flag)
                # Continue running
                program_running = True
            else:
                print('Invalid input. \nFormat is: \'change_cal --title <calendar title>\'')
                # Continue running
                program_running = True

        # View command
        elif user_input[0] == 'view ' or user_input[0] == 'view':
            # Validate input
            if len(user_input) > 1:
                # Valid
                user_input.pop(0)
                user_view(cal_service, master, user_input)

                # Continue running
                program_running = True
            else:
                # Not enough args
                print('Not enough arguments.')
                print("Format: \'view <--c, --e> [--course course_code] [--upcoming day/week/fortnight]\'")

                # Continue running
                program_running = True

        # Add command
        elif user_input[0] == 'add ' or user_input[0] == 'add':
            # Validate input
            if len(user_input) > 2:
                # Valid
                user_input.pop(0)
                user_add(cal_service, master, user_input, include_flag)

                # Continue running
                program_running = True
            else:
                # Not enough args
                print('Not enough arguments.')
                print("Format: \'add <--course course_code> <--single [True\\False]>\'")

                # Continue running
                program_running = True

        # All other input
        else:
            # Invalid input, continue running
            print('Unknown command. Separate all arguments with \'--\'')
            program_running = True

    # End of while loop

# MAIN


# Initialize Google Calendar API
try:
    service = gc_initialize(SCOPES, TOKEN_FILE, CRED_FILE, JSON_CRED_DICT)
except CalendarError as CE:
    CE.print_message()
    sys.exit("Failed to initialize Google Calendar service...")

# Load data from JSON file, sets default calendar if config file does not exist
new_master = jh_load_from_file(JSON_FILE_NAME, PROGRAM_PATH, DEFAULT_CALENDAR, APP_VERSION)
# Download online calendar to local machine
downloading = True
while downloading:
    try:
        # Delete old data (from memory, not from json file)
        new_master.delete_course_list()
        print('Loading...')
        # Checks if deault calendar exists
        if not gc_calendar_exists(service, new_master.default_calendar):
            # Does not exist, create new one

            gc_create_calendar(service, new_master.default_calendar)

        # Download events
        downloaded = ac_download_calendar(service, new_master, INCLUDE_FLAG)

        if downloaded:
            # Successful, done
            downloading = False
            # Overwrite old json file
            jh_save_to_file(new_master, JSON_FILE_NAME)
        else:
            raise CalendarError("Failed to download calendar.")

    except CalendarError as CE:
        CE.print_message()
        user_input = input("Would you like to try again? [y\\n]")
        if user_input == 'n' or user_input == 'no':
            # Exit program
            sys.exit("Exiting due to download error...")
        else:
            # Continue looping
            pass


# Welcome message
print("Current calendar: " + new_master.default_calendar)
print("""-------------
USER COMMANDS
-------------
-----------------------------------------------------------------------
change_cal <--title calendar_title>
view <--c, --e> [--course course_code] [--upcoming day/week/fortnight]
add <--course course_code> <--single [True\\False]>
-----------------------------------------------------------------------""")
print('Enter \'help\' to see available commands.')

main_loop(service, new_master, JSON_FILE_NAME, INCLUDE_FLAG)
