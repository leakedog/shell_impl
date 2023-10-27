#include "../include/shell.h"

bool CheckIsSpecialDevice() {
  struct stat file_stat;

  if (fstat(STDIN_FILENO, &file_stat) == 0) {
    if (S_ISCHR(file_stat.st_mode)) {
      return true;
    }
  } 

  return false;
}

void RunShell() {
  char *line;
  pipelineseq* seq;
  int status;

  bool is_special_device = CheckIsSpecialDevice();

  do {

    if (is_special_device) {
      write(STDOUT_FILENO, PROMPT_STR, strlen(PROMPT_STR));
    }

    line = ReadLine();
    seq = parseline(line);
    status = Execute(seq);
  } while (!status);

}