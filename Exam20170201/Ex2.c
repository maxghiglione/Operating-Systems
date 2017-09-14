//CodeA:
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int x;
void setX();

int main() 
{
	int status, pid=fork();
	x = 5;
	if(!pid){
		printf("ppid = %d pid = %d\n", getppid(), getpid());
		setX();
		}
	else 	{
		printf("ppid = %d pid = %d\n", getppid(), getpid());
		wait(&status);
		printf("ppid = %d pid = %d\n", getppid(), getpid());
		printf("x = %d\n",x);
		}
	}

void setX() {
	x = 10;
}

