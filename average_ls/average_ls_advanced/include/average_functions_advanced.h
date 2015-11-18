/*
 * This header file provides helper function definitions for the averageLSAdvanced
 * program.
 */

#ifndef AVERAGE_FUNCTIONS_ADVANCED_H
#define		AVERAGE_FUNCTIONS_ADVANCED_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

/*
 * recursive_search(char *, const char *, int, int)
 *
 * This function is the intial point for the recursive logic for
 * averageLSAdvanced. It intializes the FIFO for the recursion 
 * (the local var RECURSION_FIFO_PATH), sets the PRINT_FLAG
 * (if needed), and then calls the recursive_searcher logic
 * to perform the real hardwork. When the recursive_searcher returns,
 * this function takes the data from the searcher and gives it back
 * to the caller in a separate fifo specified by final_fifo_name.
 * This allows the separation of recurser logic with the caller's logic.
 */
int
recursive_search(char * pathname, 
	const char * final_fifo_name, 
	int print_flag, int average_filename_length);


#endif
