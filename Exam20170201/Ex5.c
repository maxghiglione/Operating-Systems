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
#define N 10
#define M 3
 
#define  READ   0   /* The index of the “read” end of the pipe */
#define  WRITE  1    /* The index of the “write” end of the pipe */

struct msg {
	int value;
	};

void child(int fd[], sem_t* sp);



int main()
{	
	sem_t* sp;
	
	/* Make shared memory segment. */
	int id = shmget (IPC_PRIVATE, sizeof(sem_t), SEGMENTPERM);
	if (id == -1) printf("shget error\n");

	sp = (sem_t *) shmat(id,NULL, 0);
	if (*(int*) sp ==  -1) printf("shmat error\n");
	
	/* Initialize the semaphore. */
	int retval = sem_init(sp,1,1);
	if (retval != 0) printf("sem_init error\n");


	/*child1*/
	int fd1[2];
	pipe(fd1);
	if(fcntl(fd1[0],F_SETFL,O_NONBLOCK)<0) printf("fcntl error");
	int pid1 = fork();
	if (pid1 == 0) child(fd1, sp);
	else if(pid1 < 0) printf("Fork1 failure\n");
	
	/*child2*/
	int fd2[2];	
	pipe(fd2);
	if(fcntl(fd2[0],F_SETFL,O_NONBLOCK)<0) printf("fcntl error");
	int pid2 = fork();
	if (pid2 == 0) child(fd2, sp);
	else if(pid2 < 0) printf("Fork2 failure\n");
	

	/*parent*/	
	printf("[%d] Parent starting.\n", getpid());
	int flag = 0;
	int tmp = 0;
	int nbyte = sizeof(tmp);
	int nread = nbyte;
	int data1 = -1;
	int data2 = -1;
	int data1_old = -1;
	int data2_old = -1;
	printf("[%d] byte: %d\n", getpid(), nbyte);
	while(!flag){		
		/* read child1 */
		//printf("[%d] Parent starts reading %d.\n", getpid(), pid1);	
		while(nread == nbyte){
			nread = read(fd1[0], &tmp, nbyte);
			//printf("[%d] read: %d n: %d\n", getpid(), data1, nread);
			if(nread == nbyte){
				data1 = tmp;
				if(data1 == data1_old){
					flag = 1;
				}
				else{
					data1_old = data1;
				}
			}
		}
		nread = nbyte; 
		//printf("[%d] Parent stops reading %d.\n", getpid(), pid1);
		
			
		/* read child2 */
		//printf("[%d] Parent starts reading %d.\n", getpid(), pid2);
		
		while(nread == nbyte){
			nread = read(fd2[0], &tmp, nbyte);
			//printf("[%d] read: %d n: %d\n", getpid(), data2, nread);
			if(nread == nbyte){
				data2 = tmp;
				if(data2 == data2_old){
					flag = 1;
				}
				else{
					data2_old = data2;
				}
			}
		}
		nread = nbyte; 
		//printf("[%d] Parent stops reading %d.\n", getpid(), pid2);
		
		
		/*sleep*/
		sleep(M/2);
	}
	close(fd1[READ]);
	close(fd2[READ]);
	close(fd1[WRITE]);
	close(fd2[WRITE]);
	printf("[%d] Parent closing.\n", getpid());
	/* close */
	kill(pid1, SIGINT);
	kill(pid2, SIGINT);
	exit(0);
}
	

void child(int fd[], sem_t* sp){
	
	srand(time(NULL)*getpid());
	int data = 0;
	int sleep_time;
	printf("[%d] Child starting.\n", getpid());
	
	while(1){
		//sem_wait(sp);
		data = rand()%N;
		sleep_time = rand()%M + 1;
		printf("[%d] value: %d time: %d\n", getpid(), data, sleep_time);
		close(fd[READ]);
		write(fd[WRITE], (void*) &data, sizeof(data));
		//sem_post(sp);
		
		/*sleep*/
		sleep(sleep_time);
		}
}



/*
	//parent	
	printf("[%d] Parent starting.\n", getpid());
	int flag = 0;
	int bytesread = sizeof(int);
	int data1 = 0;
	int data2 = 0;
	printf("[%d] byte: %d\n", getpid(),bytesread);
	while(!flag){		
		// read child1 
		printf("[%d] Parent starts reading %d.\n", getpid(), pid1);	
		close(fd1[WRITE]);
		while( bytesread == sizeof(int)){
			bytesread = read(fd1[0], &data1, 1);
			printf("[%d] read: %d byte: %d\n", getpid(), data1, bytesread);
			}
		bytesread = sizeof(int); 
		printf("[%d] Parent stops reading %d.\n", getpid(), pid1);
		close(fd1[READ]);
			
		// read child2
		printf("[%d] Parent starts reading %d.\n", getpid(), pid2);
		close(fd2[WRITE]);
		while( bytesread == sizeof(int)){
			bytesread = read(fd2[0], &data2, 1);
			printf("[%d] read: %d byte: %d\n", getpid(), data2, bytesread);
			}
		bytesread = sizeof(int); 
		printf("[%d] Parent stops reading %d.\n", getpid(), pid2);
		close(fd2[READ]);
		
		if(data2==0 && data1==0) flag=1; //DEBUG
		//sleep
		sleep(M/2);
	}

*/
