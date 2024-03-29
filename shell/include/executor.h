#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "./siparse.h"
#include "./config.h"
#include "./builtins.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include "utils.h"
#include "redir_maker.h"
#include "signal_handler.h"

#define FIRST_CMD (1 << 0)
#define LAST_CMD (1 << 1)
#define NOT_FIRST_CMD(x) ((x & FIRST_CMD) == 0)
#define NOT_LAST_CMD(x) ((x & LAST_CMD) == 0)

char** GetArgvFromCommmand(command* command, int* n_args);

int GetIdBuiltinCommand(char** argv, int n_args);

int ExecuteCommandBuiltin(command* command, char** argv, int n_args, int fd[2], int prevfd[2]);

int ExecuteCommandForked(command* command, char** argv, int n_args, int command_pos_mask, int child_id, int fd[2], int prevfd[2], bool is_background);

int ExecuteCommand(command* command, int pos, int child_id, int fd[2], int prevfd[2], bool is_background);

/*
    Execute sequence of commands. Each command will be executed, if one of the command wasn't executed successfully,
    executing will be finished.
*/

int ExecuteCommands(commandseq* commands, bool is_background);


/*
    Execute sequence of pipelines. Each pipeline will be executed individually, no matter 
    if program was executed with error.
*/
int Execute(pipelineseq* seq);


#endif  // EXECUTOR_H