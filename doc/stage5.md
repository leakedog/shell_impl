# OS Shell - Phase 5

*Specification for the fifth phase of the project carried out as part of the Operating Systems course exercises.*

## Shell Implementation - Phase 5 (Background Processes and Signal Handling)

1. All commands on a line where the last non-whitespace character is `&` should be executed in the background. The shell process should start them but not wait for their completion. You can assume that if commands in a pipeline are to be run in the background, they do not include shell built-in commands (i.e., all commands can be treated as programs). To check if a pipeline should be executed in the background, use the `LINBACKGROUND` flag in the `flags` field of the `pipeline` structure.

2. The shell process should monitor the statuses of terminated child processes. When operating with a prompt, it should print information about terminated background processes **before displaying the next prompt** in the following format:
```
Background process (pid) terminated. (exited with status (status))
Background process (pid) terminated. (killed by signal (signo))
```


3. Ensure that no zombie processes are left. When operating with a prompt, account for cases where the prompt is not printed for a long time (e.g., `sleep 10 | sleep 10 & sleep 1000000`). A permissible solution is to store only a fixed number of child termination statuses (as a compile-time constant) until the prompt is printed, and forget the statuses of remaining children.

4. CTRL-C sends the `SIGINT` signal to all processes in the foreground process group. Ensure that this signal does not reach background processes. At the same time, background processes should have the default handling of this signal registered.

Syscall checklist: `sigaction`, `sigprocmask`, `sigsuspend`, `setsid`

Example session:
```
$ sleep 20 | sleep 21 | sleep 22 &
$ ps ax | grep sleep
  580   ?  0:00 sleep TERM=xterm
  581   ?  0:00 sleep TERM=xterm
  582   ?  0:00 sleep TERM=xterm
  586  p1  0:00 grep sleep
$
Background process 580 terminated. (exited with status 0)
Background process 581 terminated. (exited with status 0)
Background process 582 terminated. (exited with status 0)
$ sleep 10 | sleep 10 | sleep 10 &
$ sleep 30	# (ten sleep nie powinien być przerwany wcześniej niż po 30s)
Background process 587 terminated. (exited with status 0)
Background process 588 terminated. (exited with status 0)
Background process 589 terminated. (exited with status 0)
$ sleep 10 | sleep 10 | sleep 10 &
$ sleep 1000 # (z innego terminala sprawdź czy nie ma procesów zombie po przynajmniej 10s ale przed zakończeniem sleep 1000)
(CTRL-C) (powinien przerwać sleep 1000)
Background process 591 terminated. (exited with status 0)
Background process 592 terminated. (exited with status 0)
Background process 593 terminated. (exited with status 0)
$ sleep 1000 &
$ sleep 10
(CTRL-C) (powinien przerwać sleep 10 ale nie sleep 1000)
$ ps ax | grep sleep
595   ?  0:00 sleep TERM=xterm
598  p1  0:00 grep sleep
$ /bin/kill -SIGINT 595
Background process 595 terminated. (killed by signal 2)
$ exit
```
