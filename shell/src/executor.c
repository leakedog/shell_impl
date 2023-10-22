#include "../include/executor.h"
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

	if (strchr(command->args[0].arg, '/'))
		return execve(command->args[0].arg, argv, __environ);

	if (!path) path = "/usr/local/bin:/bin:/usr/bin";

    const char* file = argv[0];

	k = strnlen(file, MAX_LINE_LENGTH) + 1;
	l = strnlen(path, MAX_LINE_LENGTH) + 1;

	for(p = path; ; p = z) {
		char b[l+k+1];
		z = strchr(p, ':');
		if (z - p >= l) {
			if (!*z++) break;
			continue;
		}
		memcpy(b, p, z - p);
		b[z - p] = '/';
		memcpy(b + (z - p) + (z > p), file, k + 1);

        if (access(b, F_OK) == 0) {
            if (access(b, X_OK)) {
                execve(b, argv, __environ);
                if (errno != EACCES) {
                    return EXIT_FAILURE;
                }
            }
            return NOT_ENOUGH_PERMISSIONS;
        } 

		if (!*z++) break;
	}
    
	return ALLOC_FAILED;
}       

int ExecuteFork(command* command) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (ExecuteCommand(command) == EXIT_FAILURE) {
            perror("lsh: ");
        }
        exit(EXIT_FAILURE);
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
        if (status != OK_STATUS) {
            return status;
        }
    }

    return OK_STATUS;
}