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

int child1(int fd[]);
int child2(int fd[]);


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
			while(1){
				scanf("%d", &data_write.value);
				getchar(); /* Eat enter */
				if(data_write.value>0){
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
					break;
				}
			}
			
			printf("Starting to shut down childs\n");
			
			int child_pid;
			int status;
			
			data_write.value = 0;
			write(fd1[WRITE], &data_write, 1);
			//kill(pid1, SIGUSR1);
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

						
			data_write.value = 0;
			write(fd2[WRITE], &data_write, 1);			

			//kill(pid2, SIGUSR1);
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

	while(1){
		close(fd[WRITE]);
		bytesRead = read( fd[READ], &data_read,  1);
		//printf("test1\n");
		if(data_read.value > 0){
			sum += data_read.value;	
			printf("Read odd 1 %d sum: %d bytes: %d\n", data_read.value, sum, bytesRead);
			n++;
			}
		else {
			printf("Read odd exit 1 %d sum: %d bytes: %d\n", data_read.value, sum, bytesRead);
			break;
			}
		//sleep(1);
		}
	int avg = sum/n;
	exit(avg);
}

int child2(int fd[]){
	struct msg data_read;
	data_read.value = 0;
	int bytesRead;
	int sum = 0;
	int n = 0;
	printf("Child2, pid %d starting.\n", getpid());

	while(1){
		close(fd[WRITE]);
		bytesRead = read(fd[READ], &data_read,  1);
		//printf("test2\n");
		if(data_read.value > 0){
			sum += data_read.value;
			printf("Read even 2 %d sum: %d bytes: %d\n", data_read.value, sum, bytesRead);
			n++;
			//sleep(1);
			}
		else {
			printf("Read even exit 2 %d sum: %d bytes: %d\n", data_read.value, sum, bytesRead);
			break;
			}
		}
	int avg = sum/n;
	exit(avg);
}
