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

typedef struct {
    char *A;     // The digits themselves
    int start;   // first position for thread to look at
    int end;     // last position thread should look at
    int bestpos; // ending position of the best sequence found by this thread
    int max;     // length of the best sequence found by this thread
    int digits;
    int numChars;      // This is the length of A
} ThreadInfoExtended;

/*
 * This function read lines from a specified file and will put each of these lines into a large character buffer
 * On success it returns 0; otherwise a non-zero value.
 */
int readFile(char *fileName, int *numChars, char *buffer);

void *findMaxSumSeq(void *param);

/*
 * This function will find the max length and position in which consecutive digits
 * add up to the number starting at a position in the string of digits treated
 * as a base 10 number of length *digits*
 *
 * i.e: 2, 5, 7, 1, 4
 * 2 + 5 + 7 = 14
 *
 */
void *findMaxSumSeqExtended(void *param);

#endif //THREADS_THREADS_JKROGSRU_H
