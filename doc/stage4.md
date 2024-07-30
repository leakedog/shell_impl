# OS Shell - Phase 4

*Specification for the fourth phase of the project carried out as part of the Operating Systems course exercises.*

## Shell Implementation - Phase 4 (Input/Output Redirection and Piping Commands)

1. Each program launched from the shell may have redirected input and output. Redirections for built-in shell commands can be ignored. The `redirs` list in the `command` structure contains the redirections defined for a given command.

  - Input Redirection: If the `flags` field in the `redir` structure matches the macro `IS_RIN(x)`, the newly launched program should have a file specified by the `filename` field opened on standard input (descriptor `0`).

  - Output Redirection: If the `flags` field in the `redir` structure matches the macro `IS_ROUT(x)` or `IS_RAPPEND(x)`, the newly launched program should have a file specified by the `filename` field opened on standard output (descriptor `1`). Additionally, if the flags match the macro `IS_RAPPEND(x)`, the file should be opened in append mode (`O_APPEND`); otherwise, the file content should be truncated (`O_TRUNC`). If the file to which output is redirected does not exist, it should be created.

  Handle the following errors:
  - File does not exist -> print to stderr: `(filename): no such file or directory\n`,
  - Insufficient access rights -> print to stderr: `(filename): permission denied\n`.

  You can assume that the redirection list for each command contains at most one input redirection and at most one output redirection.

2. Commands on a single line can be connected with pipes `|`. A sequence of such commands is called a pipeline. If the pipeline contains more than one command, assume that none of them is a shell built-in command. Execute all commands in the pipeline, each in a separate child process of the shell. Subsequent commands should be connected by pipes so that the output of the command k goes to the input of the command k+1. The shell should suspend its operation until **all** processes executing the pipeline commands have finished. If a command has defined input/output redirection(s), they take precedence over the pipes.

3. On a single line, multiple pipelines can be defined, separated by `;` (or `&`). They should be executed sequentially, i.e., execute the first, wait until all processes finish, and then execute the second, and so on.

Note: The parser accepts lines containing empty commands. In particular, a line containing `ls | sort ; ls |  | wc` will be correctly parsed. If a line contains a pipeline of at least length 2 with an empty command, such a line should be ignored in its entirety and a syntax error should be declared.

Example session (compare results with what is done, e.g., in bash):
```
$ ls > a
$ ls >> a
$ ls >> b
$ cat < a
$ cat < a > b
$ cat < a >> b
$ ls | sort
...
$ ls | cat | sort | wc
...
$ ls > /dev/null | wc
      0       0       0
$ ls | wc < /dev/null
      0       0       0
$ ls | sleep 10 | ls | sort # shel powinien się zawiesić na 10s.
...
$ ls /etc | grep "a" > a ; ls /home | sort > b
...
$ sleep 5 ; sleep 5; sleep 5; echo yawn # wykonanie powinna trwac 15s.
yawn
```

Syscall checklist: `open`, `close`, `pipe`, `dup/dup2/fcntl`.

Tests have been expanded with a set 3 covering redirections and command piping.
