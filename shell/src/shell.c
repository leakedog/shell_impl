#include "../include/shell.h"

void RunShell() {
 char *line;
  pipelineseq* seq;
  int status;

  do {
    write(0, PROMPT_STR, strlen(PROMPT_STR));
    line = ReadLine();
    seq = parseline(line);
    status = Execute(seq);

    free(line);
  } while (status);
}