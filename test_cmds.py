#!/usr/bin/env python3

import shlex
import subprocess
import sys
from typing import List, Tuple
from termcolor import colored

CASES = r'''
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
'''


def find_client() -> str:
    """
    Finds the client executable.
    :return: Path to the client executable.
    """
    import os

    # First, check the current working directory
    candidate = os.path.join(os.getcwd(), 'client')
    if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
        return candidate

    # If not found, check the PATH
    for path in os.environ['PATH'].split(os.pathsep):
        path = path.strip('"')
        candidate = os.path.join(path, 'client')
        if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
            raise FileNotFoundError(f'Client not found in the current working directory.\nDid you mean {candidate}?')

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


def run_commands(cmds_: List[str], outputs_: List[str]) -> None:
    """
    Run the commands and compare the output with the expected output.
    :param cmds_: List of str commands to run.
    :param outputs_: List of str expected outputs.
    """
    for cmd, expected in zip(cmds_, outputs_):
        try:
            out = subprocess.check_output(shlex.split(cmd), timeout=5, stderr=subprocess.STDOUT).decode('utf-8')

        except subprocess.TimeoutExpired:
            print(colored(f"Command '", 'red'), end='')
            print(colored(cmd, 'blue', attrs=['bold', 'underline']), end='')
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
            print(colored(f"command '", 'cyan'), colored(cmd, 'yellow', attrs=['bold']), end='')
            print(colored("' failed.", 'cyan'))
            print(colored("Output:", 'blue', attrs=['bold']))
            print(colored(out, 'red'))
            print(colored("Expected:", 'blue', attrs=['bold']))
            print(colored(expected, 'green'))
            print(colored("-" * 40, 'magenta'))


try:
    client = find_client()
    print(colored('Using client:', 'green'), colored(client, 'yellow', attrs=['bold']))
except FileNotFoundError:
    print(colored('Client executable not found.', 'red'), file=sys.stderr)
    exit(1)

cmds, outputs = parse_cases(CASES)
if len(cmds) != len(outputs):
    print(colored('Number of commands and outputs do not match.', 'red'), file=sys.stderr)
    exit(1)

try:
    run_commands(cmds, outputs)
except ConnectionError as err:
    print(colored(err, 'red'), file=sys.stderr)
    exit(1)
except subprocess.CalledProcessError as err:
    error = err.output.decode('utf-8').strip()
    if error:
        print(colored(error, 'red'), file=sys.stderr)
    else:
        print(colored('Command failed with exit status.'), colored(err.returncode, 'blue', attrs=['bold']),
              file=sys.stderr)
    exit(1)

print(colored('All tests passed.', 'green'))
