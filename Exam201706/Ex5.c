#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>

#define MAX_AVG 27

void parent_handler();
void children_handler();

pid_t pids[3];

int main(){
	int pipes[6];

	pipe(&pipes[0]);
	pipe(&pipes[2]);
	pipe(&pipes[4]);

	for(int i=0;i<3;i++){
		pids[i] = fork();
		if(pids[i]==0){
			signal(SIGUSR1,children_handler);
			float A = 20;
			float B = 0;
			float avg = A;			
                	srand(time(NULL)*getpid());              // *** ADDED
			while(1){
				write(pipes[2*i+1],&avg,sizeof(float));
				B = rand()%21+10;
				read(pipes[(2*i+4)%6],&A,sizeof(float)); // *** ERR
				avg = (A+B)/2.0;
				printf("[%d] a: %f b: %f avg: %f\n",getpid(), A, B, avg);
				if(avg>MAX_AVG){
  					kill(getppid(),SIGUSR1);
					close(pipes[2*i+1]);
					printf("CHILD %d calls quit\n",getpid());
					exit(0);				
				}	
			}
		}
	}

	signal(SIGUSR1,parent_handler);
	wait(NULL);
	wait(NULL);
	for(int i=0;i<6;i++) close(pipes[i]);
	return 0;
}

void children_handler(){
   	printf("CHILD %d quit\n",getpid());
	exit(0);
}

void parent_handler(){
	int first_exited = wait(NULL);
	for(int i=0;i<3;i++){
		if(pids[i]==first_exited) continue;
		kill(pids[i],SIGUSR1);
	}
}

