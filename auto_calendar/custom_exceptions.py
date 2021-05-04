class FunctionInputError(Exception):
    pass


class CalendarError(Exception):
    def __init__(self, message, fatal=False):
        self.message = message
        self.fatal = fatal

    def print_message(self):
        print(self.message)


class InvalidTitle(Exception):
    def __init__(self, bad_title=None, other=None):
        self.invalid_title = bad_title
        self.other_text = other

    def print_message(self):
        if self.invalid_title is None:
            print("Invalid title." + (self.other_text or ''))
        else:
            print("\"" + self.invalid_title + "\" is an invalid title. " + (self.other_text or ''))


class AddEventError(CalendarError):
    pass