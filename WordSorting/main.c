#include "include/wordsort.h"
#include <stdlib.h>
#include <string.h>

long MAX_WORDS = 1;
long MAX_INPUT_SIZE = 10;

typedef struct options {
	int min;
	int max;
	int freq;
	int alpha;
	int length;
} opt_struct;

opt_struct options;

static const char * optstring ="m:n:falh";

void usage() {
	printf("Usage: wordsort [-f] [-a] [-l]\n");
	printf("[-f] sorts on frequency\n");
	printf("[-a] sorts for alphabetical order\n");
	printf("[-l sorts for length\n");
	printf("[-m] considers only words of length greater than the argument\n");
	printf("[-n considers only words of length less than the argument\n");
}


int main(int argc, char * argv[]) {

	/*
	 * We accept if and only if we have at most 3 options: 1 for max length, 
	 * 1 for min length
	 * and 1 for sorting type.
	 *
	 * We also want atleast one argument so we make sure argc>1
	 */

	if (argc>7) {
		usage();
		exit(1);
	}

	char opt;
	opt = getopt(argc, argv, optstring);

	word_t * arr= malloc(MAX_WORDS * sizeof(word_t));


	while (opt!=-1) {
		switch(opt) {
			case 'm':
				options.min = atoi(optarg);
				break;
			case 'n':
				options.max = atoi(optarg);
				break;
			case 'f':
				options.freq =1;
			case 'a':
				options.alpha=1;
			case 'l':
				options.length=1;
			// case 'h':
			//  	usage();
			//  	return 0;
			}
		opt = getopt(argc, argv, optstring);
	}

	printf("Enter your string:\n");


	char * para = malloc(sizeof(char) * MAX_INPUT_SIZE);
	int index;
	if (para != 0) {
		char c = EOF;
		while ((c = getchar()) != '\n' && c != EOF) {
			
			para[index++] = (char)c;

			if (index == MAX_INPUT_SIZE) {
				MAX_INPUT_SIZE += 100;
				para = realloc(para, sizeof(char) * MAX_INPUT_SIZE);
			}
		}
	}
	else {
		printf("Problem mallocing");
		exit(1);
	}

	char * pch;
	pch = strtok(para, " ");
	long num_words = 0;

	//keep track of the maximum number of occurences to scale our histogram
	long max_occurences = 0;

	while (pch != NULL) {

		if (!(options.min==0 && options.max==0)) {
			if ((strlen(pch) < options.min) || (strlen(pch) > options.max)) {
				pch = strtok(NULL," ");
				continue;
			}
		}

		word_t wo;
		wo.w = pch;

		int exists = insert(arr, wo, num_words, &max_occurences);
		if (!exists)
			num_words++;

		if (num_words == MAX_WORDS) {
			MAX_WORDS = MAX_WORDS + 50;
			arr = realloc(arr, MAX_WORDS * sizeof(word_t));
		}

		//realloc failure
		if (arr == 0) {
			printf("Problem with realloc");
			exit(1);
		}
		
		pch = strtok(NULL," ");
	}

	sort(arr, num_words, options.freq, options.alpha, 
		options.length, sizeof(*arr));

	struct winsize w;

	if (ioctl(2, TIOCGWINSZ, &w) == 0)
		printHistogram(arr, num_words, w, max_occurences);
	else 
		perror("An error occured");

	free(para);
	free(arr);

	//prevent double deallocation, just in case
	para = 0;
	arr = 0;

	return (0);

}
