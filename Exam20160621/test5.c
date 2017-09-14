#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#define  READ   0   /* The index of the “read” end of the pipe */
#define  WRITE  1    /* The index of the “write” end of the pipe */

struct msg {
	int value;
	};

void main() {
	struct msg data_write;
	struct msg data_read;
	data_write.value = 10;
	data_read.value = 0;
	int fd[2], bytesRead;
	  /* Parent process’ message buffer */
	pipe(fd);  /* Create  an unnamed pipe */
	if ( fork() == 0 )  /* Child, write */ {
		scanf("%d", &data_write.value);
		getchar();
		write(fd[WRITE], &data_write, 1); /* Send */
		close(fd[WRITE]); /* Close used end */
	} 
	else      /* Parent, reader */ {
		bytesRead = read( fd[READ], &data_read,  1);
		/* Receive */
		printf("Read %d bytes: %d \n", data_read.value, bytesRead );
		close(fd[READ]);  /* Close used end */
	}
}
