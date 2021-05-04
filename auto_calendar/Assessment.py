class Assessment:
    def __init__(self, code, assessment_title,
                 s_year, s_month, s_day,
                 e_year, e_month, e_day,
                 s_hour=None, s_minute=None,
                 e_hour=None, e_minute=None):
        cap_code = code.upper()
        self.course_code = cap_code

        cap_title = assessment_title.upper()
        self.title = cap_title

        # Start date / time
        self.s_year = s_year
        # Cap month value
        if s_month < 0:
            self.s_month = 0
        elif s_month > 12:
            self.s_month = 12
        else:
            self.s_month = s_month
        # Cap day value
        if s_day < 0:
            self.s_day = 0
        elif s_day > 31:
            self.s_day = 31
        else:
            self.s_day = s_day
        # Cap hour values
        if s_hour and s_hour < 0:
            self.s_hour = 0
        elif s_hour and s_hour > 24:
            self.s_hour = 24
        else:
            self.s_hour = s_hour
        # Cap minute values
        if s_minute and s_minute < 0:
            self.s_minute = 0
        elif s_minute and s_hour > 60:
            self.s_minute = 60
        else:
            self.s_minute = s_minute

        # End date / time
        self.e_year = e_year
        # Cap month value
        if e_month < 0:
            self.e_month = 0
        elif e_month > 12:
            self.e_month = 12
        else:
            self.e_month = e_month
        # Cap day value
        if e_day < 0:
            self.s_day = 0
        elif e_day > 31:
            self.e_day = 31
        else:
            self.e_day = e_day
        # Cap hour values
        if e_hour and e_hour < 0:
            self.e_hour = 0
        elif e_hour and e_hour > 24:
            self.e_hour = 24
        else:
            self.e_hour = e_hour
        # Cap minute values
        if e_minute and e_minute < 0:
            self.e_minute = 0
        elif e_minute and e_minute > 60:
            self.e_minute = 60
        else:
            self.e_minute = e_minute

    # Returns all info as a string
    def get_info_as_string(self, include_code):
        string = ""

        if include_code:
            # Add course code
            string = string + self.course_code + " "
        else:
            pass

        # Get other date and time information
        if self.s_hour is None and self.s_minute is None and self.e_hour is None and self.e_minute is None:
            # No time specified, only include days
            date_difference = self.e_day - self.s_day
            if date_difference > 1:
                # Happening over several days
                string = string + self.title + \
                         " || From: " + \
                         str(self.s_year) + "-" + \
                         str(self.s_month) + "-" + \
                         str(self.s_day) + \
                         " || To: " + \
                         str(self.e_year) + "-" + \
                         str(self.e_month) + "-" + \
                         str(self.e_day)
            else:
                # Only happening on one day
                string = string + self.title + \
                         " || ALL DAY: " + \
                         str(self.s_year) + "-" + \
                         str(self.s_month) + "-" + \
                         str(self.s_day)
        else:
            # Time specified, include in info
            string = string + self.title + \
                     " || From: " + \
                     str(self.s_year) + "-" + \
                     str(self.s_month) + "-" + \
                     str(self.s_day) + \
                     " At: " + \
                     str(self.s_hour).zfill(2) + ":" + \
                     str(self.s_minute).zfill(2) + \
                     " || To: " + \
                     str(self.e_year) + "-" + \
                     str(self.e_month) + "-" + \
                     str(self.e_day) + \
                     " At: " + \
                     str(self.e_hour).zfill(2) + ":" + \
                     str(self.e_minute).zfill(2)

        return string

    # Formats object's information into an event for calendar usage
    # Returns event
    def asmt_to_event(self, description_include_flag, timeZone='Canada/Eastern', description=None):
        new_event = {}

        new_event['summary'] = self.course_code + " " + self.title

        # Different key depending on date format
        if self.s_hour and self.s_minute  and self.e_hour and self.e_minute:
            # Use dateTime format
            new_event['start'] = {'dateTime': str(self.s_year) + "-" +
                                              str(self.s_month) + "-" +
                                              str(self.s_day) + "T" +
                                              str(self.s_hour) + ":" +
                                              str(self.s_minute) + ":" + "00",
                                  'timeZone': timeZone}
            new_event['end'] = {'dateTime': str(self.e_year) +
                                              "-" + str(self.e_month) +
                                              "-" + str(self.e_day) + "T" +
                                              str(self.e_hour) + ":" +
                                              str(self.e_minute) + ":" + "00",
                                'timeZone': timeZone}

        else:
            # Use date format
            new_event['start'] = {'date': str(self.s_year) + "-" +
                                          str(self.s_month) + "-" +
                                          str(self.s_day),
                                  'timeZone': timeZone}
            new_event['end'] = {'date': str(self.e_year) + "-" +
                                        str(self.e_month) + "-" +
                                        str(self.e_day),
                                'timeZone': timeZone}

        # Add application include flag
        new_event['description'] = description_include_flag

        # Also add any other description provided
        new_event['description'] = new_event['description'] + " " + description
