#ifndef _UTILS_H_
#define _UTILS_H_

#include "siparse.h"

void my_strlen(const char*, size_t*);
void printcommand(command *, int);
void printpipeline(pipeline, int);
void printparsedline(pipelineseq *);

command * pickfirstcommand(pipelineseq *);

#endif /* !_UTILS_H_ */
