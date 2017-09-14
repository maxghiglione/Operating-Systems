#include <fcntl.h>	// ipc macros
#include <sys/stat.h>   // fifo
#include <stdio.h> 	// printf
#include <stdlib.h> 	// exit
#include <semaphore.h>  // semaphore
#include <sys/types.h>	// sleep
#include <sys/ipc.h> 	// ipc
#include <sys/shm.h>    // shmem
#include <unistd.h> 	// process
#include <signal.h> 	// signal
#include <time.h>	// time
#include <fcntl.h>	// fcntl

#define SEGMENTPERM 0666
#define SEGMENTKEY 123456

int main()
{
	int fp;
	sem_t* sp;	
  	key_t key = SEGMENTKEY;
	pid_t client_pid;
	int id, retval, value, pid, bytes;
	char* npipe = "/tmp/fifopipe";
		
	/* Open pipe */
	fp = open(npipe, O_WRONLY);
	if (fp < 0) perror("open error");
	
	/* Make shared memory segment. */
	id = shmget (key, sizeof(sem_t), SEGMENTPERM);
	if (id == -1) perror("shmget error");

	/* Attach shared memory segment. */
	sp = (sem_t *) shmat(id,NULL, 0);
	if (*(int*) sp ==  -1) perror("shmat error");

	/* Write client pid */
	client_pid = getpid();
	bytes = write(fp, &client_pid, sizeof(client_pid));
	if(bytes != sizeof(client_pid)) perror("write error");

	int fd;
	fd = open("data.txt", O_RDONLY);

	/* Start loop */
	while(1){
		bytes = read(fd, &value, sizeof(value));
		if(bytes == sizeof(value)){
			/* Send to server */
			bytes = write(fp, &value, sizeof(value));
			if(bytes != sizeof(value)) perror("write error");
		}
		else if(bytes == 0){
			/* File empty */
			printf("File empty, closing.\n");
			close(fp);
			close(fd);
			unlink(npipe);
			shmdt(sp);
			exit(0);
		}
		else perror("read error");

		sem_wait(sp);
	}
}