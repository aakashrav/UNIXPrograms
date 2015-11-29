#include <sys/types.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

char FILE_NAME [] = "/tmp/private_fileXXXXXX";

int access_synchronizer, writesignaller, end_signaller;

void error(const char * message)
{
	perror(message);
	printf("Error number: %d\n", errno);
	exit(errno);
}

int main(int argc, char * argv[])
{
	int val;
	const long PAGE_SIZE = sysconf(_SC_PAGE_SIZE);
	int fd = mkstemp(FILE_NAME);
	int i = ftruncate(fd, PAGE_SIZE);
	if (i < 0)
	{
		error("Error truncating memory mapped file");
	}

	key_t key, key_two, key_three, key_four;
	key = ftok(FILE_NAME, 'A');
	key_two = ftok(FILE_NAME, 'B');
	key_three = ftok(FILE_NAME, 'C');
	key_four = ftok(FILE_NAME, 'D');

	/**
	 * Here we create two synchronization primitives, one to control access of
	 * the shared memory itself, another to signal whether anything has been
	 * written to the shared memory, and a final to signal to the parent program
	 * that the child process has ended and therefore the program should stop.
	*/
	access_synchronizer = semget(key, 10, 0666 | IPC_CREAT);
	writesignaller = semget(key_two, 10, 0666 | IPC_CREAT);
	end_signaller = semget(key_three, 10, 0666 | IPC_CREAT);
	// parent_end_signaller = semget(key_four, 10, 0666| IPC_CREAT);

	if (semctl(writesignaller, 0, SETVAL, 1) < 0)
	{
		error("Error with initiazling signaller");
	}

	if (semctl(access_synchronizer, 0, SETVAL, 0) < 0)
	{
		error("Error with initiazling access_synchronizer");
	}

	if (semctl(end_signaller, 0, SETVAL, 0) < 0)
	{
		error("Error with initializing end_signaller");
	}

	// if (semctl(parent_end_signaller, 0, SETVAL, 0) < 0)
	// {
	// 	error("Error with initializing end_signaller");
	// }

	pid_t pid;
	if ( (pid =fork()) == -1)
	{
		error("Error on fork!");
	}

	// Fork a child that will do the writing

	if (pid == 0)
	{
		void * ptr;
		ptr = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (ptr == MAP_FAILED)
			error("Error mmap file child");
		struct sembuf sem_child;
		sem_child.sem_num = 0;
		sem_child.sem_flg = 0;

		for (int i=0; i < 5; i++)
		{

			/**
			 * Here we test whether the parent has finished.
			 * If it has, we break.
			 */
			int ending;
			if ( (ending = semctl(end_signaller, 0, GETVAL)) < 0)
			{
				error("Error getting value of parent's end_signaller");
			}

			if (ending == 1)
				break;

			/**
			 * We first set the writesignaller semaphore to 0, so that the parent
			 * knows that the child is currently writing to the semaphroe and won't
			 * prematurely try to read the semaphore
			 */
			sem_child.sem_op = -1;
			if (semop(writesignaller, &sem_child, 1) < 0)
				error("Error getting writesignaller child");
			/**
			 * We have a two phase access of the semaphore: in the first phase we 
			 * gain the semahpore only if its 0, then after gaining it set it to 1
			 * so that the other parent/child process can't gain the semaphore.
			 * This will prevent simultaneous access to the resource
			 */
			sem_child.sem_op = 0;
			if (semop(access_synchronizer, &sem_child, 1) < 0)
				error("Error getting access_synchronizer child");
			sem_child.sem_op = 1;
			if (semop(access_synchronizer, &sem_child, 1) < 0)
				error("Error setting access_synchronizer child");

			strcat(ptr, "Hello world!");

			/**
			 * We first decrement the access_synchronizer to signal back to 0
			 * so that the other process who's been waiting for it to become 0 can access
			 * the critical section. Secondly, we SHOULD increment the write_synchronizer back to 
			 * 0 to let the parent know something has been written.
			 */
			sem_child.sem_op = -1;
			if (semop(access_synchronizer, &sem_child, 1) < 0)
				error("Error releasing access_synchronizer child");
			sem_child.sem_op = 1;
			if (semop(writesignaller, &sem_child, 1) < 0)
				error("Error releasing writesignaller child");
		}

		sem_child.sem_op = 1;
		if (semop(end_signaller, &sem_child, 1) < 0)
			error("Error setting end signaller child");
		/** 
		 * Perform one final unlocking scheme so that the parent doesn't deadlock.
		 * A simple phony write signal/acces_synchronizer signal will suffice
		 */
		if (semctl(writesignaller, 0, SETVAL, 0) < 0)
		{
			error("Error setting final phony write signal child");
		}
		if (semctl(access_synchronizer, 0, SETVAL, 0) < 0)
		{
			error("Error setting final phony access_synchronizer signal child");
		}
		printf("Child finished!");
		fflush(stdout);
		munmap(ptr, PAGE_SIZE);

	}
	else
	{
		struct sembuf sem_parent;
		sem_parent.sem_num = 0;
		sem_parent.sem_flg = 0;

		void * par_ptr = mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, 0);
		if (par_ptr == MAP_FAILED)
			error("Error mmap file parent");

		for (int j =0; j < 5; j++)
		{
			/**
			 * Here we test whether the child has finished.
			 * If it has, we break.
			 */
			int ending;
			if ( (ending = semctl(end_signaller, 0, GETVAL)) < 0)
			{
				error("Error getting value of child's end_signaller");
			}

			if (ending == 1)
				break;

			printf("Parent waiting for child!\n");
			fflush(stdout);

			sem_parent.sem_op = 0;
			if (semop(writesignaller, &sem_parent, 1) < 0)
				error("Error getting writesignaller parent");
			if (semop(access_synchronizer, &sem_parent, 1) < 0)
				error("Error getting access_synchronizer parent");
			sem_parent.sem_op= 1;
			if (semop(access_synchronizer, &sem_parent, 1) < 0)
				error("Error getting access_synchronizer parent");

			printf("Child wrote: %s\n", par_ptr);
			fflush(stdout);

			sem_parent.sem_op = -1;
			if (semop(access_synchronizer, &sem_parent, 1) < 0)
				error("Error releasing access_synchronizer parent");

		}

		sem_parent.sem_op = 1;
		if (semop(end_signaller, &sem_parent, 1) < 0)
			error("Error setting end signaller parent");

		/**
		 * As in the child, we perform one final phony unlocking scheme
		 * so the child will not lock
		 */
		if (semctl(access_synchronizer, 0, SETVAL, 0) < 0)
		{
			error("Error setting final phony access_synchronizer signal parent");
		}
		/** 
		 * Wait for the child to finish unlocking all semaphores before we 
		 * prematurely finish the program
		 */
		printf("Parent finished; waiting for child to finish\n");

		int val;
			if ( (val = semctl(access_synchronizer, 0, GETVAL)) < 0)
			{
				error("Error getting value of child's end_signaller");
			}
		printf("access_synchronizer at time of parent exit: %d\n", val);
			if ( (val = semctl(writesignaller, 0, GETVAL)) < 0)
			{
				error("Error getting value of child's end_signaller");
			}
		printf("writesignaller at time of parent exit: %d\n", val);


		fflush(stdout);
		int status;
		waitpid(pid, &status, 0);

		semctl(writesignaller, 0, IPC_RMID);
		semctl(access_synchronizer, 0, IPC_RMID);
		semctl(end_signaller, 0, IPC_RMID);
		munmap(par_ptr, PAGE_SIZE);

	}
}