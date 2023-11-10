#ifndef _CONFIG_H_
#define _CONFIG_H_

#define MAX_LINE_LENGTH 2048

#define BUFFER_SIZE (MAX_LINE_LENGTH * 2)

#define MAX_COMMAND_CNT 128

#define SYNTAX_ERROR_STR "Syntax error.\n"

#define EXEC_FAILURE 127

#define PROMPT_STR "$ "

#define NOT_ENOUGH_PERMISSIONS 128

#define NO_FILE_FOUND 129

#define ALLOC_FAILED 1

#define OK_STATUS 0 

#define REDIR_ERROR 130

// reader error
enum ReaderError {
    ABORT_OK = 0,
    ABORT_FAILURE = 1,
    SKIP_LINE = 2,
    PROCEED_LINE = 3,
};

#endif /* !_CONFIG_H_ */
