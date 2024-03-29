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


int GetIdBuiltinCommand(char** argv, int n_args) {
    if (n_args == 0) {
        return -1;
    }
    for (int i = 0; builtins_table[i].name != NULL; i++) {
        if (strncmp(builtins_table[i].name, argv[0], MIN(strlen(argv[0]), strlen(builtins_table[i].name))) == 0) {
            return i;
        }
    }
    return -1;
}

int ExecuteCommandBuiltin(command* command, char** argv, int n_args, int fd[2], int prevfd[2]) {
    int original_stdin = -1, original_stdout = -1;
    int rin_file = -1, rout_file = -1;
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
    int builtin_command_id = GetIdBuiltinCommand(argv, n_args);    
    int result = (*builtins_table[builtin_command_id].fun)(argv);
    if (result == BUILTIN_ERROR) {
        fprintf(stderr, "Builtin %s error.\n", command->args->arg);
    }
    FinishRedirs(rin_file, rout_file, original_stdin, original_stdout);
    return OK_STATUS;
}

int ExecuteCommandForked(command* command, char** argv, int n_args, int command_pos_mask, int child_id, int fd[2], int prevfd[2], bool is_background) {
    if (NOT_LAST_CMD(command_pos_mask)) {
        if (pipe(fd) == -1) {
            fprintf(stderr, "lsh: Error creating pipe\n");    
            exit(EXIT_FAILURE);
        }
    }
    pid_t pid, wpid;
    int status;
    pid = fork();
    int rin_file = -1, rout_file = -1;
    if (pid == 0) {
        SetDefaultHandler();
        if (RunRedirs(command, &rin_file, &rout_file) == REDIR_ERROR) {
            if (!is_background) {
                DecreaseActiveForeignChildren();
            }
            close(rin_file);
            close(rout_file);
            exit(EXIT_FAILURE);
        } 
        if (is_background) {
            RunAsBackground();
        }
        if (rin_file != -1) {
            dup2(rin_file, fileno(stdin));
            close(rin_file);
        }
        if (rout_file != -1) {
            dup2(rout_file, fileno(stdout));
            close(rout_file);
        }
        /// Now let's do some things with pipes and fds
        if (NOT_LAST_CMD(command_pos_mask)) {
            close(fd[0]); /// Child won't read from pipe
        }
        if (NOT_FIRST_CMD(command_pos_mask) && rin_file == -1) {
            dup2(prevfd[0], fileno(stdin));
            close(prevfd[0]);
        } else if (NOT_FIRST_CMD(command_pos_mask)) {
            close(prevfd[0]);
        }
        if (NOT_LAST_CMD(command_pos_mask) && rout_file == -1) {
            dup2(fd[1], fileno(stdout)); 
            close(fd[1]);
        } else if (NOT_LAST_CMD(command_pos_mask)){
            close(fd[1]);
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
        fflush(stdout);   
        /// close files     
        close(rin_file);close(rout_file);
        /// close pipes
        close(prevfd[0]); close(fd[1]);
        exit(exit_status);
    } else if (pid < 0) {
        // Error forking
        fprintf(stderr, "%s: exec error\n", command->args->arg);
        exit(EXIT_FAILURE);
    } else {
        if (!is_background) {
            SetForegroundChildPid(child_id, pid);
        }
        // Parent process
        if (NOT_LAST_CMD(command_pos_mask)) {
            close(fd[1]);
        }
        if (NOT_FIRST_CMD(command_pos_mask)) {
            close(prevfd[0]);
        }
    }

    return OK_STATUS;
}

int ExecuteCommand(command* command, int command_pos_mask, int child_id, int fd[2], int prevfd[2], bool is_background) {
    if (command == NULL) {
        return OK_STATUS;
    }
    
    
    int n_args = 0;

    char** argv = GetArgvFromCommmand(command, &n_args);

    if (n_args == 0) {
        return OK_STATUS;
    }

    if (GetIdBuiltinCommand(argv, n_args) != -1) {
        return ExecuteCommandBuiltin(command, argv, n_args, fd, prevfd);
    }
    
    return ExecuteCommandForked(command, argv, n_args, command_pos_mask, child_id, fd, prevfd, is_background);
}

bool CheckEmptyInPipeline(commandseq* commands, int* n_commands) {
    commandseq* start = commands;
    bool was_null = false;
    do {
        if (commands->com != NULL) {
            (*n_commands)++;
        }
        was_null = (commands->com == NULL);
        commands = commands->next;
    } while(commands != start);
    return was_null && *n_commands > 1;
}

int ExecuteCommands(commandseq* commands, bool is_background) {
    WaitForegroundChildren();
    if (commands == NULL) {
        return OK_STATUS;
    }
    
    int n_commands = 0;
    /// Check and compute n_commands
    if (CheckEmptyInPipeline(commands, &n_commands)) {
        fprintf(stderr, SYNTAX_ERROR_STR);
        return OK_STATUS;
    }

    if (!is_background) {
        SetForegroundChildren(n_commands);
    } else {
        ClearForegroundChildren();
    }

    int fd[2], prevfd[2];
    fd[0] = fd[1] = prevfd[0] = prevfd[1] = 0;
    commandseq* start = commands;
    int child_id = 0;
    do {
        int command_pos_mask = (FIRST_CMD) * (start == commands) + (LAST_CMD) * (commands->next == start);
        int status = ExecuteCommand(commands->com, command_pos_mask, child_id++, fd, prevfd, is_background);
        if (status != OK_STATUS) {
            return status;
        }
        commands = commands->next;
        prevfd[0] = fd[0];
        prevfd[1] = fd[1];
    } while(commands != start);
    
    WaitForegroundChildren();

    return OK_STATUS;
}

int Execute(pipelineseq* seq) {
    if (seq == NULL) {
        return OK_STATUS;
    }
    pipelineseq* start = seq;
    do {
        int status = ExecuteCommands(seq->pipeline->commands, seq->pipeline->flags == INBACKGROUND);
        seq = seq->next;
    } while(start != seq);

    return OK_STATUS;
}