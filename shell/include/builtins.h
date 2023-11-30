#ifndef _BUILTINS_H_
#define _BUILTINS_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include "config.h"
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#define BUILTIN_ERROR 121

typedef struct {
	char* name;
	int (*fun)(char**); 
} builtin_pair;

extern builtin_pair builtins_table[];

#endif /* !_BUILTINS_H_ */
