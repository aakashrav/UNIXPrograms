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
	key_t key, key_two;
	key = ftok(FILE_NAME, 'A');
	key_two = ftok(FILE_NAME, 'B');

	int writesignaller;
	writesignaller = semget(key_two, 10, 0666 | IPC_CREAT);

	if (semctl(writesignaller, 0, SETVAL, 1) < 0)
	{
		error("Error with initiazling signaller");
	}

	pid_t pid = fork();
	if (pid == 0)
	{
		void * ptr;
		ptr = mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (ptr == MAP_FAILED)
			error("Error mmap file child");

		struct sembuf sem_child;
		sem_child.sem_num = 0;
		sem_child.sem_op = -1;
		sem_child.sem_flg = 0;
		strcat(ptr, "Hello world!");
		printf("Child is done writing!");
		fflush(stdout);
		if (semop(writesignaller, &sem_child, 0) < 0)
			error("Error releasing writesignaller");
		munmap(ptr, PAGE_SIZE);

	}
	else
	{
		struct sembuf sem_parent;
		sem_parent.sem_num = 0;
		sem_parent.sem_op = 0;
		sem_parent.sem_flg = 0;
		printf("Parent waiting for child!\n");
		fflush(stdout);

		if (semop(writesignaller, &sem_parent, 0) < 0)
			error("Error getting writesignaller parent");

		if ((val = semctl(writesignaller, 0, GETVAL)) < 0)
		{
			error("Error getting writesignaller value");
		}

		printf("Child finished!, proceeding with parent. Signaller value: %d\n", val);
		void * par_ptr = mmap(0, PAGE_SIZE, PROT_READ, MAP_SHARED, fd, 0);
		if (par_ptr == MAP_FAILED)
			error("Error mmap file parent");

		printf("Child wrote: %s\n", par_ptr);
		fflush(stdout);
		munmap(par_ptr, PAGE_SIZE);

	}
	semctl(writesignaller, 0, IPC_RMID);
}