#include "../include/reader.h"

char* ReadLine() {
    int buf_size = MAX_LINE_LENGTH;
    char* buffer = calloc(buf_size, sizeof(char));

    if (buffer == NULL) {
        perror("lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_read = read(0, buffer, buf_size);

    if (bytes_read == -1) {
        perror("lsh: Error reading from stdin\n");
        exit(EXIT_FAILURE);
    }
        
    return buffer;
}