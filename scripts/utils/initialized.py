###########################################################################
## Copyright (C) Flowbox, Inc / All Rights Reserved
## Unauthorized copying of this file, via any medium is strictly prohibited
## Proprietary and confidential
## Flowbox Team <contact@flowbox.io>, 2014
###########################################################################

import os.path
import shutil

from   utils.colors import print_warning


def checkInitialized(path):
    initPath = os.path.join(path,".init")
    return os.path.exists(initPath)

def markInitialized(path):
    initPath = os.path.join(path,".init")
    open(initPath,'a').close()

def handleInitialized(path):
    if os.path.exists(path):
        print_warning("The path '%s' exists but seems corrupted. Deleting." % path)
        shutil.rmtree(path)

def checkAndHandleInitialized(path):
    if not checkInitialized(path):
        handleInitialized(path)
        return False
    return True