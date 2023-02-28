//
// Created by Jared Krogsrud on 2/24/2023.
//

#ifndef THREADS_THREADS_JKROGSRU_H
#define THREADS_THREADS_JKROGSRU_H


#define BUFLEN 10100
#define NUM_THREADS 4
#define LINELEN 256
#define PADDING 11

typedef struct {
    char *A;     // The digits themselves
    int start;   // first position for thread to look at
    int end;     // last position thread should look at
    int bestpos; // ending position of the best sequence found by this thread
    int max;     // length of the best sequence found by this thread
} ThreadInfo;

/*
 * This function read lines from a specified file and will put each of these lines into a large character buffer
 * On success it returns 0; otherwise a non-zero value.
 */
int readFile(char *fileName, int *numChars, char *buffer);

void *findMaxSumSeq(void *param);

#endif //THREADS_THREADS_JKROGSRU_H
