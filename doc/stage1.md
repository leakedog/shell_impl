# OS Shell - Phase 1

*Specification for the first phase of the project carried out as part of the Operating Systems course exercises.*

## Shell Implementation - Phase 1

In the first phase, for each subsequent line of input, you need to execute only the first command from the provided list. In Phase 1, we assume that the input is from the console/terminal.

Values for the macros used below are defined in the file `include/config.h`.

Main shell loop:

1. Print the prompt to STDOUT.
2. Read a line from STDIN.
3. Parse the commands from the line (using the provided parser is recommended).
4. If parsing is successful, execute the first parsed command as a program in a child process, provided the command is not empty. The shell process should wait for the child process to finish. Error handling:
    - If the program specified in the command does not exist, the child process should print to STDERR `[program name]: no such file or directory\n`.
    - If the program specified in the command exists but cannot be executed due to insufficient permissions for the current user, the child process should print to STDERR `[program name]: permission denied\n`.
    - In other cases where the program cannot be executed, the child process should print to STDERR `[program name]: exec error\n`.
    - In the above failure cases, the child process should terminate with a failure, returning the value of the macro `EXEC_FAILURE` (e.g., `exit(EXEC_FAILURE)`).

5. If parsing fails, print an error message to STDERR (the value of the macro `SYNTAX_ERROR_STR`), ending with a newline character `\n`.
The main loop should end when EOF is observed on STDIN.

Temporary assumptions:
- Lines come in whole (one `read` from STDIN returns one line).

Requirements:
- The length of the line should not exceed the value of the macro `MAX_LINE_LENGTH`. Lines longer than this value should be treated as syntax errors and should not be passed to the parser.
- Only `read` can be used for reading from standard input.
- When searching for a program to execute, consider directories specified in the `PATH` environment variable (see `execvp`).
- The default prompt is given by the macro `PROMPT_STR`.

Syscall checklist (the following syscalls should appear in the code):
    `read`, `write`, `fork`, `exec`, `wait/waitpid`

Additional:
- Simple functions implemented in the file `utils.c` may be useful for debugging.
- It is expected that defined header and source files, other than `mshell.c`, may be extended/modified in subsequent phases. Therefore, it is best to place your own code in `mshell.c` or in files created by you.

Example session (end the session with CTRL-D which results in EOF on STDIN):
```
>./mshell
$ ls /home
ast bin dude
$ sleep 3
$ ls -las /home
total 6
total 40
8 drwxrwxrwx   5 bin   bin        320 Oct  5 15:13 .
8 drwxr-xr-x  17 root  operator  1408 Oct  5 13:39 ..
8 drwxr-xr-x   2 ast   operator   320 Feb 15  2013 ast
8 drwxr-xr-x   2 bin   operator   320 Feb 15  2013 bin
8 drwxr-xr-x   4 dude  dude      1024 Oct 12 19:37 dude
$
$ cat /etc/version
3.2.1, SVN revision , generated Fri Feb 15 11:34:15 GMT 2013
$ cat /etc/version /etc/version
3.2.1, SVN revision , generated Fri Feb 15 11:34:15 GMT 2013
3.2.1, SVN revision , generated Fri Feb 15 11:34:15 GMT 2013
$ iamthewalrus
iamthewalrus: no such file or directory
$ /etc/passwd
/etc/passwd: permission denied
$ CTRL-D
```
