//
// Created by Jared Krogsrud on 2/23/2023.
//
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h> // Unix util, thus red underline in Windows

#include "threads.jkrogsru.h"

int main(int argc, char *argv[])
{
    int rc, numChars;
    char buffer[BUFLEN];

    if (argc < 2)
    {
        printf("ERROR: need a filename\n");
        return 8;
    }

    rc = readFile(argv[1], &numChars, buffer);
    if (rc !=0)
    {
        return 8;
    }

    // This will print non-sense but the math should work out

//    printf("%d total characters\n", numChars);
//    printf("%s\n", buffer);
//    printf("length of buffer: %d\n", strlen(buffer));
//    printf("char at 31: %d\n", buffer[31]);

    // As 0 appears in pi it gets sent to the true zero character so when attempting
    // to read this as a string it gets cut off

    // To actually see "pi" digits

//    for (int i = 0; i < numChars; i++)
//    {
//        printf("%d", buffer[i]);
//    }
//    printf("\n");

    // Here we will create some threads and pass them into findMaxSumSeq function
    ThreadInfo tdata[NUM_THREADS];
    pthread_t threadIDs[NUM_THREADS];

    // Initialize arrays
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        tdata[i]
    }

    return 0;
}

int readFile(char *fileName, int *numChars, char *buffer)
{
    char line[LINELEN];
    char *chp;
    int len; // for line length in while loop
    int big_len = 0; // this is the position to write to in larger buffer

    FILE *fp = fopen(fileName, "r");

    if (fp == NULL)
    {
        printf("ERROR: cannot open file `%s'\n", fileName);
        return 1;
    }

    // Read file into buffer here
    chp = fgets(line, LINELEN, fp);
    if (chp == NULL)
    {
        printf("file is empty\n");
        fclose(fp);
        return 8;
    }

    while (chp != NULL)
    {
        len = strlen(line);
        if (line[len-1] == '\n') // Check we don't have a newline character floating about
        {
            line[len-1] = '\0'; // replace it with end of array character
            len = len - 1;
        }
        for (int i = 0; i < len; i++)
        {
            //append contents of the line[] buffer to big buffer
            if (isdigit(line[i])) // This will avoid the '.' in pi
            {
                buffer[big_len] = line[i] - 48;
                big_len++;
            }
        }
        chp = fgets(line, LINELEN, fp);
    }

    *numChars = big_len;
    return 0;
}

void *findMaxSumSeq(void *param)
{
    // void *param works like the Object type in Java
    ThreadInfo *data;

    data = (ThreadInfo *) param; // casts param into ThreadInfo* type

    int maxLen = 0, indexOfMax = 0, negIndex, sum;
    for (int i = data->start; i <= data->end; i++)
    {
        negIndex = -1; // We will be moving backwards and subtracting from starting index
        sum = data->A[i];
        while (i + negIndex > data->start && sum > 0)
        {
            sum -= data->A[i + negIndex];
            negIndex -= 1;
        }

        // if the sum is zero, then we have found a worthy candidate
        if (sum == 0)
        {
            if (-negIndex > maxLen)
            {
                maxLen = -negIndex;
                indexOfMax = i;
            }
        }
    }

    data->bestpos = indexOfMax;
    data->max = maxLen;
}