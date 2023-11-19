#include "../include/shell.h"
#include <stdio.h>
#include <stdlib.h>

bool CheckIsSpecialDevice() {
  struct stat file_stat;

  if (fstat(STDIN_FILENO, &file_stat) == 0) {
    if (S_ISCHR(file_stat.st_mode)) {
      return true;
    }
  } 

  return false;
}

void RunTerminalShell() {
  enum ReaderError status = PROCEED_LINE;
  while(true) {
    PrintAllFinishedSignals();
    write(STDOUT_FILENO, PROMPT_STR, strlen(PROMPT_STR));
    
    ProceedTerminalLine(&status);

    if (status == ABORT_OK) {
      write(STDOUT_FILENO, "\n", 1);
      exit(0);
    }

    if (status == ABORT_FAILURE) {
      fprintf(stderr, "lsh: Error reading from stdin\n");    
      exit(EXIT_FAILURE);
    }
}

  }
void RunFileShell() {
  enum ReaderError status = PROCEED_LINE;
  while(true) {
    ProceedFileLines(&status);
    if (status == ABORT_OK) {
      exit(EXIT_SUCCESS);
    }

    if (status == ABORT_FAILURE) {
      fprintf(stderr, "lsh: Error reading from stdin\n");  
      exit(EXIT_FAILURE);
    }
  }
}


void RunShell() {
  char *line;
  pipelineseq* seq;
  int status;
  InitializeSignals();
  bool is_special_device = CheckIsSpecialDevice();

  if (is_special_device) {
    RunTerminalShell();
  } else {
    RunFileShell();
  }
}