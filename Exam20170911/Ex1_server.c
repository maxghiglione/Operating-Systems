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

#define SEGMENTPERM 0666
#define SEGMENTKEY 123456

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
};

int main()
{
	int fp;
	sem_t* sp;	
  	key_t key = SEGMENTKEY;
	pid_t client_pid;
	int id, retval, value, pid, bytes;
	char* npipe = "/tmp/fifopipe";

	/* Create example file */
	create_example_file();

	/* Create named pipe*/
	retval = mkfifo(npipe, 0666);
	if (retval < 0) perror("mkfifo error");

	/* Open client and keep in backgroud */
	system("./Ex2_client &"); 

	/* Open pipe */
	fp = open(npipe, O_RDONLY);
	if (fp < 0) perror("open error");
	
	/* Make shared memory segment. */
	id = shmget (key, sizeof(sem_t), IPC_CREAT | SEGMENTPERM);
	if (id == -1) perror("shmget error");

	/* Attach shared memory segment. */
	sp = (sem_t *) shmat(id,NULL, 0);
	if (*(int*) sp ==  -1) perror("shmat error");
	
	/* Initialize the semaphore. */
	retval = sem_init(sp,1,0);
	if (retval != 0) perror("sem_init error");

	/* Read client pid */	
	bytes = read(fp, &client_pid, sizeof(client_pid));
	if(bytes != sizeof(client_pid)) perror("read error");


	int input;
	printf("Insert a number:\n");
	/* Start loop */
	while(1){
		bytes = read(fp, &value, sizeof(value));
		if(bytes == sizeof(value)){
			/* Pipe message is ok */
			scanf("%d", &input);
			getchar();
			if(input == value){
				/* Correct Input */
				printf("File: %d, correct input, closing.\n", value);
				kill(client_pid, SIGINT);
				close(fp);
				unlink(npipe);
				sem_post(sp);
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
		else if(bytes == 0){ 
			/* File empty */
			printf("Client exited, closing.\n");
			close(fp);
			unlink(npipe);
			sem_destroy(sp);
			shmdt(sp);
			shmctl(id, IPC_RMID, 0);
			exit(0);
		}
		else perror("read error");
	}
}