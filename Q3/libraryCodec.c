#include <stdio.h>

#include <stdlib.h>

#include "libraryCodec.h"
#include <string.h>


// Global array of characters

char globalKey[62] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

char keyEncode[62] = "cdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890ab";

void *createCodec(char key[62])
{

    // Make sure not 2 chars are the same (mapping to different original chars)

    for (int i = 0; i < 62; i++)
    {

        for (int j = i + 1; j < 62; j++)
        {

            if (key[i] == key[j])
            {

                printf("Duplicate found, code is incorrect!");

                return NULL;
            }
        }
    }

    char* ans = (char*)malloc(125 * sizeof(char));
    if (!ans) {
        return NULL;
    }

    // Copy the characters from globalKey to ans
    memcpy(ans, globalKey, 62 * sizeof(char));

    // Copy the characters from key to ans starting at index 62
    memcpy(ans + 62, key, 62 * sizeof(char));

    // Add null terminator at the end of the string
    ans[124] = '\0';

    return ans;
}

int encode(char *textin, char *textout, int len, void *codec)
{
    // Check for null pointers
    if (!textin || !textout || !codec)
    {
        return -1;
    }

    int count = 0;
    char *key = (char *)codec;

    for (int i = 0; i < len; i++)
    {
        // Find the index of the character in the codec
        int index = -1;
        for (int j = 0; j < 62; j++)
        {
            if (textin[i] == key[j])
            {
                index = j;
                break;
            }
        }

        // Encode the character if found in the codec
        if (index != -1)
        {
            textout[i] = key[62 + index];
            count++;
        }
        else
        {
            // If character not found in codec, keep it unchanged
            textout[i] = textin[i];
        }
    }

    return count;
}

int findCharacterIndex(char textChar, char *key)
{
    // Iterate through the mapping to find the character
    for (int j = 61; j < 124; j++)
    {
        if (textChar == key[j])
        {
            // Found the character, calculate and return its index
            return j - 62;
        }
    }
    // Character not found in the mapping
    return -1;
}

int decode(char *textin, char *textout, int len, void *codec)
{
    // Check for null pointers
    if (!textin || !textout || !codec)
    {
        return -1;
    }

    // Initialize count for successful decoding
    int count = 0;

    // Cast the codec pointer to char pointer
    char *key = (char *)codec;

    // Iterate through the input text
    for (int i = 0; i < len; i++)
    {
        // Find the index of the character in the codec's mapping
        int index = findCharacterIndex(textin[i], key);

        // If character found in codec's mapping
        if (index != -1)
        {
            // Decode the character and store in output text
            textout[i] = key[index];
            count++; // Increment count for successful decoding
        }
        else
        {
            // Character not found, copy as it is
            textout[i] = textin[i];
        }
    }

    return count; // Return count of successfully decoded characters
}

void freeCodec(void *codec)
{

    free(codec);
}

