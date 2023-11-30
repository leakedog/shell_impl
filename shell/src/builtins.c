#include "../include/builtins.h"
#include <limits.h>


int echo(char*[]);
int lexit(char *[]);
int lkill(char *[]);
int lcd(char *[]);
int lls(char *[]);
int undefined(char *[]);

builtin_pair builtins_table[]={	
	{"exit",	&lexit},
	{"lecho",	&echo},
	{"lcd",		&lcd},
	{"lkill",	&lkill},
	{"lls",		&lls},
	{NULL,NULL}
};

int 
echo( char * argv[])
{
	int i =1;
	if (argv[i]) printf("%s", argv[i++]);
	while  (argv[i])
		printf(" %s", argv[i++]);

	printf("\n");
	fflush(stdout);
	return 0;
}

int 
undefined(char * argv[])
{
	fprintf(stderr, "Command %s undefined.\n", argv[0]);
	return BUILTIN_ERROR;
}

int lexit(char * argv[]) {
    if(argv[1] == NULL){
        exit(0);
    }
    if(argv[2] != NULL){
        return BUILTIN_ERROR;
    }
    exit(atoi(argv[1]));
}


int lkill(char * argv[]) {
	errno = OK_STATUS;
    if(argv[1] == NULL){
        return BUILTIN_ERROR;
    }

    char *er;
	if (argv[2] == NULL) {
		long process_id = strtol(argv[1], &er, 10);	

		if (strlen(er) != 0 || errno == ERANGE) {
			return BUILTIN_ERROR;
		}

		int respond = kill(process_id, SIGTERM);
		if (respond) {
			return BUILTIN_ERROR;
		}
		return EXIT_SUCCESS;
	} 
	
	if (argv[3] != NULL) {
		return BUILTIN_ERROR;
	}

	long process_id = strtol(argv[2], &er, 10);	

	if (strlen(er) != 0 || errno == ERANGE) {
		return BUILTIN_ERROR;
	}

	long signal = strtol(argv[1] + 1, &er, 10);

	if (strlen(er) != 0 || errno == ERANGE || signal > INT_MAX || signal < INT_MIN) {
		return BUILTIN_ERROR;
	} 

	int respond = kill(process_id, signal);

	if (respond) {
		return BUILTIN_ERROR;
	}

	return EXIT_SUCCESS;
}

int lcd(char * argv[]) {
	if (argv[1] == NULL) {
		int respond = chdir(getenv("HOME"));
		if (respond) {
			return BUILTIN_ERROR;
		}
		return EXIT_SUCCESS;
	}

	if (argv[2] != NULL) {
		return BUILTIN_ERROR;
	}

	int respond = chdir(argv[1]);
	if (respond) {
		return BUILTIN_ERROR;
	}
	return EXIT_SUCCESS;
}

int lls(char * argv[]) {
	if (argv[1] != NULL) {
		return BUILTIN_ERROR;
	}
	DIR* dir;
	struct dirent* dir_entry;
	dir = opendir("./");

	if (dir == NULL) {
		return BUILTIN_ERROR;
	}

	while ((dir_entry = readdir(dir)) != NULL) {
		if (dir_entry->d_name[0] == '.') continue;
		write(STDOUT_FILENO, dir_entry->d_name, strlen(dir_entry->d_name));
		write(STDOUT_FILENO, "\n", 1);
	}

	closedir(dir);
	return EXIT_SUCCESS;
}