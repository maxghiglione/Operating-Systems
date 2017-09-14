#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <stdlib.h>

int main (){
	int i, status;
	for(i=0;i<3;i++)
		{
		if(fork() == 0){
	printf("Child: %d: Hello World!", i);
			exit (0);
	}
	else{
		printf("Parent: %d: Hello World!\n",i); 
		wait(& status );
	}
	}
	return 0;
   }

