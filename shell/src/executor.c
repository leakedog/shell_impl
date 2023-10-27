#include "../include/executor.h"

int ExecuteCommandInFork(command* command) {

	if (!command->args) return -1;

    int n_args = 0;
    argseq* start = command->args;
    do {
        n_args++;
        start = start->next;
    } while(start != command->args); // Is it cyclic? Or just line

    char** argv = (char**) malloc(n_args * sizeof(char*));
    
    if (argv == NULL) {
        return ALLOC_FAILED;
    }

    start = command->args;
    int it = 0;

    printf("Execute command with %d args: ", n_args);
    do {
        argv[it++] = start->arg;
        printf("%s ", argv[it -1]);
        start = start->next;
    } while(start != command->args && start);
    printf("\n");

    // for (int i = 0; builtins_table[i].name != NULL; i++) {
    //     if (strncmp(builtins_table[i].name, argv[0], strlen(argv[0])) == 0) {
    //         return (*builtins_table[i].fun)(argv);
    //     }
    // }


    int id = execvp(argv[0], argv);


    printf("%d\n", id);
    free(argv);
    if (id == -1) {
        switch (errno) {
            case ENOENT:
                return ENOENT;
            case EACCES:
                return EACCES;
            default:
                printf("%d\n", errno);
        }
    } else {
        return OK_STATUS;
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
                break;
            case EACCES:
                printf("WTF");
                perror(command->args->arg);
                break;
            case ALLOC_FAILED:
                perror("lsh: Error during allocation");
                exit(EXIT_FAILURE);
                break;
        }
        printf("no error %d\n", errno);
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
    commandseq* start = commands;
    do {
        int status = ExecuteFork(commands->com);
        if (status != OK_STATUS) {
            return status;
        }
        commands = commands->next;
        printf("Next command\n");
    } while(commands != start);
    return OK_STATUS;
}

int Execute(pipelineseq* seq) {
    pipelineseq* start = seq;
    do {
        int status = ExecuteCommands(seq->pipeline->commands);
        seq = seq->next;
    } while(start != seq);

    return OK_STATUS;
}