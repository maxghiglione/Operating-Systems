#include <stdio.h> 	// printf, open, read
#include <unistd.h>

int main(int argc, char ** argv) 
{
int i;

for (i=0; i < 3; i++) 
{
execlp ("echo", "echo", "hello", NULL);
printf("I’m out of exec \n");
}
}

