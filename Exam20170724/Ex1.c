#include <stdio.h> 	// printf, open, read
#include <pthread.h> 	// pthread
#include <unistd.h> 	// process
#include <stdlib.h> 	// exit
#include <sys/types.h> 	// wait, sleep
#include <sys/wait.h> 	// wait
#include <semaphore.h> 	// semaphore

int main()
{
if(fork() == 0)
{
printf("1 %d\n",getpid());
sleep(1);
kill(getppid(), SIGINT);
printf("1 %d\n",getpid());
exit(1);
}
else{

printf("2 %d\n",getpid());
while(1);
printf("2 %d\n",getpid());
}
return 0;
}

