#!/usr/bin/env python3
import os
import sys
import shlex
import atexit
import argparse
import subprocess
from time import sleep
from typing import List, Tuple
from termcolor import colored
from multiprocessing import Process, Value

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
(int) 2
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


def is_server_running(port: int = 1234) -> bool:
    """
    Checks if the server is running.
    :param port: Port number to check.
    :return: True if the server is running, False otherwise.
    """
    import socket

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        return s.connect_ex(('localhost', port)) == 0


server_pid = Value('i', 0)


def start_server(server_path: str, port: int = 1234) -> None:
    """
    Starts the server.
    :param server_path: Path to the server executable.
    :param port: Port number to listen on.
    :return: None
    """

    def run_server() -> None:
        """
        Runs the server.
        :return: None
        """
        global server_pid
        server_process = subprocess.Popen([server_path, "--port", str(port)],
                                          stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        server_pid.value = server_process.pid

    server_process_ = Process(target=run_server)
    server_process_.start()
    sleep(0.5)  # Wait for the server to start


def stop_server() -> None:
    """
    Stops the server.
    :return: None
    """
    if server_pid.value:
        try:
            os.kill(server_pid.value, 9)
        except OSError:
            pass


def find_executable(executable_name: str, executable_path: str = None) -> str:
    """
    Finds an executable by name or path.
    :param executable_name: Name of the executable.
    :param executable_path: Optional path to the executable.
    :return: Absolute path to the executable.
    """
    # Check if the provided path, if any, is valid
    if executable_path is not None:
        if os.path.isfile(executable_path):
            if os.access(executable_path, os.X_OK):
                return os.path.abspath(executable_path)
            else:
                raise PermissionError(f'Provided {executable_name} path {executable_path} is not executable.')
        else:
            raise FileNotFoundError(f'Provided {executable_name} path {executable_path} is not a valid executable.')

    # Check the current working directory
    candidate = os.path.join(os.getcwd(), executable_name)
    if os.path.isfile(candidate):
        if os.access(candidate, os.X_OK):
            return os.path.abspath(candidate)
        else:
            raise PermissionError(f'{executable_name} found in the current working directory is not executable.')

    # If not found, check the PATH
    for path in os.environ['PATH'].split(os.pathsep):
        path = path.strip('"')
        candidate = os.path.join(path, executable_name)
        if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
            raise FileNotFoundError(f'{executable_name} not found in the current working directory.'
                                    f'\nDid you mean {os.path.abspath(candidate)}?')

    raise FileNotFoundError(f'{executable_name} executable not found.')


def find_server(server_path: str = None) -> str:
    """
    Finds the server executable.
    :param server_path: Optional path to the server executable.
    :return: Absolute path to the server executable.
    """
    return find_executable('server', server_path)


def find_client(client_path: str = None) -> str:
    """
    Finds the client executable.
    :param client_path: Optional path to the client executable.
    :return: Absolute path to the client executable.
    """
    return find_executable('client', client_path)


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


def run_commands(cmds_: List[str], outputs_: List[str], port: int = 1234) -> bool:
    """
    Run the commands and compare the output with the expected output.
    :param cmds_: List of str commands to run.
    :param outputs_: List of str expected outputs.
    :param port: Port number to connect to.
    :return: True if all tests pass, False otherwise.
    """
    success = True
    for cmd, expected in zip(cmds_, outputs_):
        cmd = f"{cmd} --port {port}"
        try:
            out = subprocess.check_output(shlex.split(cmd), timeout=5, stderr=subprocess.STDOUT).decode('utf-8')

        except subprocess.TimeoutExpired:
            cmd_no_name = ' '.join(cmd.split()[1:])

            print(colored(f"Command '", 'red'), end='')
            print(colored(cmd_no_name, 'blue', attrs=['bold', 'underline']), end='')
            print(colored("' did not complete within the specified timeout.", 'red'))
            success = False
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


def print_help(executable: str) -> None:
    """
    Prints a help message.
    :param executable: Name of the executable.
    :return: None
    """
    print(colored('Example:', 'cyan'), colored(f'{sys.argv[0]} --{executable} /path/to/{executable}',
                                               'yellow', attrs=['bold']), '\n')
    print(colored(f'Alternatively, place the {executable} executable in the current working directory.', 'cyan'))
    print(colored(f'Ensure the {executable} executable is named', 'cyan'),
          colored(f'{executable}', 'yellow', attrs=['bold', 'reverse']),
          colored(f'and has execute permissions.', 'cyan'), '\n')
    print(colored('For more options, run', 'cyan'), colored(f'{sys.argv[0]} --help', 'yellow', attrs=['bold']))


if __name__ == '__main__':
    # Stop the server when the script exits
    atexit.register(stop_server)

    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Run tests for the client.')
    parser.add_argument('--client', type=str, help='path to the client executable')
    parser.add_argument('--server', type=str, help='path to the server executable')
    parser.add_argument('--port', type=int, default=1234, help='port number to use for communication')
    parser.add_argument('--no-color', action='store_true', help='disable colored output')
    args = parser.parse_args()

    # Port number must be valid
    if not 0 < args.port <= 65535:
        print(colored('Port number must be between 0 and 65535.', 'red'), file=sys.stderr)
        exit(1)

    print(colored('Using port:', 'green'), colored(args.port, 'yellow', attrs=['bold']), '\n')

    # Disable colored output if requested
    if args.no_color:
        os.environ['NO_COLOR'] = '1'

    # The server must be running
    if not is_server_running(args.port):
        try:
            server = find_server(args.server)
            print(colored('Using server:', 'green'), colored(server, 'yellow', attrs=['bold']))
            start_server(server, args.port)

        except TimeoutError as err:
            print(colored(err, 'red'), '\n', file=sys.stderr)
            exit(1)

        except FileNotFoundError as err:
            print(colored('Server is not running.', 'red', attrs=['bold']), file=sys.stderr)
            print(colored(err, 'red'), '\n', file=sys.stderr)
            print(colored('Please start the server before running the tests.', 'cyan'))
            print(colored('You can also provide the path to the server executable using the --server flag.', 'cyan'))
            print_help('server')
            exit(1)

        except PermissionError as err:
            print(colored(err, 'red'), file=sys.stderr)
            print(colored('Ensure the server executable has execute permissions.', 'cyan'))
            exit(1)

        except BaseException as err:
            print(colored(err, 'red'), file=sys.stderr)
            exit(1)

    try:
        client = find_client(args.client)
        print(colored('Using client:', 'green'), colored(client, 'yellow', attrs=['bold']), '\n')
    except FileNotFoundError as err:
        print(colored(err, 'red', attrs=['bold']), '\n', file=sys.stderr)
        print(colored('Please provide the path to the client executable using the --client flag.', 'cyan'))
        print_help('client')
        exit(1)

    except PermissionError as err:
        print(colored(err, 'red'), file=sys.stderr)
        print(colored('Ensure the client executable has execute permissions.', 'cyan'))
        exit(1)

    except BaseException as err:
        print(colored(err, 'red'), file=sys.stderr)
        exit(1)

    # Replace './client' with the actual client path in the CASES string
    formatted_cases = CASES.replace('./client', client)

    cmds, outputs = parse_cases(formatted_cases)
    if len(cmds) != len(outputs):
        print(colored('Number of commands and outputs do not match.', 'red'), file=sys.stderr)
        exit(1)

    try:
        all_tests_passed = run_commands(cmds, outputs, args.port)
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

    except BaseException as err:
        print(colored(err, 'red'), file=sys.stderr)
        exit(1)

    if all_tests_passed:
        print(colored('All tests passed.', 'green'))
    else:
        print(colored('Some tests failed.', 'red'), file=sys.stderr)
        exit(1)
