#include "file_helpers.h"

void* load_file(char* filename, size_t* bytes) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror_and_exit(filename);
    }

    fseek(file, 0, SEEK_END);
    *bytes = ftell(file);
    rewind(file);

    // Allocate memory for entire content
    void* buffer = calloc(1, *bytes + 1);

    // Copy the file contents into the buffer
    fread(buffer, *bytes, 1, file);

    fclose(file);
    return buffer;
}

void save_file(char* filename, void* src, size_t bytes) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        perror_and_exit(filename);
    }
    fwrite(src, 1, bytes, file);
    fclose(file);
}