//
// Created by Jared Krogsrud on 2/24/2023.
//

#ifndef THREADS_THREADS_JKROGSRU_H
#define THREADS_THREADS_JKROGSRU_H


#define BUFLEN 10100
#define NUM_THREADS 4
#define LINELEN 256

/*
 * This function read lines from a specified file and will put each of these lines into a large character buffer
 * On success it returns 0; otherwise a non-zero value.
 */
int readFile(char *fileName, int *numChars, char *buffer);

void *findMaxSumSeq(void *param);

#endif //THREADS_THREADS_JKROGSRU_H
