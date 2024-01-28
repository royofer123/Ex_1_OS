#include <stdio.h>
#include <stdlib.h>
#include "libraryCodec.h"

// Check if file exists
int fileExists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Read file content into memory
char *readFile(const char *filename, long *fileSize) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(*fileSize + 1); // Allocate memory for content
    if (!content) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    fread(content, 1, *fileSize, file); // Read file content
    content[*fileSize] = '\0'; // Null-terminate the content
    fclose(file);
    return content;
}

// Write content to file
int writeFile(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file for writing");
        return 0;
    }

    fputs(content, file); // Write content to file
    fclose(file);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    if (!fileExists(argv[1])) {
        printf("Source file does not exist.\n");
        return 1;
    }

    void *cipher = createCodec(keyEncode); // Create cipher
    if (!cipher) {
        printf("Failed to create cipher!\n");
        return 1;
    }

    long fileSize;
    char *fileContent = readFile(argv[1], &fileSize); // Read file content
    if (!fileContent) {
        printf("Failed to read file.\n");
        freeCodec(cipher);
        return 1;
    }

    char *decodedContent = (char *)malloc(fileSize + 1); // Allocate memory for decoded content
    if (!decodedContent) {
        perror("Error allocating memory");
        free(fileContent);
        freeCodec(cipher);
        return 1;
    }

    decode(fileContent, decodedContent, fileSize, cipher); // Decode file content 

    if (!writeFile(argv[2], decodedContent)) { // Write decoded content to destination file
        printf("Failed to write to destination file.\n");
        free(fileContent);
        free(decodedContent);
        freeCodec(cipher);
        return 1;
    }

    free(fileContent);
    free(decodedContent);
    freeCodec(cipher);
    printf("Decoding successful.\n");
    return 0;
}
