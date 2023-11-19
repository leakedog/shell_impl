#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>

void SetForegroundChildren(int cnt);

void ClearForegroundChildren();

void DecreaseActiveForeignChildren();

void SetForegroundChildPid(int child_id, pid_t cpid);

bool IsForegroundChildren(pid_t pid);

void Handler(int sig);

void SetDefaultHandler();

void RunAsBackground();

void WaitForegroundChildren();

void SetDefaultBlock();

void SetChildBlock();

void InitializeSignals();

void PrintAllFinishedSignals();

#endif  // SIGNAL_HANDLER_H