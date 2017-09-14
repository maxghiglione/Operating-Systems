#include <stdio.h>	/* printf 		*/
#include <stdlib.h> 	/* exit 		*/
#include <sys/types.h>
#include <sys/wait.h> 	/* wait 		*/
#include <pthread.h>	/* threads 		*/
#include <unistd.h> 	/* POSIX (getpid)	*/


int value = 0; 

void *runner() 
{      
	value = 5;
	pthread_exit(0);
}

int main()
{
	pid_t pid;
	pthread_t tid; 
	printf("MAIN: value = %d\n",value);

	pid = fork();
	if (pid == 0) { 
		pthread_create(&tid, NULL,runner,NULL);
		pthread_join(tid,NULL);
		printf("PID = 0: value = %d\n",value); 
		exit(0);
	}
	else { 
		wait(NULL);
		printf("PID <> 0: value = %d\n",value); 
        }
        printf("MAIN: value = %d\n",value);
}

// MAIN: value = 0
// PID = 0: value = 5
// PID <> 0: value = 0
// MAIN: value = 0
