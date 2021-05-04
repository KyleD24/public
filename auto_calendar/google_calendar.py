from __future__ import print_function
from googleapiclient import discovery
import datetime
import os.path
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from googleapiclient.errors import HttpError
from custom_exceptions import CalendarError
import json


# Returns calendarId as a string
def __get_calendar_id(calendar_service, calendar_name):
    try:
        calendar_items = calendar_service.calendarList().list().execute()['items']
    except:
        raise CalendarError("Failed to get list of calendar items. Calendar might not exist.")

    for i in calendar_items:
        if i['summary'] == calendar_name:
            # Calendar found
            return i['id']
        else:
            pass

    # Not in calendar
    raise CalendarError("Calendar does not exist.")


# Returns eventId as a string
def __get_event_id(calendar_service, calendar_id, event_name):
    try:
        event_list = calendar_service.events().list(calendarId=calendar_id).execute()['items']
    except:
        raise CalendarError("Failed to get event list. Calendar may not exist.")

    for i in event_list:
        if i['summary'] == event_name:
            return i['id']
        else:
            pass

    raise CalendarError("Event does not exist.")


# Determines if a specified calendar exists given service + query
# Returns True / False
# Raises HttpError and CalendarError
def gc_calendar_exists(calendar_service, calendar_name):
    try:
        calendar_items = calendar_service.calendarList().list().execute()['items']
    except HttpError as HE:
        raise CalendarError('Calendar API is disabled for this user.\n'
                                'Visit https://console.developers.google.com/apis/api/calendar-json.googleapis.com/overview?project=607984928992'
                                ' to enable it then retry. ')

    for i in calendar_items:
        if i['summary'] == calendar_name:
            # Calendar found
            return True
        else:
            pass

    # Not found
    return False


# Signs in the user, returns service
# Some code taken from https://developers.google.com/calendar/quickstart/python
def gc_initialize(scopes, token_json_file, credentials_json_file, json_cred_dict):
    creds = None
    # The file token.json stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    if os.path.exists(token_json_file):
        try:
            creds = Credentials.from_authorized_user_file(token_json_file, scopes)
        except:
            raise CalendarError("Failed to get credentials from .json file.")

    # If there are no (valid) credentials available, let the user log in.
    if not creds or not creds.valid:
        if creds and creds.expired and creds.refresh_token:
            try:
                creds.refresh(Request())
            except:
                raise CalendarError("Unable to refresh token.")
        else:
            # Make sure credentials.json exists
            if not os.path.exists(credentials_json_file):
                print('Creating credentials.json file...')
                with open(credentials_json_file, 'w') as outfile:
                    outfile.write(json.dumps(json_cred_dict))
                    outfile.close()

            print('Logging in to Google account... (if browser doesn\'t automatically open, '
                  'go to the link below to log in)')
            flow = InstalledAppFlow.from_client_secrets_file(
                credentials_json_file, scopes)
            creds = flow.run_local_server(port=0)
        # Save the credentials for the next run
        with open(token_json_file, 'w') as token:
            token.write(creds.to_json())

    service = discovery.build('calendar', 'v3', credentials=creds)

    return service


# Creates a secondary calendar with the given name
# Returns True if successful
# Raises CalendarError
def gc_create_calendar(calendar_service, calendar_name):
    if gc_calendar_exists(calendar_service, calendar_name):
        # Calendar exists, done
        return True

    else:
        # Calendar doesn't exist, create new one
        calendar_dict = {'summary': calendar_name}
        # Insert new calendar to existing calendars
        try:
            calendar_service.calendars().insert(body=calendar_dict).execute()
            return True
        except:
            raise CalendarError("Failed to create new calendar.")


# Returns a list of events in the given calendar
# Raises CalendarError and HttpError
def gc_get_events(calendar_service, calendar_name):
    try:
        if gc_calendar_exists(calendar_service, calendar_name):
            calendar_list = calendar_service.calendarList().list().execute()['items']

            for i in calendar_list:
                if i['summary'] == calendar_name:
                    calendar_id = i['id']
                else:
                    pass

            event_list = calendar_service.events().list(calendarId=calendar_id).execute()['items']

            return event_list
        else:
            raise CalendarError("Specified calendar does not exist.")
    except CalendarError as CE:
        raise CalendarError(CE.message)


# Creates a calendar given an "event" source
# See https://developers.google.com/calendar/v3/reference/events#resource for event format
# Returns true if successful
def gc_create_event(calendar_service,
                    calendar_name,
                    all_day=False,
                    event={"summary": "Example Event",
                           "end": {"dateTime": "2021-01-01T15:00:00", "timeZone": "Canada/Eastern"},
                           "start": {"dateTime": "2021-01-01T09:00:00", "timeZone": "Canada/Eastern"},
                           "description": "Example event description"}):

    # Safety checks
    if all_day:
        # All-day event, ensure dates are given
        if event["end"]["date"] and event["start"]["date"]:
            # Valid
            pass
        else:
            # Invalid
            raise CalendarError("date not given in new event.")

    else:
        # Specific date and time, ensure dateTime exists
        if event["end"]["dateTime"] and event["start"]["dateTime"]:
            # Valid
            pass
        else:
            # Invalid
            raise CalendarError("dateTime not given in new event.")

    try:
        # Make sure no duplicates are made (based on event name)
        all_events = gc_get_events(calendar_service, calendar_name)
        for i in all_events:
            if i['summary'] == event['summary']:
                # Duplicate event, overwrite old event
                result = gc_delete_event(calendar_service, calendar_name, i['summary'])
                if result:
                    # Event deleted, create new event
                    pass
                else:
                    # Failed to delete event
                    raise CalendarError("Failed to delete event.")
            else:
                pass

        # Insert event
        calendar_service.events().insert(calendarId=__get_calendar_id(calendar_service, calendar_name), body=event).execute()
        return True

    except CalendarError:
        raise CalendarError("Failed to insert new event into " + calendar_name)
        return False


# Attempts to delete an event
# Returns True if successful
def gc_delete_event(calendar_service, calendar_name, event_name):
    # Checks if calendar exists
    try:
        if not gc_calendar_exists(calendar_service, calendar_name):
            raise CalendarError("Calendar does not exist.")
        else:
            cal_id = __get_calendar_id(calendar_service, calendar_name)
            # Check if event exists
            all_events = gc_get_events(calendar_service, calendar_name)
            for i in all_events:
                if i['summary'] == event_name:
                    # Event found, proceed to delete it
                    event_id = __get_event_id(calendar_service, cal_id, event_name)
                    calendar_service.events().delete(calendarId=cal_id, eventId=event_id).execute()
                    # Event deleted successfully
                    return True

                else:
                    pass

            raise CalendarError("Event does not exist.")

    except CalendarError as CE:
        raise CalendarError(CE.message)
