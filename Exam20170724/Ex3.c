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
for(i = 0; i < 2; i++)
	pid = fork();

printf("f: %d - i: %d - p: %d\n", pid, getpid(), getppid());
//if(pid == 0) printf("%d - %d hello \n", pid, getpid());

return 0;
}
