#include <signal.h>
#include <stdio.h>

void cnt(int sig);

int main()
{
  signal(SIGINT, cnt); 
  printf("Begin counting and INTERRUPTs\n");
  for(;;);  
}

void cnt(int sig)
{
  static int count=0;
  printf("Total of %d INTERRUPTS received\n", ++count);
  if(count==2) signal(SIGINT, SIG_DFL);
}

