# OS Shell - Phase 3

*Specification for the third phase of the project carried out as part of the Operating Systems course exercises.*

## Shell Implementation - Phase 3 (Built-in Shell Commands)

Before executing a command as a program, check if it is a shell built-in command. Shell built-in commands are located in the `builtins_table` in the file `builtins.c`. Each entry consists of a command name and a pointer to the function that implements the command. The function argument is an array of arguments analogous to `argv`. You need to implement the following commands:

- `exit` - Terminates the shell process.
- `lcd [path]` - Changes the current directory to the specified path. If the command is invoked without an argument, the directory should be changed to the value of the `HOME` environment variable.
- `lkill [ -signal_number ] pid` - Sends the signal `signal_number` to the process/group `pid`. The default signal number is `SIGTERM`.
- `lls` - Lists the names of files in the current directory (similar to `ls -1` with no options). The directory contents should be printed in the order returned by the system. Names starting with `.` should be ignored.

In case of any problems executing the command (e.g., incorrect number or format of arguments, syscall failure, etc.), print the following to STDERR:
```
Builtin 'command_name' error.
```


Session example:
```
sleep 10000 &
sleep 10001 &
./mshell
$ pwd
/home/dude
$ lcd /etc
$ pwd
/etc
$ lcd ../home
$ pwd
/home
$ lls
bin
ast
dude
$ lcd /
$ lls
usr
boot.cfg
bin
boot
boot_monitor
dev
etc
home
lib
libexec
mnt
proc
root
sbin
sys
tmp
var
$ lcd
$ pwd
/home/dude
$ cd ../../usr/src/etc
$ pwd
/usr/src/etc
$ ps
 PID TTY  TIME CMD
...
 2194  p1  0:00 sleep TERM=xterm
 2195  p1  0:00 sleep TERM=xterm
...
$ lkill -15 2194
$ ps
  PID TTY  TIME CMD
...
 2195  p1  0:00 sleep TERM=xterm
...
$ lkill 2195
$ ps
  PID TTY  TIME CMD
  ...
$ lcd /CraneJacksonsFountainStreetTheatre
Builtin lcd error.
$ lkill
Builtin lkill error.
$ exit
```
Syscall checklist: `exit`, `chdir`, `kill`, `readdir`, `opendir/fdopendir`, `closedir`.

Testy zostały rozbudowane o zestaw 2 obejmujący wywoływanie poleceń shella.
