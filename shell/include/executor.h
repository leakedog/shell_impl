#ifndef EXECUTOR_H
#define EXECUTOR_H
#include <unistd.h>
#include <sys/wait.h>
#include "./siparse.h"
#include "./config.h"

int ExecuteCommandInFork(command* command);

int ExecuteCommand(command* command);

int ExecuteCommands(commandseq* commands);

int Execute(pipelineseq* seq);


#endif  // EXECUTOR_H