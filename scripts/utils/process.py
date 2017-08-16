###########################################################################
## Copyright (C) Flowbox, Inc / All Rights Reserved
## Unauthorized copying of this file, via any medium is strictly prohibited
## Proprietary and confidential
## Flowbox Team <contact@flowbox.io>, 2014
###########################################################################

import sys
from subprocess import call

from utils.colors      import print_error, formatQuestion


def handle_out(code):
    if code != 0 and code != "":
        print_error("ERROR")
        sys.exit(code)

def autocall(args):
    handle_out(call(args))


def ask(question, silent=False):
    if silent:
        print(formatQuestion("Question override: " + question))
        return True
    else:
        answer = None
        while True:
            ansRaw = raw_input(formatQuestion(question) + " [Y/n] ")
            ans = ansRaw.lower()
            if ans not in ["", "y", "yes", "n", "no"]:
                print_error("'%s' is not a valid answer, please choose beetween YES or NO." % ansRaw)
            else: break
        if ans in ["", "y", "yes"]: return True
        else:                       return False