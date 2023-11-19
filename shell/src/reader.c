#include "../include/reader.h"
#include <stdio.h>



char buffer[BUFFER_SIZE];
char* buffer_start = buffer;
int prefix_filled = 0;


void ProceedTerminalLine(enum ReaderError* error) {
    *error = PROCEED_LINE;
    SetDefaultBlock();
    ssize_t bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
    SetChildBlock();
    if (bytes_read == -1) {
        *error = ABORT_FAILURE;
        return;
    } else if (bytes_read == 0) {
        *error = ABORT_OK;
        return;
    } else if (bytes_read >= MAX_LINE_LENGTH) {
        fprintf(stderr, SYNTAX_ERROR_STR);
        *error = SKIP_LINE;
        return;
    }

    if (buffer[bytes_read - 1] != '\n') {
        write(STDOUT_FILENO, "\n", 1);
        *error = SKIP_LINE;
        return;
    } 
    
    buffer[bytes_read - 1] = '\0';
    Execute(parseline(buffer));
}

void ProceedFileLines(enum ReaderError* error) {
    *error = PROCEED_LINE;

    ssize_t bytes_read = 0;
    /// lets read until full 
    do {
        char* point_to_read = buffer + prefix_filled;
        SetDefaultBlock();
        bytes_read = read(0, point_to_read, (BUFFER_SIZE - prefix_filled));
        SetChildBlock();
        if (bytes_read == -1) {
            *error = ABORT_FAILURE;
            return;
        }
        prefix_filled += bytes_read;
    } while(prefix_filled != BUFFER_SIZE && bytes_read > 0);

    char* buffer_start = buffer;
    char* buffer_end = buffer_start + prefix_filled;
    char* line_finishing_position; 
    static bool was_syntax_error = false;
    do {
        line_finishing_position = strchr(buffer_start, '\n');
        if (line_finishing_position == NULL) {
            if (prefix_filled > MAX_LINE_LENGTH || was_syntax_error) {
                if (!was_syntax_error) {
                    fprintf(stderr, SYNTAX_ERROR_STR);
                    *error = SKIP_LINE;
                }
                was_syntax_error = true;
                prefix_filled = 0;
                break;
            } else {
                if (bytes_read == 0 && prefix_filled != BUFFER_SIZE) {
                    if (prefix_filled) {
                        *(buffer_start + prefix_filled) = '\0';
                        pipelineseq* line = parseline(buffer_start);
                        Execute(line);
                    }
                    prefix_filled = 0;
                    *error = ABORT_OK;
                    break;
                }
                prefix_filled = (buffer_end - buffer_start);
                memmove(buffer, buffer_start, prefix_filled);
            }
        } else {
            *line_finishing_position = '\0';
            if ((line_finishing_position - buffer_start + 1) > MAX_LINE_LENGTH || was_syntax_error) {
                if (!was_syntax_error) {
                    fprintf(stderr, SYNTAX_ERROR_STR);
                    *error = SKIP_LINE;
                }
                was_syntax_error = false;
            } else {
                pipelineseq* line = parseline(buffer_start);
                int status = Execute(line);
            }
            prefix_filled -= (line_finishing_position - buffer_start + 1);
            buffer_start = line_finishing_position + 1;
        }
    } while(line_finishing_position != NULL && prefix_filled != 0);
}
    