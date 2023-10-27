#ifndef SHELL_H
#define SHELL_H

#include "builtins.h"
#include "utils.h"
#include "siparse.h"
#include "config.h"
#include "reader.h"
#include "executor.h"
#include <sys/types.h>
#include <sys/stat.h>

bool CheckIsSpecialDevice();

void RunShell();

#endif  // SHELL_H

