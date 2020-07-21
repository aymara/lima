#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

import sys,argparse
import os


#----------------------------------------------------------------------
# main function
def main(argv):
    # parse command-line arguments
    parser=argparse.ArgumentParser(
        description="Convert codes file to symbolic code file")
    # optional arguments
    parser.add_argument("codesfile",
                        type=argparse.FileType('r',
                                               encoding='utf-8'),
                        help="input file: codes file")
    param=parser.parse_args()

    print('<?xml version=\'1.0\' encoding=\'UTF-8\'?>')
    print('<codes>')

    for code_line in param.codesfile:
        code_line = code_line.strip()
        print('  <code key="{}">'.format(code_line))
        if '-' in code_line:
            micro, fieldsString = code_line.split('-')
            print('    <field key="MICRO" value="{}"/>'.format(micro))
            fields = fieldsString.split('|')
            for field in fields:
                key, value = field.split('=')
                print('    <field key="{}" value="{}"/>'.format(key, value))
        else:
            print('    <field key="MICRO" value="{}"/>'.format(code_line))
        print('  </code>')
    print('</codes>')


main(sys.argv[1:])
