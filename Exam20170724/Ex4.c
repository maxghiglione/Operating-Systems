#include <stdio.h> 	// printf, open, read
#include <pthread.h> 	// pthread
#include <unistd.h> 	// process
#include <stdlib.h> 	// exit
#include <sys/types.h> 	// wait, sleep
#include <sys/wait.h> 	// wait
#include <semaphore.h> 	// semaphore

int main()
{
	 int i, pid;
	 printf("\n");
	
	 for(i = 0; i < 2; i++){
		pid = fork();
		if( pid !=0){
		printf("%d) parent f: %d - i: %d - p: %d\n", i,  pid, getpid(), getppid());
		wait(NULL);
		//exit(0);
		return 1;
		}
		else{
			printf("%d) child f: %d - i: %d - p: %d\n", i,  pid, getpid(), getppid());
			if(i > 0){
				exit(0);
			}
		}
	 }

return 0;
}
