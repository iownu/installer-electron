#!/usr/bin/python2.7
# coding=utf-8
import argparse
from subprocess import call, PIPE

import psutil
import subprocess

from utils import colors
from utils.process import handle_out


class VAction(argparse.Action):
    def __call__(self, parser, args, values, option_string=None):
        # print 'values: {v!r}'.format(v=values)
        if values is None:
            values = '2'
        try:
            values = int(values)
        except ValueError:
            values = values.count('v') + 1
        setattr(args, self.dest, values)


class Ver(object):
    verbose = 2


def print_error(s, v=None):
    if not v:
        v = Ver.verbose

    if v > 0:
        colors.print_info(s)


def print_warning(s, v=None):
    if not v:
        v = Ver.verbose

    if v > 0:
        colors.print_info(s)


def print_very_important_info(s, v=None):
    if not v:
        v = Ver.verbose

    if v > 1:
        colors.print_info(s)


def print_info(s, v=None):
    if not v:
        v = Ver.verbose

    if v > 2:
        colors.print_info(s)


def info_call(command):
    if Ver.verbose < 2:
        popen_n_wait(command)
    else:
        handle_out(call(command, shell=True))


def set_verbosity(v):
    Ver.verbose = v


def popen_n_wait(args):
    if Ver.verbose > 2:
        print_info('Command used: ' + str(args))
        psutil.Popen(args, shell=True).wait()
    else:
        # TODO [PP]: shell should be set to False, need to change arguments in info_call to array instead of one string
        subprocess.check_call(args, stdout=PIPE, stderr=PIPE, shell=True)
