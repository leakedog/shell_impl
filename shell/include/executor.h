#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <unistd.h>
#include <sys/wait.h>
#include "./siparse.h"
#include "./config.h"


int ExecuteCommandInFork(command* command);

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