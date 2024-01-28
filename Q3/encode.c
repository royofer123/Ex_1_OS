#include <stdio.h>
#include <stdlib.h>
#include "libraryCodec.h"

// Check if a file exists
int fileExists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Read the content of a file into memory
char *readFile(const char *filename, long *fileSize) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(*fileSize + 1); // +1 for the null terminator
    if (!content) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    if (fread(content, 1, *fileSize, file) != *fileSize) {
        perror("Error reading file");
        free(content);
        fclose(file);
        return NULL;
    }

    content[*fileSize] = '\0'; // Null-terminate the content
    fclose(file);
    return content;
}

// Write content to a file
int writeFile(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file for writing");
        return 0;
    }

    if (fputs(content, file) == EOF) {
        perror("Error writing to file");
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    // Check if source file exists
    if (!fileExists(argv[1])) {
        printf("Source file '%s' does not exist.\n", argv[1]);
        return 1;
    }

    // Create codec
    void *codec = createCodec(keyEncode);
    if (!codec) {
        printf("Failed to create codec!\n");
        return 1;
    }

    long fileSize;
    char *fileContent = readFile(argv[1], &fileSize); // Read file content
    if (!fileContent) {
        printf("Failed to read file '%s'.\n", argv[1]);
        freeCodec(codec);
        return 1;
    }

    char *encodedContent = (char *)malloc(fileSize + 1); // Allocate memory for encoded content
    if (!encodedContent) {
        perror("Error allocating memory");
        free(fileContent);
        freeCodec(codec);
        return 1;
    }

    encode(fileContent, encodedContent, fileSize, codec); // Encode file content

    // Write the encoded content to the destination file
    if (!writeFile(argv[2], encodedContent)) {
        printf("Failed to write to destination file '%s'.\n", argv[2]);
        free(fileContent);
        free(encodedContent);
        freeCodec(codec);
        return 1;
    }

    // Free allocated memory and close codec
    free(fileContent);
    free(encodedContent);
    freeCodec(codec);

    printf("Encoding successful.\n");
    return 0;
}
