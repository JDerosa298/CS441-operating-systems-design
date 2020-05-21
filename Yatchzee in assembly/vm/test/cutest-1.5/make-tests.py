#!/usr/bin/python
'''
Generates a CuTest test suite and is meant to be a cross-platform replacement
to make-tests.sh.
'''
# pylint: disable=invalid-name

import argparse
import re
import sys
import textwrap


# pylint: disable=too-few-public-methods
class GenTest(object):
    '''Generates a CuTest test suite file from input test files.'''

    def __init__(self, src, function_prefix):
        '''
        Args:
            src ([str]): A list of source test file paths
            function_prefix (str): A regex string to use to scan for functions
                in src files.
        '''

        self._src = src
        self._function_prefix = function_prefix.replace('\\', r'\\\\')

    def _gen_tests_from_file(self, file_name):
        '''Generates a list of test function names from src files
        Args:
            file_name (str): The path to a test source file.

        Returns:
            A list of function name strings.
        '''

        with open(file_name, 'r') as test_file:
            funcs = [
                l.split('(')[0].split()[1] for l in test_file.readlines()
                if re.search(self._function_prefix, l)
            ]
        return funcs

    @staticmethod
    def _write_output(tests, output):
        ''' Generates the full output to out.
        Args:
            tests ([str]): The list of function names.
            output (file): The output file to write to.
        '''

        output.write(
            textwrap.dedent('''
        /* This is auto-generated code. Edit at your own peril. */
        #include <stdio.h>
        #include <stdlib.h>

        #include "CuTest.h"
       ''') + '\n')

        # function prototypes
        for func_name in tests:
            output.write('extern void %s(CuTest *);\n' % func_name)

        output.write(
            textwrap.dedent('''
        int RunAllTests(void)
        {
            CuString *output = CuStringNew();
            CuSuite* suite = CuSuiteNew();
        ''') + '\n')

        # Add functions to suite
        for func_name in tests:
            output.write('    SUITE_ADD_TEST(suite, %s);\n' % func_name)

        output.write(
            textwrap.dedent('''
        CuSuiteRun(suite);
        CuSuiteSummary(suite, output);
        CuSuiteDetails(suite, output);
        printf("%s\\n", output->buffer);
        CuStringDelete(output);
        int failed = suite->failCount;
        CuSuiteDelete(suite);
        return failed;
    }

    int main(void)
    {
        return RunAllTests();
    }
    ''') + '\n')

    def generate(self, output):
        '''Generates the test suite to output.
    Args:
      output (file): The file to write to.
    '''

        tests = []
        for file_name in self._src:
            tests_in_this_file = self._gen_tests_from_file(file_name)
            if tests_in_this_file:
                tests += tests_in_this_file

        GenTest._write_output(tests, output)


def main():
    '''Main entry point for execution.'''

    parser = argparse.ArgumentParser(
        'Generates a test suite for CuTest from a collection of test c files')

    parser.add_argument(
        '-o',
        '--output',
        dest='output',
        help='The output file for the test suite.',)

    parser.add_argument(
        '-p',
        '--prefix',
        help='Use prefix to find test functions in src files',
        default='^void t_.*',)

    parser.add_argument("test_files", help="The source test files", nargs='+')

    args = parser.parse_args()

    gt = GenTest(args.test_files, args.prefix)
    if args.output != None:
        with open(args.output, 'w+') as out:
            gt.generate(out)
    else:
        gt.generate(sys.stdout)


if __name__ == '__main__':
    main()
