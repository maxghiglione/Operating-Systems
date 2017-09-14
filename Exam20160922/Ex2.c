#include <stdlib.h>
#include <unistd.h> 	// process
#include <stdio.h> 	// printf, open, read

void main()
{
int pid1 = 0;
int pid2 = 0;

pid1 = fork();  /* call #1 */
	
pid2 = fork();  /* call #2 */ 

if (pid1 == 0)
pid2 = fork();  /* call #3 */ 
//printf("pid1: %d - pid2: %d - i: %d - p: %d\n", pid1, pid2, getpid(), getppid());

if (pid1 != pid2)  fork();  /* call #4 */




printf("pid1: %d - pid2: %d - i: %d - p: %d\n", pid1, pid2, getpid(), getppid());
}

