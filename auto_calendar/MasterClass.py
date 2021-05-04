from Course import Course
from custom_exceptions import FunctionInputError


# Class to contain all other sub-classes + info
# Mostly used for easy JSON encoding / decoding
class MasterClass:
    def __init__(self, default_calendar, app_version):
        self.course_list = []
        self.default_calendar = default_calendar
        self.app_version = app_version

    # Returns true if course is in course_list
    def __course_exists(self, code_title, query):
        cap_code_title = code_title.upper()
        for i in self.course_list:
            if cap_code_title == "CODE":
                if i.code == query:
                    return True
                else:
                    pass
            elif cap_code_title == "TITLE":
                if i.title == query:
                    return True
                else:
                    pass
            else:
                raise FunctionInputError("Invalid function input. Input must be \"CODE\" or \"TITLE\"")

        # Not in list
        return False

        # Searches for a course based on course code
        # Returns the course as object

    def __search_by_code(self, course_code):
        cap_code = course_code.upper()

        for i in self.course_list:
            if i.code == cap_code:
                return i
            else:
                pass

        # Searches for a course based on course title
        # Returns the course as object

    def __search_by_title(self, course_title):
        cap_title = course_title.upper()
        for i in self.course_list:
            if i.title == cap_title:
                return i
            else:
                pass

    # Adds course to course list
    # Does nothing if course exists
    def add_course(self, course_code, course_title=''):
        if self.__course_exists("CODE", course_code):
            # Course already exists
            pass
        else:
            new_course = Course(course_code, course_title)
            self.course_list.append(new_course)
            return

    # Attempts to remove course from course list
    # Returns true if successful
    def remove_course(self, course_code):
        index = 0

        if self.__course_exists("CODE", course_code):
            for i in self.course_list:
                if i.code == course_code:
                    self.course_list.pop(index)
                    return True
                else:
                    index = index + 1
        else:
            # Course not found
            return False

    # Deletes all courses / dates etc.
    # Used when downloading from online calendar
    def delete_course_list(self):
        self.course_list = []

    # Checks if course exists, then adds assessment
    # Returns true if successful
    def add_assessment(self, course_code, title,
                       s_year, s_month, s_day,
                       e_year, e_month, e_day,
                       s_hour=None, s_minute=None,
                       e_hour=None, e_minute=None):

        if self.__course_exists("CODE", course_code):
            search = self.__search_by_code(course_code)
            search.add_asmt(title,
                            s_year, s_month, s_day,
                            e_year, e_month, e_day,
                            s_hour, s_minute,
                            e_hour, e_minute)
            return True

        else:
            return False

    # Returns a string containing all course codes + titles
    def get_courses(self):
        course_list = []

        for i in self.course_list:
            course_list.append(i.code + " " + i.title)

        return course_list

    # Returns a list of assessments given a course code
    def get_assessments(self, course_code, include_course_code=False):
        cap_code = course_code.upper()
        if self.__course_exists("CODE", course_code):
            search = self.__search_by_code(cap_code)
            return search.get_assessments_s(include_course_code)
        else:
            empty_list = ["Course does not exist"]
            return empty_list
