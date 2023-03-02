//
// Created by Jared Krogsrud on 2/23/2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h> // Unix util, thus red underline in Windows
#include <sys/time.h>

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

    // Read in File
    rc = readFile(argv[1], &numChars, buffer);
    if (rc !=0)
    {
        return 8;
    }

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

    // Display results
    displayResults(startingPos, longestRun, 1, buffer);

    /**
      *   GRADUATE SECTION
      *   Here you will find a generalized function wherein you can input the
      *   number digits you want the tail end of the max sum sequence to be.
      *
      *   That is the first problem had digits = 1, and the grad section has
      *   digits = 2. But we can experiment further.
      *
      *   Also found below is a means of timing how long it takes on these lists
      *   so that we may see the benefits of using multiple threads should we run
      *   this many times. Data is gathered by openeing a shared file and
      *   writing data in a csv file to be easier to visualize later.
      */
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
        if (tdataE[index].max > longestRunE)
        {
            longestRunE = tdataE[index].max;
            startingPosE = tdataE[index].bestpos;
        }
    }

    displayResults(startingPosE, longestRunE, digits, buffer);

    /**
     *  Here we will run some experiments on some random sets
     *  of numbers of sizes 100,000, 1,000,000 and 10,000,000
     *  and thread counts of 1, 2, 4, 8, 16
     */

    // First we generate a character arrays of random ints

    int buffLens[3] = {10000, 100000, 1000000};
    int threadCounts[5] = {1, 2, 4, 8, 16};

    printf("Loc 1\n");

    char buffer1[buffLens[0]];
    char buffer2[buffLens[1]];
    char buffer3[buffLens[2]];

    int r;
    int digitTest = 2;
    FILE *fp;
    char* fileName = "thread_timing.txt";

    struct timeval start, finish;
    struct timezone tzone;
    long timeUsed;


    // Initialize random number generator
    srand(time(NULL));

    // Initialize the random buffers
    // buffer1
    for (int i = 0; i < buffLens[0]; ++i)
    {
        r = rand() % 10;
        buffer1[i] = r;
    }

    // buffer2
    for (int i = 0; i < buffLens[1]; ++i)
    {
        r = rand() % 10;
        buffer2[i] = r;
    }

    // buffer3
    for (int i = 0; i < buffLens[2]; ++i)
    {
        r = rand() % 10;
        buffer3[i] = r;
    }

    // open file for writing
    fp = fopen(fileName, "w");

    // check if opened successfully
    if (fp == NULL)
    {
        printf("Error opening: %s\n", fileName);
        return 8;
    }

    printf("Loc 2\n");

    for (int digit = 1; digit <= 3; ++digit)
    {
        // Buffer 1
        // Run once per thread count
        for (int threadCounter = 0; threadCounter < 5; ++threadCounter)
        {
            // Start Timer
            gettimeofday(&start, &tzone);

            int numThreads = threadCounts[threadCounter];
            ThreadInfoExtended tdataExperiment[numThreads];
            pthread_t threadIDsExperiment[numThreads];

            // Initialize arrays
            for (int i = 0; i < numThreads; ++i)
            {

                tdataExperiment[i].start = i * (buffLens[0] / numThreads);
                tdataExperiment[i].end = (i + 1)  * (buffLens[0] / numThreads);
                if (tdataExperiment[i].start < 0)
                {
                    tdataExperiment[i].start = 0;
                }
                if (tdataExperiment[i].end > buffLens[0] -1)
                {
                    tdataExperiment[i].end = buffLens[0] -1;
                }

                tdataExperiment[i].A = buffer1;
                tdataExperiment[i].digits = digitTest;
                tdataExperiment[i].numChars = buffLens[0];
            }

            // begin search!
            // create child threads
            for (int i = 0; i < numThreads; ++i)
            {
                pthread_create(&threadIDsExperiment[i], NULL, findMaxSumSeqExtended, &tdataExperiment[i]);
            }

            // wait for child threads to be done
            for (int i=0; i<numThreads; ++i)
                pthread_join(threadIDsExperiment[i], NULL);

            // Gather data
            int longestRunExperiment = tdataExperiment[0].max;
            int startingPosExperiment = tdataExperiment[0].bestpos;

            for (int index = 1; index < numThreads; ++index)
            {
                if (tdataExperiment[index].max > longestRunExperiment)
                {
                    longestRunExperiment = tdataExperiment[index].max;
                    startingPosExperiment = tdataExperiment[index].bestpos;
                }
            }

            // End timer
            gettimeofday(&finish, &tzone);
            timeUsed = finish.tv_usec - start.tv_usec;
            // Write Data to file
            fprintf(fp, "%d,%d,%d,%lu\n", digit, buffLens[0], numThreads, timeUsed);
        }

        // Buffer 2
        // Run once per thread count
        for (int threadCounter = 0; threadCounter < 5; ++threadCounter)
        {
            // Start Timer
            gettimeofday(&start, &tzone);

            int numThreads = threadCounts[threadCounter];
            ThreadInfoExtended tdataExperiment[numThreads];
            pthread_t threadIDsExperiment[numThreads];

            // Initialize arrays
            for (int i = 0; i < numThreads; ++i)
            {

                tdataExperiment[i].start = i * (buffLens[1] / numThreads);
                tdataExperiment[i].end = (i + 1)  * (buffLens[1] / numThreads);
                if (tdataExperiment[i].start < 0)
                {
                    tdataExperiment[i].start = 0;
                }
                if (tdataExperiment[i].end > buffLens[1] -1)
                {
                    tdataExperiment[i].end = buffLens[1] -1;
                }

                tdataExperiment[i].A = buffer2;
                tdataExperiment[i].digits = digitTest;
                tdataExperiment[i].numChars = buffLens[1];
            }

            // begin search!
            // create child threads
            for (int i = 0; i < numThreads; ++i)
            {
                pthread_create(&threadIDsExperiment[i], NULL, findMaxSumSeqExtended, &tdataExperiment[i]);
            }

            // wait for child threads to be done
            for (int i=0; i<numThreads; ++i)
                pthread_join(threadIDsExperiment[i], NULL);

            // Gather data
            int longestRunExperiment = tdataExperiment[0].max;
            int startingPosExperiment = tdataExperiment[0].bestpos;

            for (int index = 1; index < numThreads; ++index)
            {
                if (tdataExperiment[index].max > longestRunExperiment)
                {
                    longestRunExperiment = tdataExperiment[index].max;
                    startingPosExperiment = tdataExperiment[index].bestpos;
                }
            }

            // End timer
            gettimeofday(&finish, &tzone);
            timeUsed = finish.tv_usec - start.tv_usec;
            // Write Data to file
            fprintf(fp, "%d,%d,%d,%lu\n", digit, buffLens[1], numThreads, timeUsed);
        }

        // Buffer 3
        // Run once per thread count
        for (int threadCounter = 0; threadCounter < 5; ++threadCounter)
        {
            // Start Timer
            gettimeofday(&start, &tzone);

            int numThreads = threadCounts[threadCounter];
            ThreadInfoExtended tdataExperiment[numThreads];
            pthread_t threadIDsExperiment[numThreads];

            // Initialize arrays
            for (int i = 0; i < numThreads; ++i)
            {

                tdataExperiment[i].start = i * (buffLens[2] / numThreads);
                tdataExperiment[i].end = (i + 1)  * (buffLens[2] / numThreads);
                if (tdataExperiment[i].start < 0)
                {
                    tdataExperiment[i].start = 0;
                }
                if (tdataExperiment[i].end > buffLens[2] -1)
                {
                    tdataExperiment[i].end = buffLens[2] -1;
                }

                tdataExperiment[i].A = buffer3;
                tdataExperiment[i].digits = digitTest;
                tdataExperiment[i].numChars = buffLens[2];
            }

            // begin search!
            // create child threads
            for (int i = 0; i < numThreads; ++i)
            {
                pthread_create(&threadIDsExperiment[i], NULL, findMaxSumSeqExtended, &tdataExperiment[i]);
            }

            // wait for child threads to be done
            for (int i=0; i<numThreads; ++i)
                pthread_join(threadIDsExperiment[i], NULL);

            // Gather data
            int longestRunExperiment = tdataExperiment[0].max;
            int startingPosExperiment = tdataExperiment[0].bestpos;

            for (int index = 1; index < numThreads; ++index)
            {
                if (tdataExperiment[index].max > longestRunExperiment)
                {
                    longestRunExperiment = tdataExperiment[index].max;
                    startingPosExperiment = tdataExperiment[index].bestpos;
                }
            }

            // End timer
            gettimeofday(&finish, &tzone);
            timeUsed = finish.tv_usec - start.tv_usec;
            // Write Data to file
            fprintf(fp, "%d,%d,%d,%lu\n", digit, buffLens[2], numThreads, timeUsed);
        }
    }

    fclose(fp);

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
        while (i + negIndex > data->start && sum >= 0)
        {
            sum -= data->A[i + negIndex];
            negIndex -= 1;
        }

        //
        if (sum + data->A[i + negIndex + 1] == 0)
        {
            if (-negIndex > maxLen)
            {
                maxLen = -negIndex;
                indexOfMax = i;
            }
        }
    }

    data->bestpos = indexOfMax;
    data->max = maxLen - 1;
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

int displayResults(int indexOfMax, int maxLength, int digits, char *buffer)
{
    printf("Longest desired run with %d digits: %d, at position %d\n", digits, maxLength, indexOfMax);

    for (int index = -(maxLength - digits); index < 0; ++index)
    {
        if (index == -1)
        {
            printf("%d = ", buffer[index + indexOfMax]);
        }
        else {
            printf("%d + ", buffer[index + indexOfMax]);
        }
    }
    for (int index = 0; index < digits; ++index)
    {
        printf("%d", buffer[indexOfMax + index]);
    }
    printf("\n");
}