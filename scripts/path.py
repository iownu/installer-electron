# coding=utf-8
import os
import shutil

from verbosity import print_error
from utils.errors import fatal

import contextlib

__author__ = 'marek_000'


def recreate_dir(path):
    rm_tree_if_present(path)
    os.makedirs(path)

def get_first_dir_in_path(path):
    return next(os.walk(path))[1][0]

def get_first_n_following_dirs_in_path(path, n):  

    dir_path = ''

    for i in xrange(n):
        dir = next(os.walk(os.path.join(path, dir_path)))[1][0]
        dir_path = os.path.join(dir_path, dir)

    return dir_path

def make_dirs_if_needed(path):
    if not os.path.exists(path):
        os.makedirs(path)

def rm_tree_if_present(path):
    if os.path.exists(path):
        shutil.rmtree(path)

def require_file(path, name, argument):
    if not os.path.exists(path):
        print_error('%s path: "%s" could not be found' % (name, path))
        if argument != -1:
            print_error('Consider setting right %s argument and run script again' % argument)
        fatal()

def popen_n_wait(args, consider_to_set_flag):
        if len(args) < 1:
            raise Exception('Popen_n_wait called without path to program witch should be called')

        require_file(args[0], args[0], consider_to_set_flag)

        builderUtils.verbosity.popen_n_wait(args)

@contextlib.contextmanager
def working_directory(path):
    """A context manager which changes the working directory to the given
    path, and then changes it back to its previous value on exit.

    """
    prev_cwd = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev_cwd)
