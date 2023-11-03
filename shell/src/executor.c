#include "../include/executor.h"


char* commands_buffer[MAX_COMMAND_CNT];


char** GetArgvFromCommmand(command* command, int* n_args) {
    if (!command->args) return NULL;
    *n_args = 0;
    argseq* start = command->args;
    do {
        commands_buffer[(*n_args)++] = start->arg;
        start = start->next;
    } while(start != command->args && start);

    commands_buffer[*n_args] = NULL;

    return commands_buffer;
}

int ExecuteCommandInFork(char** argv, int n_args) {
	if (n_args == 0) return OK_STATUS;

    int id = execvp(argv[0], argv);

    if (id == -1) {
        switch (errno) {
            case ENOENT:
                return ENOENT;
            case EACCES:
                return EACCES;
            default:
                return errno;
        }
    } 

    return OK_STATUS;
}       

int ExecuteBuiltinCommand(char** argv, int n_args) {
    if (n_args == 0) {
        return EXIT_FAILURE;
    }
    for (int i = 0; builtins_table[i].name != NULL; i++) {
        if (strncmp(builtins_table[i].name, argv[0], MIN(strlen(argv[0]), strlen(builtins_table[i].name))) == 0) {
            return (*builtins_table[i].fun)(argv);
        }
    }
    return EXIT_FAILURE;
}

int ExecuteCommand(command* command) {
    if (command == NULL) {
        return OK_STATUS;
    }

    int n_args = 0;

    char** argv = GetArgvFromCommmand(command, &n_args);

    if (n_args == 0) {
        return OK_STATUS;
    }

    int status_builtin_execution = ExecuteBuiltinCommand(argv, n_args);
    

    switch (status_builtin_execution) {
        case BUILTIN_ERROR:
            fprintf(stderr, "Builtin %s error.\n", command->args->arg);
            return OK_STATUS;
            break;
        case EXIT_SUCCESS:
            return OK_STATUS;
            break;
    };


    pid_t pid, wpid;
    int status;
    
    pid = fork();

    if (pid == 0) {
        // Child process
        int respond = ExecuteCommandInFork(argv, n_args);
        switch (respond) {
            case ENOENT:
                fprintf(stderr, "%s: no such file or directory\n", command->args->arg);
                exit(EXIT_FAILURE);
                break;
            case EACCES:
                fprintf(stderr, "%s: permission denied\n", command->args->arg);
                exit(EXIT_FAILURE);
                break;
            case OK_STATUS:
                break;
            default:
                fprintf(stderr, "%s: exec error\n", command->args->arg);
                exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        // Error forking
        fprintf(stderr, "%s: exec error\n", command->args->arg);
        exit(EXIT_FAILURE); // Terminate the child process
    } else  {
        // Parent process
        wpid = wait(&status);
        if (wpid == -1) {
            exit(EXIT_FAILURE);
        }
    }

    return OK_STATUS;
}

int ExecuteCommands(commandseq* commands) {
    if (commands == NULL) {
        return OK_STATUS;
    }

    commandseq* start = commands;
    do {
        int status = ExecuteCommand(commands->com);
        if (status != OK_STATUS) {
            return status;
        }
        commands = commands->next;
    } while(commands != start);
    return OK_STATUS;
}

int Execute(pipelineseq* seq) {
    if (seq == NULL) {
        return OK_STATUS;
    }

    pipelineseq* start = seq;
    do {
        int status = ExecuteCommands(seq->pipeline->commands);
        seq = seq->next;
    } while(start != seq);

    return OK_STATUS;
}