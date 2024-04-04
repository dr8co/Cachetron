#!/usr/bin/env python3

import shlex
import subprocess
import sys
import argparse
from typing import List, Tuple
from termcolor import colored

CASES = r'''
$ ./client asdf
(err) 1 Unknown cmd
$ ./client get asdf
(nil)
$ ./client set k v
(nil)
$ ./client get k
(str) v
$ ./client keys
(arr) len=1
(str) k
(arr) end
$ ./client set k2 v2
(nil)
$ ./client exists k
(int) 1
$ ./client exists k k2 asdf k k2
(int) 23
$ ./client del k
(int) 1
$ ./client del k2
(int) 1
$ ./client del k
(int) 0
$ ./client keys
(arr) len=0
(arr) end
$ ./client exists k
(int) 0
$ ./client zscore asdf n1
(nil)
$ ./client zquery xxx 1 asdf 1 10
(arr) len=0
(arr) end
$ ./client zadd zset 1 n1
(int) 1
$ ./client zadd zset 2 n2
(int) 1
$ ./client zadd zset 1.1 n1
(int) 0
$ ./client zscore zset n1
(dbl) 1.1
$ ./client zquery zset 1 "" 0 10
(arr) len=4
(str) n1
(dbl) 1.1
(str) n2
(dbl) 2
(arr) end
$ ./client zquery zset 1.1 "" 1 10
(arr) len=2
(str) n2
(dbl) 2
(arr) end
$ ./client zquery zset 1.1 "" 2 10
(arr) len=0
(arr) end
$ ./client zrem zset adsf
(int) 0
$ ./client zrem zset n1
(int) 1
$ ./client zquery zset 1 "" 0 10
(arr) len=2
(str) n2
(dbl) 2
(arr) end
$ ./client shutdown
(str) Server is shutting down...
'''


def find_client(client_path: str = None) -> str:
    """
    Finds the client executable.
    :param client_path: Optional path to the client executable.
    :return: Absolute path to the client executable.
    """
    import os

    # Check if the provided path, if any, is valid
    if client_path is not None:
        if os.path.isfile(client_path):
            if os.access(client_path, os.X_OK):
                return os.path.abspath(client_path)
            else:
                raise PermissionError(f'Provided client path {client_path} is not executable.')
        else:
            raise FileNotFoundError(f'Provided client path {client_path} is not a valid executable.')

    # Check the current working directory
    candidate = os.path.join(os.getcwd(), 'client')
    if os.path.isfile(candidate):
        if os.access(candidate, os.X_OK):
            return os.path.abspath(candidate)
        else:
            raise PermissionError('Client found in the current working directory is not executable.')

    # If not found, check the PATH
    for path in os.environ['PATH'].split(os.pathsep):
        path = path.strip('"')
        candidate = os.path.join(path, 'client')
        if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
            raise FileNotFoundError('Client not found in the current working directory.'
                                    f'\nDid you mean {os.path.abspath(candidate)}?')

    raise FileNotFoundError('Client executable not found.')


def parse_cases(cases: str) -> Tuple[List[str], List[str]]:
    """
    Parses the test cases into commands and expected outputs.
    :param cases: String containing the test cases.
    :return: Tuple of lists of str commands and expected outputs.
    """
    cmds_, outputs_ = [], []
    for line in cases.splitlines():
        line = line.strip()
        if line.startswith('$ '):  # Command
            cmds_.append(line[2:])
            outputs_.append('')
        elif line:  # Output
            outputs_[-1] += line + '\n'
    return cmds_, outputs_


def run_commands(cmds_: List[str], outputs_: List[str]) -> bool:
    """
    Run the commands and compare the output with the expected output.
    :param cmds_: List of str commands to run.
    :param outputs_: List of str expected outputs.
    :return: True if all tests pass, False otherwise.
    """
    success = True
    for cmd, expected in zip(cmds_, outputs_):
        try:
            out = subprocess.check_output(shlex.split(cmd), timeout=5, stderr=subprocess.STDOUT).decode('utf-8')

        except subprocess.TimeoutExpired:
            cmd_no_name = ' '.join(cmd.split()[1:])

            print(colored(f"Command '", 'red'), end='')
            print(colored(cmd_no_name, 'blue', attrs=['bold', 'underline']), end='')
            print(colored("' did not complete within the specified timeout.", 'red'))
            continue

        # Handle non-zero exit status
        except subprocess.CalledProcessError as e:
            out = e.output.decode('utf-8').strip()
            if "Connection refused" in out:
                raise ConnectionError('Connection refused. Is the server running?')
            if "Connection reset by peer" in out:
                raise ConnectionError('Connection reset by peer. The server may have exited unexpectedly.')
            raise e

        if out != expected:
            cmd_no_name = ' '.join(cmd.split()[1:])

            print(colored(f"command '", 'cyan'), colored(cmd_no_name, 'yellow', attrs=['bold']), sep='', end='')
            print(colored("' failed.", 'cyan'), '\n')
            print(colored("Output:", 'blue', attrs=['bold']))
            print(colored(out, 'red'))

            print(colored("Expected:", 'blue', attrs=['bold']))
            print(colored(expected, 'green'), '\n', colored("-" * 60, 'magenta'), sep='')
            success = False

    return success


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run tests for the client.')
    parser.add_argument('-c', '--client', type=str, help='Path to the client executable.')
    args = parser.parse_args()

    try:
        client = find_client(args.client)
        print(colored('Using client:', 'green'), colored(client, 'yellow', attrs=['bold']), '\n')
    except FileNotFoundError as err:
        print(colored(err, 'red', attrs=['bold']), file=sys.stderr)
        print(colored('Please provide the path to the client executable using the --client flag.', 'cyan'))
        print(colored('Example:', 'cyan'), colored(f'{sys.argv[0]} --client /path/to/client', 'yellow', attrs=['bold']))
        print(colored('Alternatively, place the client executable in the current working directory.', 'cyan'))
        print(colored('Ensure the client executable is named', 'cyan'), colored('client', 'yellow',
                                                                                attrs=['bold', 'reverse']))
        print(colored('For more options, run', 'cyan'), colored(f'{sys.argv[0]} --help', 'yellow', attrs=['bold']))
        exit(1)
    except PermissionError as err:
        print(colored(err, 'red'), file=sys.stderr)
        print(colored('Ensure the client executable has execute permissions.', 'cyan'))
        exit(1)

    # Replace './client' with the actual client path in the CASES string
    formatted_cases = CASES.replace('./client', client)

    cmds, outputs = parse_cases(formatted_cases)
    if len(cmds) != len(outputs):
        print(colored('Number of commands and outputs do not match.', 'red'), file=sys.stderr)
        exit(1)

    try:
        all_tests_passed = run_commands(cmds, outputs)
    except ConnectionError as err:
        print(colored(err, 'red'), file=sys.stderr)
        exit(1)
    except subprocess.CalledProcessError as err:
        error = err.output.decode('utf-8').strip()
        if error:
            print(colored(error, 'red'), file=sys.stderr)
        else:
            print(colored('Command failed with exit status', 'red'),
                  colored(err.returncode, 'blue', attrs=['bold']), file=sys.stderr)
        exit(1)

    if all_tests_passed:
        print(colored('All tests passed.', 'green'))
    else:
        print(colored('Some tests failed.', 'red'), file=sys.stderr)
        exit(1)
