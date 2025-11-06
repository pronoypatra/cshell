# Custom Shell Implementation - Complete Documentation

## Overview

This is a custom shell implementation written in C that provides a Unix-like command-line interface with support for built-in commands, background/foreground processes, piping, redirection, job control, command history, and tab completion.

## Project Structure

The project is organized into modular components, with each module handling a specific aspect of shell functionality:

### Core Files

- **shell.c** - Main shell program that orchestrates all functionality
- **Makefile** - Build configuration for compiling all source files

### Module Files

#### Built-in Commands
- **builtin.c / builtin.h** - Implements built-in shell commands (cd, pwd, echo, exit, clear)

#### Process Management
- **foreground.c / foreground.h** - Handles foreground process execution
- **background.c / background.h** - Manages background process execution and cleanup
- **fgbg.c / fgbg.h** - Implements foreground/background job control (fg, bg, sig commands)
- **jobs.c / jobs.h** - Manages and displays background jobs

#### File System Operations
- **ls.c / ls.h** - Custom `ls` command with color coding and formatting
- **discovery.c / discovery.h** - File discovery/search functionality
- **pinfo.c / pinfo.h** - Process information display

#### User Interface
- **prompt.c / prompt.h** - Custom prompt generation with timing information
- **autocomplete.c / autocomplete.h** - Tab completion functionality
- **history.c / history.h** - Command history management

#### Utilities
- **helper.c / helper.h** - Utility functions (tokenization, command routing, constants)

## Constants and Configuration

All magic numbers have been replaced with named constants defined in `helper.h`:

- `MAX_BACKGROUND_JOBS` (100) - Maximum number of background jobs
- `MAX_COMMAND_LENGTH` (100) - Maximum length of a command
- `MAX_PIPED_COMMANDS` (1000) - Maximum number of commands in a pipe
- `MAX_ARGS` (20) - Maximum number of arguments per command
- `MAX_BUFFER_SIZE` (200) - Standard buffer size
- `MAX_PATH_LENGTH` (200) - Maximum path length
- `MAX_DIRS` (10) - Maximum directories in operations
- `HISTORY` (20) - Number of commands stored in history

## Features

### Built-in Commands

#### cd
- Changes the current directory
- Supports `~` for home directory
- Supports `-` for previous directory
- Syntax: `cd [directory]`

#### pwd
- Prints the current working directory
- Syntax: `pwd`

#### echo
- Prints arguments to stdout
- Syntax: `echo [arguments...]`

#### exit
- Exits the shell
- Optional exit code: `exit [code]`
- Default exit code: 0

#### clear
- Clears the terminal screen
- Syntax: `clear`

#### ls
- Lists directory contents with color coding
- Options: `-l` (long format), `-a` (show hidden files)
- Colors: Directories (blue), Executables (green), Files (white)
- Syntax: `ls [options] [directories/files...]`

#### pinfo
- Displays process information
- Shows: PID, process status, memory usage, executable path
- Syntax: `pinfo [pid]` (if no PID given, shows current process)

#### discover
- Recursively searches for files/directories
- Options: `-d` (directories only), `-f` (files only)
- Syntax: `discover [directory] ["filename"] [options]`

#### history
- Displays last 10 commands from history
- Syntax: `history`

#### jobs
- Lists background jobs
- Options: `-r` (running only), `-s` (stopped only)
- Syntax: `jobs [options]`

#### fg
- Brings a background job to foreground
- Syntax: `fg [job_number]`

#### bg
- Resumes a stopped background job
- Syntax: `bg [job_number]`

#### sig
- Sends a signal to a job
- Syntax: `sig [job_number] [signal_number]`

### Advanced Features

#### Piping
- Supports multiple commands connected with `|`
- Maximum 1000 piped commands
- Example: `ls | grep test | wc -l`

#### Redirection
- Output redirection: `command > file` or `command >> file` (append)
- Input redirection: `command < file`
- Can be combined with piping

#### Background Processes
- Run commands in background with `&`
- Maximum 100 background jobs
- Jobs are automatically tracked and cleaned up
- Job completion notifications are displayed

#### Command History
- Stores last 20 commands
- Persisted to `history.txt` file
- Accessible via `history` command
- Automatic saving of non-empty commands

#### Tab Completion
- Press TAB to autocomplete file/directory names
- Shows all matching options if multiple matches
- Automatically completes common prefix

#### Signal Handling
- `Ctrl+C` (SIGINT): Interrupts foreground processes
- `Ctrl+Z` (SIGTSTP): Suspends foreground processes and moves to background
- `Ctrl+D`: Exits the shell

#### Prompt Features
- Displays: `username@hostname:directory>`
- Shows execution time for commands taking > 1 second
- Uses `~` for home directory in prompt
- Color-coded output

## Architecture

### Main Loop (shell.c)

The main execution loop:
1. Sets up signal handlers
2. Displays prompt
3. Reads user input (with raw mode for special keys)
4. Handles tab completion
5. Parses command (handles `;` and `&` separators)
6. Executes commands (built-in or external)
7. Manages piping and redirection
8. Handles background/foreground execution

### Command Execution Flow

1. **Input Parsing**: Tokenizes input by `;` and `&`
2. **Syntax Validation**: Checks for valid command structure
3. **Command Routing**: Determines if command is built-in or external
4. **Built-in Execution**: Direct function call
5. **External Execution**: Fork and exec
6. **Pipe Setup**: Creates pipes between commands
7. **Redirection**: Sets up input/output redirection
8. **Process Management**: Tracks background jobs, waits for foreground

### Memory Management

- All dynamically allocated memory is properly freed
- Background process argument arrays are freed after fork
- Foreground process argument arrays are freed after execution
- No memory leaks in normal operation

### Error Handling

- Standardized error messages using `perror()`
- Input validation (command length limits)
- File operation error checking
- Process creation error handling

## File Descriptions

### shell.c
Main shell implementation containing:
- Global job tracking arrays
- Signal handlers (SIGINT, SIGTSTP, SIGCHLD)
- Main execution loop
- Command parsing and execution
- Pipe and redirection handling
- Background/foreground process management

### builtin.c
Implements all built-in commands:
- `docd()` - Change directory
- `dopwd()` - Print working directory
- `doecho()` - Echo arguments
- `doexit()` - Exit shell
- `doclear()` - Clear screen

### foreground.c
Executes commands in foreground:
- Forks process
- Executes command
- Waits for completion
- Returns execution time
- Handles errors

### background.c
Manages background processes:
- `background()` - Starts background process
- `backgrounddone()` - Signal handler for completed background jobs
- Tracks job numbers and PIDs
- Displays completion notifications

### fgbg.c
Job control commands:
- `fg()` - Bring job to foreground
- `bg()` - Resume stopped job
- `sig()` - Send signal to job

### jobs.c
Job management:
- `dojobs()` - List background jobs
- Sorts jobs by command name
- Filters by status (running/stopped)
- Reads process status from `/proc`

### helper.c
Utility functions:
- `tokenize()` - Tokenizes strings by delimiter
- `issyntaxerror()` - Validates command syntax
- `whichcommand()` - Routes commands to appropriate handler

### prompt.c
Custom prompt generation:
- Gets username and hostname
- Formats directory path (uses `~` for home)
- Adds execution time if > 1 second
- Color-coded output

### history.c
Command history management:
- `gethistoryfirst()` - Loads history from file
- `gethistory()` - Adds command to history
- `dohistory()` - Displays history
- Persists to `history.txt`

### autocomplete.c
Tab completion:
- `autocomplete()` - Handles TAB key
- Scans current directory
- Finds matching files/directories
- Completes common prefix
- Shows all options if multiple matches

### ls.c
Custom ls implementation:
- `dols()` - Main ls function
- `printfile()` - Prints file with permissions
- `printdirectory()` - Prints directory contents
- Color coding based on file type
- Supports `-l` and `-a` options

### discovery.c
File search functionality:
- `dodiscovery()` - Main discovery function
- `dfs()` - Recursive depth-first search
- Supports directory and file filtering
- Color-coded output

### pinfo.c
Process information:
- `dopinfo()` - Displays process info
- Reads from `/proc/[pid]/stat`
- Shows PID, status, memory, executable path
- Handles current process (no PID argument)

## Build Instructions

### Compilation

```bash
make
```

This compiles all source files into a single executable. The Makefile includes:
- `shell.c`
- `foreground.c`
- `background.c`
- `builtin.c`
- `helper.c`
- `history.c`
- `prompt.c`
- `discovery.c`
- `pinfo.c`
- `ls.c`
- `jobs.c`
- `autocomplete.c`
- `fgbg.c`

### Running

```bash
./a.out
```

## Usage Examples

### Basic Commands
```bash
$ pwd
/home/user/project

$ cd ~/Documents
$ ls -la

$ echo "Hello, World!"
Hello, World!
```

### Piping and Redirection
```bash
$ ls | grep .c | wc -l
14

$ ls > filelist.txt
$ cat < filelist.txt

$ echo "test" >> filelist.txt
```

### Background Jobs
```bash
$ sleep 10 &
[1] 12345

$ jobs
[1] Running sleep 10 [12345]

$ fg 1
# Brings job 1 to foreground

$ jobs -s
# Shows only stopped jobs
```

### Process Information
```bash
$ pinfo
pid: 12345
process Stat: S+
memory: 1024
executable Path: /home/user/shell

$ pinfo 12345
# Shows info for specific PID
```

### File Discovery
```bash
$ discover . "*.c"
./shell.c
./helper.c
./builtin.c

$ discover ~/Documents -f
# Shows only files

$ discover ~/Documents -d
# Shows only directories
```

### History
```bash
$ history
ls
cd ~
pwd
echo "test"
# ... (last 10 commands)
```

## Improvements Made

### Code Quality
1. **Constants**: Replaced all magic numbers with named constants
2. **Memory Management**: Fixed memory leaks in background/foreground execution
3. **Error Handling**: Standardized error messages using `perror()`
4. **Input Validation**: Added command length validation

### Bug Fixes
1. **Uninitialized Pointer**: Fixed `int *status` to `int status` in foreground.c
2. **Memory Leaks**: Added proper `free()` calls for allocated memory
3. **Typo**: Fixed "Inavlid sig" to "Invalid sig"

### Security
1. **Input Validation**: Command length checking to prevent buffer overflows
2. **Error Handling**: Proper error checking for system calls

### New Features
1. **exit Command**: Graceful shell exit with optional exit code
2. **clear Command**: Terminal screen clearing

## Limitations

1. Maximum command length: 100 characters
2. Maximum background jobs: 100
3. Maximum piped commands: 1000
4. History size: 20 commands
5. Fixed-size arrays (no dynamic resizing)

## Future Enhancements

Potential improvements:
- Environment variable support
- Command aliasing
- More advanced redirection (stderr, etc.)
- Command substitution with backticks
- More built-in commands
- Configuration file support
- Better error recovery
- Dynamic array resizing

## Notes

- The shell uses raw terminal mode for input handling (tab completion, special keys)
- History is persisted to `history.txt` in the shell's home directory
- Background job tracking uses global arrays (limited to 100 jobs)
- Process status is read from `/proc/[pid]/stat` (Linux-specific)
- Color codes use ANSI escape sequences

## Dependencies

- Standard C library
- POSIX system calls (fork, exec, wait, etc.)
- Linux-specific: `/proc` filesystem for process information
- Terminal control: termios for raw mode

## Author Notes

This shell implementation demonstrates:
- Process management (fork, exec, wait)
- Signal handling
- File I/O and redirection
- Pipe implementation
- Job control
- Terminal I/O (raw mode)
- System programming concepts

