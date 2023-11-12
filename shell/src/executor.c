#include "../include/executor.h"
#include <stdlib.h>


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

int ExecuteCommand(command* command, int pos, int fd[2], int prevfd[2]) {
    if (command == NULL) {
        return OK_STATUS;
    }
    
    if (NOT_LAST_CMD(pos)) {
        if (pipe(fd) == -1) {
            fprintf(stderr, "lsh: Error creating pipe\n");    
            exit(EXIT_FAILURE);
        }
    }
    
    int n_args = 0;

    char** argv = GetArgvFromCommmand(command, &n_args);

    if (n_args == 0) {
        return OK_STATUS;
    }

    int rin_file = -1, rout_file = -1;
    int original_stdin = -1;
    int original_stdout = -1;
    if (RunRedirs(command, &rin_file, &rout_file) == REDIR_ERROR) {
        close(rin_file);
        close(rout_file);
        return EXIT_FAILURE;
    } else {
        if (rin_file != -1) {
            original_stdin = dup(fileno(stdin));
            dup2(rin_file, fileno(stdin));
        }
        if (rout_file != -1) {
            original_stdout = dup(fileno(stdout));
            dup2(rout_file, fileno(stdout));
        }
    }

    int status_builtin_execution = ExecuteBuiltinCommand(argv, n_args);
    

    switch (status_builtin_execution) {
        case BUILTIN_ERROR:
            fprintf(stderr, "Builtin %s error.\n", command->args->arg);
            FinishRedirs(rin_file, rout_file, original_stdin, original_stdout);
            return OK_STATUS;
        case EXIT_SUCCESS:
            FinishRedirs(rin_file, rout_file, original_stdin, original_stdout);
            return OK_STATUS;
    };


    pid_t pid, wpid;
    int status;
    


    pid = fork();
    if (pid == 0) {
        if (NOT_LAST_CMD(pos)) {
            close(fd[0]); /// Child won't read
        }
        if (NOT_FIRST_CMD(pos) && original_stdin == -1) {
            rin_file = prevfd[0];
            dup2(rin_file, fileno(stdin));
        } else if (NOT_FIRST_CMD(pos)) {
            close(prevfd[0]);
            close(original_stdin);
        }
        if (NOT_LAST_CMD(pos) && original_stdout == -1) {
            rout_file = fd[1];
            dup2(rout_file, fileno(stdout)); 
        } else if (NOT_LAST_CMD(pos)){
            close(fd[1]);
            close(original_stdout);   
        }

        // Child process
        int respond = ExecuteCommandInFork(argv, n_args);
        int exit_status = OK_STATUS;
        switch (respond) {
            case ENOENT:
                fprintf(stderr, "%s: no such file or directory\n", command->args->arg);
                exit_status = EXIT_FAILURE;
                break;
            case EACCES:
                fprintf(stderr, "%s: permission denied\n", command->args->arg);
                exit_status = EXIT_FAILURE;
                break;
            case OK_STATUS:
                break;
            default:
                fprintf(stderr, "%s: exec error\n", command->args->arg);
                exit_status = EXIT_FAILURE;
        }
        exit(exit_status);
    } else if (pid < 0) {
        // Error forking
        fprintf(stderr, "%s: exec error\n", command->args->arg);
        exit(EXIT_FAILURE); // Terminate the child process
    } else  {
        // Parent process
        wpid = wait(&status);
        if (NOT_LAST_CMD(pos)) {
            close(fd[1]);
        }
        if (NOT_FIRST_CMD(pos)) {
            close(prevfd[0]);
        }
        if (wpid == -1) {
            exit(EXIT_FAILURE);
        }
       
    }


    FinishRedirs(rin_file, rout_file, original_stdin, original_stdout);
    
   

    return OK_STATUS;   
}

int ExecuteCommands(commandseq* commands) {
    if (commands == NULL) {
        return OK_STATUS;
    }

    int fd[2], prevfd[2];
    fd[0] = fd[1] = prevfd[0] = prevfd[1] = 0;
    commandseq* start = commands;
    int cnt_elems =0 ;
    do {
        int pos = (FIRST_CMD) * (start == commands) + (LAST_CMD) * (commands->next == start);
        int status = ExecuteCommand(commands->com, pos, fd, prevfd);
        if (status != OK_STATUS) {
            return status;
        }
        commands = commands->next;
        prevfd[0] = fd[0];
        prevfd[1] = fd[1];
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