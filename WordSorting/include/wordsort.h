/*
*This file gives helper functions for the wordsort program
* © Sample copyright:
* blah blah blah
*/

#ifndef _WORDSORT_H
#define _WORDSORT_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

//Just for aesthetic purposes :)
#ifdef _cplusplus
extern "C" {
#endif

/* Structure defining a word and the number of times it occured on input*/

 typedef struct word_count_pair {
	char * w;
	long num_occurences;
} word_t;

//No other structures, definitions, or global variables to declare

//Functions

/*
 * int insert(word arr[], word new, size_t size)
 *
 * Helper function to insert a new word into a word array
 */
int insert(word_t * arr, word_t new, long size, long * max_occurences);


/*
 *lengthComp(const void * w1, const void * w2)
 *
 * Comparison function for sorting based on length
 */
int lengthComp(const void * w1, const void * w2);

/*
 *alphaComp(const void * w1, const void * w2)
 *
 * Comparison function for sorting based on alphabetical order
 */
int alphaComp(const void * w1, const void * w2);

/*
 *frequencyComp(const void * w1, const void * w2)
 *
 *Comparison function for sorting based on frequency
 */
int frequencyComp(const void * w1, const void * w2);

/* 
 * int sort()
 *
 * Wrapper for sorting function that counts the number of real elements present beforehand
 */
int sort(word_t * arr, size_t size, int freq, int alpha, int length, size_t word_size); 

/*
 *printHashes(long length)
 *
 *Helper function that prints <length> number of hashes
 */
void printHashes(long length); 


/* 
 *printHistogram(word arr[], size_t size)
 *
 *Prints the histogram for word occurence
 */
void printHistogram(word_t * arr, size_t size, struct winsize w, long max_occurences);


#ifdef _cplusplus
}
#endif

#endif /* _WORDSORT_H */
