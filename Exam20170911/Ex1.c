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
 
#define  READ   0
#define  WRITE  1 

void create_example_file();

int main() {	
	int fp[2];
	sem_t* sp;
	int id, retval, value, pid, bytes;

	/* Create example file */
	create_example_file();

	/* Open pipe */
	pipe(fp);
	
	/* Make shared memory segment. */
	id = shmget (IPC_PRIVATE, sizeof(sem_t), SEGMENTPERM);
	if (id == -1) perror("shmget error");


	/* Attach shared memory segment. */
	sp = (sem_t *) shmat(id,NULL, 0);
	if (*(int*) sp ==  -1) perror("shmat error");
	
	/* Initialize the semaphore. */
	retval = sem_init(sp,1,0);
	if (retval != 0) perror("sem_init error");


	pid = fork();
	if(pid < 0) perror("fork error");
	else if(pid == 0) { 
	/* Client */
		int fd;
		fd = open("data.txt", O_RDONLY);
		/* Start loop */
		while(1){
			bytes = read(fd, &value, sizeof(value));
			if(bytes == sizeof(value)){
				/* Send to server */
				bytes = write(fp[WRITE], &value, sizeof(value));
				if(bytes != sizeof(value)) perror("write error");
				}
			else{
				/* File empty */
				printf("File empty, closing.\n");
				kill(getppid(), SIGINT);
				close(fp[READ]);
				close(fp[WRITE]);
				close(fd);
				sem_destroy(sp);
				shmctl(id, IPC_RMID, 0);
				exit(0);
			}
			sem_wait(sp);
		}
	}
	else if(pid > 0){
	/* Server */
		int input;
		printf("Insert a number:\n");
		/* Start loop */
		while(1){
			scanf("%d", &input);
			getchar();
			bytes = read(fp[READ], &value, sizeof(value));
			if(bytes != sizeof(value)) perror("read error");
			if(input == value){
				/* Correct Input */
				printf("File: %d, correct input, closing.\n", value);
				kill(pid, SIGINT);
				close(fp[READ]);
				close(fp[WRITE]);
				sem_destroy(sp);
				shmctl(id, IPC_RMID, 0);
				exit(0);
			}
			else{
				/* Ask for another value */
				printf("File: %d, insert another number.\n", value);
				sem_post(sp);
			}
		}
	}
}	



void create_example_file(){
	int n = 5;
	int input;
	int fd = open("data.txt", O_WRONLY|O_CREAT,S_IRWXU);
	if(fd < 0) perror("error opening file");
	printf("Insert %d example data:\n", n);
	for(int i = 0; i < n; i++) {
			scanf("%d", &input);
			getchar();
			write(fd, &input, sizeof(int));
			}
	close(fd);
	printf("Example file created.\n");
}