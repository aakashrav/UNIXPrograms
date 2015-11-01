#include "include/wordsort.h"

/*
 * int insert(word_t arr[], word_t new, size_t size)
 *
 * Helper function to insert a new word into a word array
 */
int insert(word_t * arr, word_t new, long size, long * max_occurences) {
	int exists =0;
	int i=0;

	//For the initial case where we have no elements
	if (size != 0) {
		for (i=0; i < size; i++) {
			if (!strcmp(arr[i].w,new.w)) {
				arr[i].num_occurences++;
				if (arr[i].num_occurences > *max_occurences)
					*(max_occurences)=arr[i].num_occurences;
				exists=1;
				break;
			}
		}
	}

	if (!exists) {
		arr[i] = new;
		arr[i].num_occurences = 1;
	}

	return exists;
}

/*
 *lengthComp(const void * w1, const void * w2)
 *
 * Comparison function for sorting based on length
 */
int lengthComp(const void * w1, const void * w2) {
	word_t * wordone = (word_t *)w1;
	word_t * wordtwo = (word_t *)w2;

	if (strlen(wordone->w) < strlen(wordtwo->w))
		return 1;
	else 
		return -1;

	return 0;
}

/*
 *alphaComp(const void * w1, const void * w2)
 *
 * Comparison function for sorting based on alphabetical order
 */
int alphaComp(const void * w1, const void * w2) {
	word_t * wordone = (word_t *)w1;
	word_t * wordtwo = (word_t *)w2;

	if (wordone->w[0] < wordtwo->w[0])
		return 1;
	else 
		return -1;

	return 0;
}

/*
 *frequencyComp(const void * w1, const void * w2)
 *
 *Comparison function for sorting based on frequency
 */
int frequencyComp(const void * w1, const void * w2) {
	word_t * wordone = (word_t *)w1;
	word_t * wordtwo = (word_t *)w2;

	if (wordone->num_occurences < wordtwo->num_occurences)
		return 1;
	else 
		return -1;

	return 0;
}

/* 
 * int sort()
 *
 * Wrapper for sorting function that counts the number of real elements present beforehand
 */
int sort(word_t * arr, size_t size, int freq, int alpha, int length, size_t word_size) {

	if (freq==1)
		qsort(arr, size, word_size, frequencyComp);
	else if (alpha ==1)
		qsort(arr, size, word_size, alphaComp);
	else
		qsort(arr, size, word_size, lengthComp);

	return 0;

}

/*
 *printHashes(long length)
 *
 *Helper function that prints <length> number of hashes
 */
void printHashes(long length) {

	for (int i=0; i < length; i++) {
		printf("#");
	}
}

/* 
 *printHistogram(word arr[], size_t size)
 *
 *Prints the histogram for word occurence
 */
void printHistogram(word_t * arr, size_t size, struct winsize w, long max_occurences) {
	for (int i=0; i < size; i++) {
		printf("%s|", arr[i].w);

		double proportion;

		if (max_occurences != 0)
			proportion = (double)(arr[i].num_occurences)/(double)max_occurences;
		else
			proportion = 1;

		long num_hashes = (w.ws_col-20) * proportion;

		printHashes(num_hashes);
		printf("|%ld\n", (arr[i]).num_occurences);
	}
	fflush(stdout);
}
