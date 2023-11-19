#ifndef READER_H
#define READER_H
#include "config.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "executor.h"
#include "utils.h"
#include "siparse.h"
#include "signal_handler.h"

void ProceedTerminalLine(enum ReaderError* status);

char* ReadLine();

void ProceedFileLines(enum ReaderError* error);

#endif  // READER_H