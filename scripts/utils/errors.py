###########################################################################
## Copyright (C) Flowbox, Inc / All Rights Reserved
## Unauthorized copying of this file, via any medium is strictly prohibited
## Proprietary and confidential
## Flowbox Team <contact@flowbox.io>, 2014
###########################################################################

import sys

from utils.colors import print_error


def fatal():
    print_error ("ERROR")
    sys.exit(1)


def handle_out(code):
    if code:
        print_error("ERROR")
        sys.exit(code)

def handle_err(code):
    if code:
        sys.exit(code)