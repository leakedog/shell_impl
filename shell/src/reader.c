#include "../include/reader.h"

char buffer[MAX_LINE_LENGTH + 1];
char next_line_saved[MAX_LINE_LENGTH + 1];
int prefix_filled = 0;

char* ReadLine() {  
    ssize_t bytes_read = read(STDIN_FILENO, next_line_saved + prefix_filled, MAX_LINE_LENGTH - prefix_filled);

    if (bytes_read == -1) {
        perror("lsh: Error reading from stdin\n");
        exit(EXIT_FAILURE);
    }

    prefix_filled += bytes_read;

    unsigned long j = -1;
    for (int i = 0; i < prefix_filled; i++) {
        if (next_line_saved[i] == '\0' || next_line_saved[i] == '\n') {
            j = i;
        }
    }
    

    /// let's copy that prefix to [0, j] buffer
    
    strncpy(buffer, next_line_saved, j + 1);

    strncpy(next_line_saved, next_line_saved + j + 1, prefix_filled - j - 1);
    
    prefix_filled -= j + 1;
    next_line_saved[prefix_filled] = '\0';
        
    return buffer;
}