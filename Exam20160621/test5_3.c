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
#define  EVEN 2
#define  ODD 0


int sum = 0;
int n = 0;
int avg = 0;

void handler(){
	if(n != 0) avg = sum/n;
	printf("[%d] child exit\n", getpid());
	exit(avg);
}

int main()
{
	// creating //
	int value = 0;
	size_t size = sizeof(int);
	int fd[4];
	pid_t pid[2];
	
	if(pipe(&fd[0]) < 0) printf("Pipe failure\n");
	if(pipe(&fd[2]) < 0) printf("Pipe failure\n");

	for(int i=0;i<2;i++){
		pid[i] = fork();
		if(pid[i] == 0){
			//child
			int bytes_read = 0;
			signal(SIGUSR1,handler);
			printf("[%d] child %d start\n", getpid(), 2*i);
			while(1){
				if(read(fd[2*i], &value,  size) < 0) {
					printf("[%d] Read %d failure\n", getpid(), 2*i);
				}
				else {
					printf("[%d] child %d read\n", getpid(), i);
					sum += value;
					n++;
				}
			}
		}
		else if (pid[i] < 0) printf("Fork failure\n");
		else printf("[%d] child %d created\n", getpid(), 2*i);
	}
	
	// looping //
	int flag = 0;
	int is_odd = 0;
	int is_neg = 0;
	
	while(!is_neg){
		scanf("%d", &value);
		getchar();
		
		if(value > 0){
			is_odd = ((int)value)%2;
			printf("[%d] writing %d to child %d\n", getpid(), value, is_odd*2);
			write(fd[is_odd*2 + 1], &value, size);
			//close(fd[is_odd*2 + 1]);
		}
		else{
			printf("[%d] value %d is negative\n", getpid(), value);
			is_neg = 1;
		}
	}
	
	// closing //
	int child_pid;
	int status;
	
	for(int i=0;i<2;i++){
		kill(pid[i], SIGUSR1);
		printf("[%d] waiting child %d\n", getpid(), 2*i);
		child_pid = waitpid(pid[i], &status, 0);
		if(WIFEXITED(status)) 
			{
			avg = WEXITSTATUS(status);
			printf("[%d] child %d average is: %d\n", getpid(), 2*i, avg);
			}
		else printf("error exiting\n");
	}
	for(int i=0;i<4;i++) close(fd[i]);
}
