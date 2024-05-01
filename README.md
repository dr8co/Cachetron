<h1 align="center">Cachetron</h1>
<h2 align="center">A simple in-memory data structure store</h2>

<p align="center">
<img alt="A simple in-memory data structure store" height="512" src="./img/cachetron1-rounded@0.5.png" title="Cachetron" width="512"/>
</p>

## About

Cachetron is a simple in-memory data structure store that can be used as
an application cache or a quick-response database.

It is **a key-value store** that provides access to data structures via
a set of commands that are sent using a client-server model with TCP sockets.

## Table of Contents

* [About](#about)
* [Supported Commands](#supported-commands)
* [Building](#building-cachetron)
* [Running](#running-cachetron)
* [Testing](#testing)
  * [Testing the Server and Client](#testing-the-server-and-the-client)
  * [Testing the Data Structures](#testing-the-data-structures)
* [License](#license)

## Supported Commands

Cachetron supports the following commands:

* `GET <key>`: Get the value of a key.
* `SET <key> <value>`: Set the value of a key.
* `DEL <key>`: Delete a key.
* `KEYS`: Get all keys.
* `ZADD <key> <score> <value>`: Add a value to a sorted set.
* `ZREM <key> <value>`: Remove a value from a sorted set.
* `ZSCORE <key> <value>`: Get the score of a value in a sorted set.
* `ZQUERY <key> <min> <max>`: Get all values in a sorted set with scores between `min` and `max`.
* `EXISTS <key>`: Check if a key exists.
* `EXPIRE <key> <milliseconds>`: Set a key to expire in `milliseconds` milliseconds.
* `TTL <key>`: Get the time-to-live of a key in milliseconds.
* `COMMAND [list]`: Get a list of all commands or their verbose descriptions.
* `SHUTDOWN`: Shutdown the server.

These commands are similar to those provided by Redis, but Cachetron is not
intended to be a drop-in replacement for Redis.

It is a simple project that I created to learn more about networking and data structures in C.

## Building Cachetron

Cachetron uses CMake as its build system, and **it is intended for Linux systems only**.

To build Cachetron, you will need to have the following installed:

* `CMake` 3.27 or later
* `Clang 18` and later or `GCC 13` and later. **C23 support is required**.
* `Ninja` 1.11 or later (optional, but recommended)
* `Python` 3.11 or later (optional, for testing)

To build Cachetron, follow these steps:

```bash
# Clone the repository (or download the source code)
git clone https://github.com/dr8co/Cachetron.git

# Change directory to the project root
cd Cachetron

# Build from the main branch
git checkout main

# Create a build directory
mkdir build

# Configure CMake
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -G Ninja

# Build the server and client binaries
cmake --build build --target server client -j $(nproc)
```

Replace `-G Ninja` with `-G "Unix Makefiles"` if you don't have Ninja installed,
or remove it to use the default generator for your system.\
Other generators may be used as well.

Pass the number of jobs to the `-j` flag to speed up the build process.\
The above `$(nproc)` command will use the number of processors available on your system,
assuming you are on a **Linux system** (Which **is the only target system for this project**).

This builds the project in release mode. To build in debug mode, replace `Release` with `Debug`.

The built binaries will be in the `build` directory.

## Running Cachetron

To run Cachetron, you will need to start the server and connect to it using a client.

To start the server, run it from the `build` directory:

```bash
# Assuming you are still in the project root
./build/server
```

The server will start listening on `localhost:1234` by default.
If the port is already in use, the server will exit with an error.

To change the port, pass it with the `--port` flag:

```bash
./build/server --port 12345
```

The server should be running before you start the client.
Leave it running in the background or in a separate terminal.

Use the client to run the commands:

```bash
# Assuming you are still in the project root
./build/client COMMAND
```

**Make sure both the client and the server are using the same port.**

Like the server, the client will use `localhost:1234` by default.

To change the port, pass it with the `--port` flag:

```bash
./build/client --port 12345 COMMAND
```

Replace `COMMAND` with one of the supported commands listed above.

The server runs indefinitely until you send the `SHUTDOWN` command,
or you stop it using `Ctrl+C` or by sending appropriate signals to the process.

The server prints **`'EOF'`** to the console after executing each command.

**Caveats**:

1. The server does not support multiple clients at the same time.
2. The client currently does not support interactive mode.
Only one command can be sent at a time.
If the port was changed, it must be passed with every command.

## Testing

### Testing the Server and the Client

A [python script](./test_cmds.py) is provided to run tests on the server.

Python 3.11 or later and the `termcolor` package are required to run the tests.

The project uses [Poetry](https://python-poetry.org/ "Poetry") to manage the script's
virtual environment and dependencies.

The [pyproject.toml](./pyproject.toml) file contains the Poetry configuration.\
Use it to install the dependencies and activate the virtual environment:

```bash
# Install poetry
curl -sSL https://install.python-poetry.org | python3 -

# Install dependencies
poetry install

# Activate the virtual environment
poetry shell
```

To run the tests, invoke the script from the same directory as the client:

```bash
# Assuming you are still in the project root
cp test_cmds.py build/
./build/test_cmds.py
```

The script assumes that the server is running on `localhost:1234`.
If not, it will search for the server executable in its
working directory and start it.\
The server path can be passed with `--server` flag:

```bash
./test_cmds.py --server ./build/server
```

If the port is in use, pass a different port with the `--port` flag:

```bash
./test_cmds.py --server ./build/server --port 12345
```

The script also searches for the client executable in the same directory.\
The client path can be passed with `--client` flag:

```bash
./test_cmds.py --client ./build/client
```

The script will run a series of commands and check the output.
Unexpected output will be logged to the console.

To suppress color output, run the script with `--no-color` flag:

```bash
./test_cmds.py --server ./build/server --client ./build/client --no-color
```

Alternatively, you can set the `NO_COLOR` environment variable:

```bash
NO_COLOR=1 ./test_cmds.py --server ./build/server --client ./build/client
```

Remember to deactivate the virtual environment when you are done:

```bash
# Exit the virtual environment
exit
```

**Note**: Running the script is roughly similar to:

```bash
# Running the server in the background
./build/server > /dev/null 2>&1 &

# Running the tests with the client in a loop
# The tests are predefined in the script
for (( i = 0; i < cmds_len; i++ )); do
    output=$(./build/client COMMAND) # The command is different for each iteration
        
    # Log the output if it is unexpected
    if [ "$output" != "${expected_output[$i]}" ]; then
        env echo -e "Test $i failed: Expected:\n '${expected_output[$i]}'\nbut got:\n'$output'"
    fi
done

# The last command stops the server
./build/client SHUTDOWN
```

### Testing the Data Structures

The server uses a set of data structures to store the keys and values.

These data structures are tested using the [Google Test](https://google.github.io/googletest/ "GTest") framework.

The Google Test Suite requires a C++-14 capable compiler.\
Since the project is built with C23 support, a C++-14 capable compiler should be available.

The tests have been configured to run with the CMake build system.

To run the tests, you need to build the test binaries:

```bash
# From the project root,
# Configure CMake if you haven't already
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -G Ninja

# Build all targets
cmake --build build --target all -j $(nproc)
```

After building the project, you can run the tests:

```bash
# Assuming you are still in the project root and you have built the project
cd build && ctest -C Release -j $(nproc)
```

Remember to replace `-C Release` with `-C Debug` if you built the project in debug mode.

Do not try running the tests if you haven't built the test binaries.

## License

Cachetron is licensed under the GNU General Public License v3.0.\
See [LICENSE](./LICENSE) for more information.
