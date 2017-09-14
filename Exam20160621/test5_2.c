#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define  READ   0   /* The index of the “read” end of the pipe */
#define  WRITE  1    /* The index of the “write” end of the pipe */

struct msg {
	int value;
	};

void handler1(int sig);
void handler2(int sig);

int child1(int fd[]);
int child2(int fd[]);

int flag1 = 0;
int flag2 = 0;

int main()
{
	int avg = 0;
	int value = 0;

	int fd1[2];
	pipe(fd1);
	int fd2[2];
	pipe(fd2);

	pid_t pid1, pid2;
	pid1 = fork();
	if(pid1 < 0){
		printf("Fork failure\n");
		exit(-1);
	}
	else if(pid1 == 0){
		child1(fd1);	
	}
	else{	
		pid2 = fork();
		if(pid1 < 0){
			printf("Fork failure\n");
			exit(-1);
		}	
		else if(pid2 == 0){
			child2(fd2);
		}
		else{	
			struct msg data_write;
			data_write.value = 0;
			while(data_write.value>=0){
				scanf("%d", &data_write.value);
				getchar(); /* Eat enter */
				if(data_write.value>=0){
					if(data_write.value%2){ /* Odd */
						close(fd1[READ]);
						printf("writing to child1\n");
						write(fd1[WRITE], &data_write, 1); /* Send */
						
					}
					else{
						close(fd2[READ]);
						printf("writing to child2\n");
						write(fd2[WRITE], &data_write, 1); /* Send */
						
					}
				}
				else{
					printf("Negative number %d.\n", data_write.value);
				}
			}
			
			printf("Starting to shut down childs\n");
			
			int child_pid;
			int status;

			kill(pid1, SIGUSR1);
			//flag1 = 1;
			child_pid = waitpid(pid1, &status, 0);
			if(WIFEXITED(status)) 
				{
				int avg = WEXITSTATUS(status);
				printf("Child1: %d, Odd average is: %d\n",child_pid, avg);
				}
			else
				{
				printf("error exiting\n");
				}
			
			kill(pid2, SIGUSR1);
			//flag2 = 1;
			child_pid = waitpid(pid2, &status, 0);
			if(WIFEXITED(status)) 
				{
				int avg = WEXITSTATUS(status);
				printf("Child2: %d, Even average is: %d\n",child_pid, avg);
				}
			else
				{
				printf("error exiting\n");
				}
		}
	}
	exit(0);
}

int child1(int fd[]){
	struct msg data_read;
	data_read.value = 0;
	int bytesRead;
	int sum = 0;
	int n = 0;
	printf("Child1, pid %d starting.\n", getpid());
	signal(SIGUSR1,handler1);
	while(flag1 == 0){
		close(fd[WRITE]);
		bytesRead = read( fd[READ], &data_read,  1);
		//printf("test1\n");
		sum += data_read.value;	
		printf("Read odd 1 %d sum: %d bytes: %d, flag: %d \n", data_read.value, sum, bytesRead, flag1 );
		n++;
		//sleep(1);
		}
	
	int avg = sum/n;
	printf("Child 1 %d\n", avg);
	close(fd[READ]);
	
	
	
	printf("Child 1 exiting\n");
	exit(avg);
}

int child2(int fd[]){
	struct msg data_read;
	data_read.value = 0;
	int bytesRead;
	int sum = 0;
	int n = 0;
	printf("Child2, pid %d starting.\n", getpid());
	signal(SIGUSR2,handler2);
	while(flag2 == 0){
		close(fd[WRITE]);
		bytesRead = read(fd[READ], &data_read,  1);
		//printf("test2\n");
		sum += data_read.value;
		printf("Read even 2 %d sum: %d bytes: %d flag: %d \n", data_read.value, sum, bytesRead, flag2);
		n++;
		//sleep(1);
		}
	
	int avg = sum/n;
	printf("Child 2 %d\n", avg);
	close(fd[READ]);
	
	
	
	printf("Child 2 exiting\n");
	exit(avg);
}


void handler1(int sig){
	flag1 = (sig == SIGUSR1);
	printf("Child 1 signal\n");
}

void handler2(int sig){
	flag2 = (sig == SIGUSR2);
	printf("Child 2 signal\n");
}
