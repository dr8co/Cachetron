#!/usr/bin/env python3

import shlex
import subprocess
from typing import List, Tuple

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
            print(f"Command '{cmd}' did not complete within the specified timeout.")
            continue

        # Handle non-zero exit status
        except subprocess.CalledProcessError as e:
            out = e.output.decode('utf-8').strip()
            if "Connection refused" in out:
                raise ConnectionError('Connection refused. Is the server running?')
            raise e

        if out != expected:
            print(f'cmd: {cmd}\noutput:\n{out}\nexpected:\n{expected}\n{"-" * 40}')


try:
    client = find_client()
    print(f'Using client: {client}')
except FileNotFoundError:
    print('Client executable not found.')
    exit(1)

cmds, outputs = parse_cases(CASES)
if len(cmds) != len(outputs):
    print('Number of commands and outputs do not match.')
    exit(1)

try:
    run_commands(cmds, outputs)
except ConnectionError as err:
    print(err)
    exit(1)
except subprocess.CalledProcessError as err:
    error = err.output.decode('utf-8').strip()
    if error:
        print(error)
    else:
        print(f'Command failed with exit status {err.returncode}.')
    exit(1)

print('All tests passed.')
