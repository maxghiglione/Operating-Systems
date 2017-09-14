#include <signal.h> /*kill*/
#include <stdio.h>
#include <unistd.h> /* getpid */
#include <stdlib.h> /*exit */


int main()
{   
   if(fork() == 0 )
   {
	printf("P1 (pid=%d): \n", getpid());
	sleep(5);
	kill(getppid(), SIGINT);
	printf("P1 (pid=%d): \n", getpid());
	exit(0);  
	printf("P1 (pid=%d): \n", getpid()); 
   }
   else    
   {  
	printf("P2 (pid=%d): \n", getpid());
        for(;;);
	printf("P2 (pid=%d): \n", getpid());
   }
   printf("P2 (pid=%d): \n", getpid());
   return 0;
}

