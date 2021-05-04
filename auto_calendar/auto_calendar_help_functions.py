from json_handling import *
from google_calendar import *
from custom_exceptions import InvalidTitle, CalendarError, AddEventError
from datetime import date, timedelta


# Shows text and options on how to use this application
def ac_help_menu(app_version):
    help_message = """---------------------------------------------------------  HELP MENU  ----------------------------------------------
Notes:
    * type 'quit' to save and quit the program.
    * deleting calendars / events is not supported
    * events will be associated with courses and will have their course codes in the title
    * if changes are made to the calendar in a browser, application must be restarted
    * to 'sign out', delete the token.json and data.json files
-----------------------------------------------------------------------------------------------------------
-------------
USER COMMANDS
-------------
-----------------------------------------------------------------------------------------------------------
change_cal <--title calendar_title>
    * changes your current calendar, allowing you to view / add events to a secondary Google calendar
    * --title is required
    * must separate --title and calendar_title by a space
    * you may have spaces in the calendar_title

view <--c, --e> [--course course_code] [--upcoming day/week/fortnight]
    * must have one of --c or --e flag
        * --c list all calendars in the Google account
        * --e lists all events in the current calendar and can be filtered with the following arguments
    * optional arg: --course lets you specify which course you would like to see events for
        * Expects letters followed by numbers, separated by a space, for example 'ECE 250'
    * optional arg: --upcoming lets you view events within a specific time
        * must be one of 'day', 'week', or 'fortnight' to specify 1, 7, or 14 day periods
    * --course and --upcoming are not exclusive to each other and can be in any order
    
add <--course course_code> <--single [True\\False]>
    * adds events to a specified course, both arguments are required
    * --course expects letters followed by numbers, separated by a space, for example 'ECE 250'
    * --single allows you to input multiple events at a time, or just one
        * Entering 'true' will create only one event, 'false' will prompt you until you enter 'q' or 'quit'
        * In either case, you will be prompted to then input the event information, see below
    
    event information format
        (title)       (description)          (start date)               (end date)*
    <event_title>, <event_description>, <YYYY-MM-DD>[T][HH:MM:SS], <YYYY-MM-DD>[T][HH:MM:SS]
    * * you may exclude end date if the event is all-day and begins + ends on the same day
        * Adding a time in this way will create an event that begins and ends at the same time
    * spaces are allowed, just not in between date/times
    * event_description is required but it may be left blank, as long as there is a comma
    * enter 'q' or 'quit' to finish entering events
    * for date formatting:
        * dates can either be 'all-day' (no time) or have a time associated with them
        * if an event is 'all-day', simply do not put anything after 'YYYY-MM-DD'
        * if an event has a time, you must insert a 'T' directly after the date (no space) follow by the time
        * if end date occurs before the start date, the event will not be created
    * Example: 'example 1 title, example 1 description, 2021-06-10, 2021-06-11'
        * Creates an all-day event that lasts from june 10th to june 11th during 2021
    * Example: 'example 2 title, example 2 description, 2021-06-10T10:00:00, 2021-06-11T15:00:00'
        * Creates an event starting at 10am on June 10th 2021, and ending at 3pm June 11th 2021
    * Example: 'example 3 title, example 3 description, 2021-06-10
        * Creates an all-day event that begins and ends on June 10th 2021
-----------------------------------------------------------------------------------------------------------
-------------
PREREQUISITES
-------------
* The author (Kyle Dyck) must add you to the list of 'testers' since this application is not published
"""
    print(help_message)

    return


# Downloads Google Calendar to memory.
# Returns True if successful
def ac_download_calendar(cal_service, master_class, include_flag):
    # Get list of events found online
    try:
        online_list = gc_get_events(cal_service, master_class.default_calendar)
    except CalendarError as CE:
        raise CalendarError(CE.message)

    auto_calendar_list = []

    # Sorts through events to get all events to be read by this app using include_flag
    for i in online_list:
        # Check if any description at all
        if 'description' in i:
            # Check if event has app flag
            if include_flag in i['description']:
                # Event has include flag, add to list of included events
                auto_calendar_list.append(i)

            else:
                # Ignore this event
                pass
        else:
            # Ignore this event
            pass

    # Goes through auto_calendar_list and populates masterclass object with information
    for i in auto_calendar_list:
        try:
            # Determine which date format is being used
            date = 'date' in i['start'] and 'date' in i['end']
            date_time = 'dateTime' in i['start'] and 'dateTime' in i['end']

            if date or date_time:
                # Initialize hours and minutes, only used with dateTime format
                s_hour = None
                s_minute = None
                e_hour = None
                e_minute = None

                # Get start and end dates
                if date:
                    full_start_date = i['start']['date'].split('-')
                    full_end_date = i['end']['date'].split('-')
                else:
                    # date_time format

                    # Start date parsing
                    full_start_date = i['start']['dateTime'].split('-')
                    day_and_time = full_start_date[2].split('T')
                    full_start_date[2] = day_and_time[0]
                    hour_minute = day_and_time[1].split(':')
                    s_hour = hour_minute[0]
                    s_minute = hour_minute[1]

                    # End date parsing
                    full_end_date = i['end']['dateTime'].split('-')
                    day_and_time = full_end_date[2].split('T')
                    full_end_date[2] = day_and_time[0]
                    hour_minute = day_and_time[1].split(':')
                    e_hour = hour_minute[0]
                    e_minute = hour_minute[1]

                s_year = full_start_date[0]
                s_month = full_start_date[1]
                s_day = full_start_date[2]

                e_year = full_end_date[0]
                e_month = full_end_date[1]
                e_day = full_end_date[2]

            else:
                raise CalendarError("No valid date / time for \"" + i['summary'] + "\"")

            # Parses title (course_code and title)
            # Note: Assumes first two items in title are course code ex ECE 250 ...
            # Must have text following course code
            full_title = i['summary'].split(" ")
            if len(full_title) > 2:
                course_code = full_title[0] + " " + full_title[1]
                event_title = ""
                # Get the rest of the title
                for j in full_title[2:]:
                    event_title = event_title + " " + j

            elif len(full_title) == 2:
                raise InvalidTitle(i['summary'], "Must have text after course code.")
            else:
                raise InvalidTitle(i['summary'],
                                   "Must have course code letters followed by numbers, separated by a space.")

            # Ensures course is added to masterclass list
            master_class.add_course(course_code)
            # Adds assessment to master class, according to date format
            if s_hour and s_minute and e_hour and e_minute:
                # Include hours and minutes
                asmt_added = master_class.add_assessment(course_code, event_title,
                                                         int(s_year), int(s_month), int(s_day),
                                                         int(e_year), int(e_month), int(e_day),
                                                         int(s_hour), int(s_minute),
                                                         int(e_hour), int(e_minute))
            else:
                # Don't include hours and minutes
                asmt_added = master_class.add_assessment(course_code, event_title,
                                                         int(s_year), int(s_month), int(s_day),
                                                         int(e_year), int(e_month), int(e_day))

            if asmt_added:
                # Success, keep going
                pass
            else:
                raise CalendarError("Failed to add assessment to MasterClass")

            # End

        except CalendarError as CE:
            raise CalendarError(CE.message)
        except InvalidTitle as IT:
            IT.print_message()
        finally:
            # Finished downloading successfully
            pass

    return True


# Prints all calendars with numbers beside each
# Returns nothing
def ac_print_cals(cal_service, master_class):
    # Get list of calendars
    calendar_list = cal_service.calendarList().list().execute()['items']

    # Displays current default calendar
    print("Current calendar is: " + master_class.default_calendar)
    # Displays list of calendars to user
    print("Here is the list of available calendars:")

    for i in range(len(calendar_list)):
        print(str(i + 1) + ". " + calendar_list[i]['summary'])

    return


# Prompts user for input and parses into an event object
# Returns event (dictionary) ready to be added to google calendar
# Throws AddEventError if input is invalid
def ac_create_event(include_cal_flag, course_code):
    event = {}
    event_info_splitter = ','

    # Make sure course_code is formatted properly
    temp = course_code.split(' ')
    if len(temp) != 2:
        # Invalid
        raise AddEventError('Course code must be letters followed by numbers, separated by a space.'
                            'For example \'ECE 250\'')

    # Prompt user for event info
    user_input = input('Enter event info: ').split(event_info_splitter)

    # First check if user wishes to quit
    if user_input[0] == 'q' or user_input[0] == 'quit' or user_input[0] == 'd' or user_input[0] == 'done':
        # User wishes to exit, throw "fatal error"
        raise AddEventError('Done.', True)

    # Check input is formatted properly

    if not (len(user_input) == 4 or len(user_input) == 3):
        # Invalid
        raise AddEventError('Invalid input. \nFormat: (title) (description) (start date) (end date)\n'
                            '\'<event_title>' + event_info_splitter +
                            '<event_description>' + event_info_splitter +
                            '<YYYY-MM-DD>[T][HH:MM:SS]' + event_info_splitter +
                            '<YYYY-MM-DD>[T][HH:MM:SS]\''
                            '\nIf all-day, do not include \'T\' and time, only YYYY-MM-DD for both start and end.')

    one_day = False
    if len(user_input) == 3:
        # Assumed all-day event, lasting one day
        one_day = True

    # Variables to populate for adding event
    new_title = ''
    new_desc = include_cal_flag + ' ' + str(user_input[1].strip())
    new_start = ''
    new_end = ''

    # Check if title is empty
    title_bool = bool(user_input[0].strip())
    if not title_bool:
        # Invalid
        raise AddEventError('Invalid title format, cannot be blank')

    new_title = course_code + ' ' + str(user_input[0].strip())
    input_index = 2
    # Start and end dates must be the same format
    start_all_day = False
    end_all_day = False
    # Check start and end date, should be same format
    for i in range(2):
        # Repeats same thing if one_day
        if not one_day:
            temp_date = user_input[input_index]
        else:
            temp_date = user_input[2]

        # Check date formatting
        temp_date = temp_date.split('-')
        # Checks correct number of arguments
        if len(temp_date) != 3:
            # Invalid
            raise AddEventError('Invalid date format, must be \'YYYY-MM-DD\' with optional time \'THH:MM:SS\' ')
        # Cuts off anything extra in last index
        temp_date[2] = temp_date[2][0] + temp_date[2][1]

        # Removes any leading spaces
        temp_date[0] = temp_date[0].strip()

        # Check if dates given are numbers
        for j in temp_date:
            try:
                int(j)
            except ValueError:
                raise AddEventError('Invalid date format. Dates must be integers')

        if len(temp_date[0]) != 4 or int(temp_date[0]) < 0:
            # Invalid year format
            raise AddEventError('Invalid year format, must be \'YYYY\' with NO spaces')

        if len(temp_date[1]) != 2 or int(temp_date[1]) < 0 or int(temp_date[1]) > 12:
            # Invalid month format
            raise AddEventError('Invalid month format, must be \'MM\'')

        if len(temp_date[2]) != 2 or int(temp_date[2]) < 0 or int(temp_date[2]) > 31:
            # Invalid day format
            raise AddEventError('Invalid day format, must be \'DD\'')

        # Now check time
        # Repeats same thing if one_day
        if not one_day:
            temp_date = user_input[input_index]
        else:
            temp_date = user_input[2]
        if 'T' in temp_date:
            if input_index == 2:
                # Start
                start_all_day = False
            else:
                # End
                end_all_day = False

            # Split by time character
            temp_date = temp_date.split('T')
            if len(temp_date) != 2:
                # Invalid
                raise AddEventError('Invalid time format, must be \'<YYYY-MM-DD>[T][HH:MM:SS]\' with NO spaces')

            # Only look at time portion 'HH:MM:SS'
            temp_date = temp_date[1]
            temp_date = temp_date.split(':')

            if len(temp_date) != 3:
                # Invalid
                raise AddEventError('Invalid time format, must be \'HH:MM:SS\'')

            if len(temp_date[0]) != 2 or int(temp_date[0]) < 0 or int(temp_date[0]) > 23:
                # Invalid
                raise AddEventError('Invalid hour format, must be \'HH\' and must be between 0 and 24')

            if len(temp_date[1]) != 2 or int(temp_date[1]) < 0 or int(temp_date[1]) > 59:
                # Invalid
                raise AddEventError('Invalid minute format, must be \'MM\' and must be between 0 and 60')

            if len(temp_date[2]) != 2 or int(temp_date[2]) < 0 or int(temp_date[2]) > 59:
                # Invalid
                raise AddEventError('Invalid second format, must be \'SS\' and must be between 0 and 60')

        else:
            if input_index == 2:
                # Start
                start_all_day = True
            else:
                # End
                end_all_day = True

        # Survived all the checks, set values
        if input_index == 2:
            # Start
            new_start = user_input[input_index].strip()
        else:
            # End
            # Repeats same thing if one_day
            if not one_day:
                new_end = user_input[input_index].strip()
            else:
                new_end = user_input[2].strip()

        # Increment for end date
        input_index = input_index + 1

    # At end, both should match
    if start_all_day ^ end_all_day:
        # They are different, raise error
        raise AddEventError('Start and end date formats must be the same. Either include time in both or in neither.')

    # Finally, we have them all

    # Create event with variables
    if start_all_day:
        # Use date format
        event = {"summary": new_title,
                 "end": {"date": new_end, "timeZone": "Canada/Eastern"},
                 "start": {"date": new_start, "timeZone": "Canada/Eastern"},
                 "description": new_desc}

    else:
        # Use dateTime format
        event = {"summary": new_title,
                 "end": {"dateTime": new_end, "timeZone": "Canada/Eastern"},
                 "start": {"dateTime": new_start, "timeZone": "Canada/Eastern"},
                 "description": new_desc}

    return event


# USER FUNCTIONS


# Changes current / default calendar to user input
# Calendar must exist, otherwise no change occurs
# Returns nothing
def user_change_cal(cal_service, master_class, new_title, json_file_name):
    if not gc_calendar_exists(cal_service, new_title):
        # Calendar doesn't exist, create it
        print("Creating new calendar...")
        gc_create_calendar(cal_service, new_title)

    # Change default calendar to given calendar
    master_class.default_calendar = new_title
    # Save settings
    jh_save_to_file(master_class, json_file_name)
    print("Default calendar changed to " + master_class.default_calendar)
    return


# Shows different information depending on parameters
# Returns nothing
def user_view(cal_service, master_class, args_list):
    # Make sure valid calendar/event flag
    if args_list[0] == "c " or args_list[0] == "e " or args_list[0] == "c" or args_list[0] == "e":
        # Valid, check if calendar flag is used
        if args_list[0] == 'c ' or args_list[0] == 'c':
            # Display all calendars, ignore everything else
            calendar_list = cal_service.calendarList().list().execute()['items']
            print('List of calendars: ')
            for i in calendar_list:
                print(i['summary'])

            return
        else:
            # Event flag, check if args need to be adjusted
            if len(args_list) > 1:
                # Parse args, splitting by a space
                args_list = args_list[1:]
                temp_list = args_list
                args_list = []
                for i in temp_list:
                    temp_argument = i.split()
                    for j in temp_argument:
                        args_list.append(j)
            # Check if there are any courses / events at all
            if len(master_class.course_list) > 0:
                # Something in course_list
                # List of events to print
                event_list = []
                # Flag for checking if course / upcoming filters are given
                course_filter = False
                upcoming_filter = False

                # Check if course filter is given
                if 'course' in args_list:
                    # Filter given, create list of events based on course
                    course_filter = True

                    # Get argument after --course for given course
                    index = 0
                    searching = True
                    while searching:
                        if args_list[index] == 'course':
                            # Argument is in next spot
                            index = index + 1
                            searching = False
                        else:
                            # Keep searching
                            index = index + 1

                    # Make sure index is valid
                    if index <= len(args_list) - 2:
                        # Valid
                        course_arg = args_list[index] + ' ' + args_list[index + 1]

                        # Make sure course exists
                        course_exists = False
                        for i in master_class.course_list:
                            if course_arg == i.code:
                                # Course found
                                course_exists = True
                            else:
                                pass

                        if course_exists:
                            # Get list of events for given course
                            event_list = master_class.get_assessments(course_arg)

                        else:
                            # Course doesn't exist
                            print('No events exist for given course.')
                            print('List of courses with events: ')
                            for i in master_class.course_list:
                                print(i.code)

                            return
                    else:
                        # Invalid
                        print('Missing argument after \'--course\'. Looking for course code, example: '
                              '\'ECE 250\'')
                        return

                else:
                    # No course filter, generate list of all events
                    event_list = []
                    for i in master_class.course_list:
                        # Loop through each course
                        # Returns list of course information as a string
                        course_asmt_list = master_class.get_assessments(i.code, True)

                        for j in course_asmt_list:
                            event_list.append(j)

                # Check if upcoming filter is given
                if 'upcoming' in args_list:
                    upcoming_filter = True
                    # Get today's date
                    current_date = date.today()
                    # Find argument after --upcoming
                    index = args_list.index('upcoming')

                    # Check if argument exists after 'upcoming'
                    if len(args_list) <= index:
                        # Missing argument after 'upcoming'
                        print("Missing argument after \'--upcoming\'.")
                        return

                    # Second check for arg after '--upcoming'
                    if args_list[index + 1] == 'course':
                        print("Missing argument after \'--upcoming\'.")
                        return

                    upcoming_arg = args_list[index + 1]

                    # Check if argument is valid
                    if not (upcoming_arg == 'day' or upcoming_arg == 'week' or upcoming_arg == 'fortnight'):
                        # Upcoming argument must be one of these
                        print('Invalid argument after \'--upcoming\'. '
                              'Must be one of \'day\', \'week\', or \'fortnight\'.')
                        return

                    # Get delta to subtract based on given time window
                    if upcoming_arg == 'day':
                        # delta = 1
                        given_date_delta = timedelta(1)
                    elif upcoming_arg == 'week':
                        # delta = 7
                        given_date_delta = timedelta(7)
                    else:
                        # fortnight, delta = 14
                        given_date_delta = timedelta(14)

                    # Go through current event_list and only keep events within specified timeframe
                    for i in event_list:
                        # Extract date from event
                        # Example format: ' TEST 3 || From: 2021-3-18 At: 19:30 || To: 2021-3-18 At: 20:30'
                        temp = i.split('||')
                        if not (len(temp) == 3 or len(temp) == 2):
                            # Shouldn't happen
                            print('Error extracting date from event.')
                            return

                        # Get start date section
                        temp = temp[1]
                        temp = temp.split(' ')
                        # Check if all-day event or not
                        if len(temp) == 6:
                            # Get date itself (not including time)
                            temp = temp[2]
                        else:
                            # Same, but different position
                            temp = temp[3]
                        # Split year, month, day
                        temp = temp.split('-')
                        # Make date from temp to compare them
                        event_date = date(int(temp[0]), int(temp[1]), int(temp[2]))

                        # Get actual difference between today and the event
                        actual_date_difference = event_date - current_date

                        # Check if event is within specified time frame
                        if actual_date_difference <= given_date_delta:
                            # Event is within specified window, keep it
                            pass
                        else:
                            # Event is outside specified window, remove it
                            event_list.remove(i)

                else:
                    # No filter, do not modify event_list
                    pass

                # Show different outputs depending on used filters
                # Note: will only show events starting from today, not past events
                if course_filter:
                    if upcoming_filter:
                        print("Listing events for " + course_arg + " within one " + upcoming_arg + ":")
                    else:
                        print("Listing events for " + course_arg + ":")
                else:
                    if upcoming_filter:
                        print("Listing all events within one " + upcoming_arg + ':')
                    else:
                        print("Listing all events: ")

                # Print all events in generated list
                if len(event_list) > 0:
                    for i in event_list:
                        print(i)
                else:
                    # No current / future events
                    print('Found 0 events occurring today or in the future.')

                # Successful
                return

            else:
                # No courses -> no events
                print('No current / future events found in current calendar, use the \'add\' '
                      'command to begin creating events.')
                return

    else:
        # Invalid
        print("Must have calendar \'--c\' or event \'--e\' flag in arguments. Arguments are separated by \'--\'")
        return


# Adds events to the calendar
# Returns nothing
def user_add(cal_service, master_class, args_list, include_flag):
    # Make sure enough args
    if len(args_list) < 2:
        # Invalid
        print('Not enough arguments.')
        return

    # Make sure --course flag is included
    if 'course' in args_list[0] or 'course' in args_list[1]:
        # Split args into flags and arguments
        temp_args = []
        for i in args_list:
            temp = i.split(' ')
            for j in temp:
                temp_args.append(j)

        # Get course argument
        # Make sure course argument is valid
        index = temp_args.index('course') + 1
        if len(temp_args) <= index + 1:
            # Missing argument
            print('Missing argument after \'--course\' ')
            return
        # Second check
        if temp_args[index] == 'single ':
            # Missing argument
            print('Missing argument after \'--course\' ')
            return

        # Extract course argument
        course_arg = temp_args[index] + ' ' + temp_args[index + 1]

        # Make sure --single flag is included
        if 'single' in args_list[0] or 'single' in args_list[1]:
            # Good

            # Check whether single event or not
            # Make sure single argument is valid
            index = temp_args.index('single') + 1
            if len(temp_args) <= index:
                # Missing argument
                print('Missing argument after \'--single\' ')
                return
            # Second check
            if temp_args[index] == 'course ':
                # Missing argument
                print('Missing argument after \'--single\' ')
                return

            # Extract single argument
            single_arg = temp_args[index]

            if single_arg.capitalize() == 'True':
                single_flag = True
            elif single_arg.capitalize() == 'False':
                single_flag = False
            else:
                # Invalid argument format
                print('Invalid argument after \'--single\'. Must be \'True\' or \'False\'.')
                return

            if single_flag:
                # Only create one event
                looping = False
            else:
                # Create multiple events
                looping = True

            # Create a single event
            try:
                print('Generic Format: \n(title) (description) (start date) (end date)')
                print('<event_title>, <event_description>, <YYYY-MM-DD>[T][HH:MM:SS], <YYYY-MM-DD>[T][HH:MM:SS]')
                new_event = ac_create_event(include_flag, course_arg)

                # Determine if all-day or not
                all_day = False
                if "date" in new_event["start"]:
                    all_day = True
                try:
                    gc_create_event(cal_service, master_class.default_calendar, all_day, new_event)

                    if not looping:
                        print('Event added, you are now at the main menu.')

                except:
                    # General catch
                    print('Google failed to create new event.')
                    pass

            except AddEventError as AE:
                AE.print_message()

            # Multiple events
            while looping:
                try:
                    new_event = ac_create_event(include_flag, course_arg)

                    # Determine if all-day or not
                    all_day = False
                    if "date" in new_event["start"]:
                        all_day = True
                    try:
                        gc_create_event(cal_service, master_class.default_calendar, all_day, new_event)
                    except:
                        # General catch
                        print('Google failed to create new event.')
                        pass

                except AddEventError as AE:
                    AE.print_message()
                    if AE.fatal:
                        # Exit command, means user is finished
                        looping = False
                        print('Events added, you are now at the main menu.')

            # Done
            return

        else:
            print('Must have \'--single\' flag in arguments.')
            return

    else:
        print('Must have \'--course\' flag in arguments.')
        return
