#include <stdio.h> // printf
#include <unistd.h> // posix (processes)

void main()
{ 
int i, pid;
for(i = 0; i<2; i++)
	pid = fork();

//printf("return: %d pid: %d, parent: %d\n", pid, getpid(), getppid());
//if(pid) printf("Hello World, pid: %d\n", getpid());
if(pid) printf("Hello World\n");
}
