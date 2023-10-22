#include "../include/executor.h"
#include <asm-generic/errno-base.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int ExecuteCommandInFork(command* command) {
    const char *p, *z, *path = getenv("PATH");
	size_t l, k;
	int seen_eacces = 0;

	if (!*command->args[0].arg) return -1;

    int n_args = 0;
    argseq* start = command->args;
    do {
        n_args++;
        start = start->next;
    } while(start != command->args && start); // Is it cyclic? Or just line

    char** argv = calloc(n_args, sizeof(char*));
    
    if (argv == NULL) {
        return ALLOCFAILED;
    }

    start = command->args;
    
    do {
        argv[0] = start->arg;
        start = start->next;
    } while(start != command->args && start);
    
    execvp(argv[0], argv);

    switch (errno) {
        case ENOENT:
            return ENOENT;
        case EACCES:
            return EACCES;
        default:
            return 0;
    }
}       

int ExecuteFork(command* command) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        switch (ExecuteCommandInFork(command)) {
            case ENOENT:
                perror(command->args->arg);
                perror(": no such file or directory\n");
                break;
            case EACCES:
                perror(command->args->arg);
                perror(": permission denied\n");
                break;
            case ALLOCFAILED:
                perror("lsh: Error during allocation");
                exit(EXIT_FAILURE);
                break;
        }
    } else if (pid < 0) {
        // Error forking
        perror(command->args->arg);
        perror(": exec error\n");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return OK_STATUS;
}

int ExecuteCommands(commandseq* commands) {
    while (commands) {
        int status = ExecuteFork(commands->com);
        if (status != OK_STATUS) {
            return status;
        }
        commands = commands->next;
    }
    return OK_STATUS;
}

int Execute(pipelineseq* seq) {
    for (pipelineseq* cur = seq; cur; cur = cur->next) {
        int status = ExecuteCommands(cur->pipeline->commands);
    }
    return OK_STATUS;
}