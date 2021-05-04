from Assessment import Assessment as asmt
from datetime import date, datetime


class Course:
    def __init__(self, course_code, course_title=''):
        self.code = course_code
        self.title = course_title
        self.assessments = []

    def find_assessment_index(self, new_asmt):
        index = 0

        # Goes through list of assessments and returns index for new_asmt
        # Sorts by earliest start date
        for current in self.assessments:
            if new_asmt.s_year == current.s_year:
                if new_asmt.s_month == current.s_month:
                    if new_asmt.s_day == current.s_day:
                        # If hour and minute exist, compare them
                        if not(new_asmt.s_hour is None or current.s_hour is None):
                            if not(new_asmt.s_minute is None or current.s_minute is None):
                                if new_asmt.s_hour == current.s_hour:
                                    if new_asmt.s_minute <= current.s_minute:
                                        # new is either before or at same time as current
                                        return index
                                    else:
                                        # new is after current, check next element
                                        pass

                                elif new_asmt.s_hour < current.s_hour:
                                    # new before current
                                    return index

                                else:
                                    # new is after current, check next element
                                    pass

                            else:
                                # They are None, don't compare them
                                return index
                        else:
                            # They are None, don't compare them
                            return index

                    elif new_asmt.s_day < current.s_day:
                        # new is before current
                        return index

                    else:
                        # new is after current, check next element
                        pass

                elif new_asmt.s_month < current.s_month:
                    # new is before current
                    return index

                else:
                    # new is after current, check next element
                    pass

            elif new_asmt.s_year < current.s_year:
                # new is before current
                return index

            else:
                # new is after current, check next element
                pass

            index = index + 1

        return index

    # Returns a list of assessments as strings
    def get_assessments_s(self, include_course_code):
        assessment_list = []
        for i in self.assessments:
            assessment_list.append(i.get_info_as_string(include_course_code))

        return assessment_list

    def add_asmt(self, title,
                 s_year, s_month, s_day,
                 e_year, e_month, e_day,
                 s_hour=None, s_minute=None,
                 e_hour=None, e_minute=None):
        new_assessment = asmt(self.code, title,
                              s_year, s_month, s_day,
                              e_year, e_month, e_day,
                              s_hour, s_minute,
                              e_hour, e_minute)

        # Only insert events that are from today + future
        todays_date = date.today()
        # Make given values a date to compare to today's day
        asmt_date = date(s_year, s_month, s_day)

        if asmt_date >= todays_date:
            # Valid, add to list
            new_index = self.find_assessment_index(new_assessment)
            self.assessments.insert(new_index, new_assessment)
        else:
            # Past event, do nothing
            pass







