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
        tdata[i].start = i * (numChars / NUM_THREADS) - PADDING;
        tdata[i].end = (i + 1)  * (numChars / NUM_THREADS) + PADDING;
        if (tdata[i].start < 0)
        {
            tdata[i].start = 0;
        }
        if (tdata[i].end > numChars -1)
        {
            tdata[i].end = numChars -1;
        }

        tdata[i].A = buffer;
    }

    // create child threads
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_create(&threadIDs[i], NULL, findMaxSumSeq, &tdata[i]);
    }

    // wait for child threads to be done
    for (int i=0; i<NUM_THREADS; ++i)
        pthread_join(threadIDs[i], NULL);

    // Gather data
    int longestRun = tdata[0].max;
    int startingPos = tdata[0].bestpos;

    for (int index = 1; index < NUM_THREADS; ++index)
    {
        if (tdata[index].max > longestRun)
        {
            longestRun = tdata[index].max;
            startingPos = tdata[index].bestpos;
        }
    }

    printf("Longest desired run: %d, at position %d\n", longestRun, startingPos);

    // Need to actually print out the sequence

    for (int index = -longestRun + 1; index < 0; ++index)
    {
        if (index == -1)
        {
            printf("%d = %d\n", buffer[index + startingPos], buffer[startingPos]);
        }
        else {
            printf("%d + ", buffer[index + startingPos]);
        }
    }

    // Graduate Section
    ThreadInfoExtended tdataE[NUM_THREADS];
    pthread_t threadIDsE[NUM_THREADS];

    int digits = 2;

    // Initialize arrays
    for (int i = 0; i < NUM_THREADS; ++i)
    {

        tdataE[i].start = i * (numChars / NUM_THREADS);
        tdataE[i].end = (i + 1)  * (numChars / NUM_THREADS);
        if (tdataE[i].start < 0)
        {
            tdataE[i].start = 0;
        }
        if (tdataE[i].end > numChars -1)
        {
            tdataE[i].end = numChars -1;
        }

        tdataE[i].A = buffer;
        tdataE[i].digits = digits;
        tdataE[i].numChars = numChars;
    }

    // create child threads
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_create(&threadIDsE[i], NULL, findMaxSumSeqExtended, &tdataE[i]);
    }

    // wait for child threads to be done
    for (int i=0; i<NUM_THREADS; ++i)
        pthread_join(threadIDsE[i], NULL);

    // Gather data
    int longestRunE = tdataE[0].max;
    int startingPosE = tdataE[0].bestpos;

    for (int index = 1; index < NUM_THREADS; ++index)
    {
        if (tdata[index].max > longestRunE)
        {
            longestRunE = tdataE[index].max;
            startingPosE = tdataE[index].bestpos;
        }
    }

    printf("Longest desired run with %d digits: %d, at position %d\n", digits, longestRunE, startingPosE);

    for (int index = -(longestRunE - digits); index < 0; ++index)
    {
        if (index == -1)
        {
            printf("%d = ", buffer[index + startingPosE]);
        }
        else {
            printf("%d + ", buffer[index + startingPosE]);
        }
    }
    for (int index = 0; index < digits; ++index)
    {
        printf("%d", buffer[startingPosE + index]);
    }
    printf("\n");

    // Don't seem to be getting instructor's result

    longestRunE = 32;
    startingPosE = 4806;

    for (int index = -(longestRunE - digits); index < 0; ++index)
    {
        if (index == -1)
        {
            printf("%d = ", buffer[index + startingPosE]);
        }
        else {
            printf("%d + ", buffer[index + startingPosE]);
        }
    }
    for (int index = 0; index < digits; ++index)
    {
        printf("%d", buffer[startingPosE + index]);
    }
    printf("\n");

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

void *findMaxSumSeqExtended(void *param)
{
    ThreadInfoExtended *data;

    data = (ThreadInfoExtended *) param; // casts param into ThreadInfo* type

    int maxLen = 0, indexOfMax = 0, negIndex, sum, tens;

    for (int i = data->start; i <= data->end; i++)
    {
        negIndex = -1; // We will be moving backwards and subtracting from starting index
        sum = 0;
        tens = 1;

        // Need to create the first multiple of tens spot
        for (int j = 0; j < data->digits - 1; ++j)
        {
            tens *= 10;
        }

        // Create the sum
        for(int j = 0; j < data->digits; ++j)
        {
            if (i + j < data->numChars)
            {
                sum += tens * data->A[i+j];
                tens /= 10;
            }
        }

        while (i + negIndex > 0 && sum >= 0)
        {
            sum -= data->A[i + negIndex];
            negIndex -= 1;
        }

        // sum should be negative check that adding previous back in makes it 0
        if (sum + data->A[i + negIndex + 1] == 0)
        {

            if (-negIndex > maxLen + 1)
            {
                maxLen = -negIndex - 1;
                indexOfMax = i;
            }
        }
    }

    data->bestpos = indexOfMax;
    data->max = maxLen + data->digits - 1;
}