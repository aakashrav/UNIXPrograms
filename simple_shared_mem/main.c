#include <sys/types.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char FILE_NAME [] = "/tmp/private_fileXXXXXX";

void error(const char * message)
{
	perror(message);
	exit(1);
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

	key_t key, key_two;
	key = ftok(FILE_NAME, 'A');
	key_two = ftok(FILE_NAME, 'B');

	/**
	 * Here we create two synchronization primitives, one to control access of
	 * the shared memory itself, and another to signal whether anything has been
	 * written to the shared memory
	*/
	int access_synchronizer,writesignaller;
	access_synchronizer = semget(key, 10, 0666 | IPC_CREAT);
	writesignaller = semget(key_two, 10, 0666 | IPC_CREAT);

	if (semctl(writesignaller, 0, SETVAL, 1) < 0)
	{
		error("Error with initiazling signaller");
	}

	if (semctl(access_synchronizer, 0, SETVAL, 0) < 0)
	{
		error("Error with initiazling access_synchronizer");
	}

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
				error("Error setting writesignaller child");

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
			printf("Parent waiting for child!\n");
			fflush(stdout);

			sem_parent.sem_op = 0;
			if (semop(writesignaller, &sem_parent, 1) < 0)
				error("Error getting writesignaller parent");
			printf("got here!");
			fflush(stdout);
			sem_parent.sem_op = 0;
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

		semctl(writesignaller, 0, IPC_RMID);
		munmap(par_ptr, PAGE_SIZE);

	}
}