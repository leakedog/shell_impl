#ifndef REDIR_MAKER_H
#define REDIR_MAKER_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include "siparse.h"
#include "config.h"

int RunRedirs(command* command, int* rin_file, int* rout_file);

void FinishRedirs(int rin_file, int rout_file, int original_stdin, int original_stdout);

#endif  // REDIR_MAKER_H