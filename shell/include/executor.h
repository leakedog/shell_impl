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

char** GetArgvFromCommmand(command* command, int* n_args);

int ExecuteBuiltinCommand(char** argv, int n_args);

int ExecuteCommandInFork(char** argv, int n_args);

int ExecuteCommand(command* command);


/*
    Execute sequence of commands. Each command will be executed, if one of the command wasn't executed successfully,
    executing will be finished.
*/

int ExecuteCommands(commandseq* commands);


/*
    Execute sequence of pipelines. Each pipeline will be executed individually, no matter 
    if program was executed with error.
*/
int Execute(pipelineseq* seq);


#endif  // EXECUTOR_H