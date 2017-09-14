#include <stdio.h> 	// printf
#include <stdlib.h> 	// exit
#include <semaphore.h>  // semaphore
#include <sys/types.h>	// sleep
#include <sys/ipc.h> 	// ipc
#include <sys/shm.h>    // shmem
#include <unistd.h> 	// process
#include <signal.h> 	// signal
#include <fcntl.h> 	// file

#define SEGMENTPERM 0666
#define N 3

int main(int argc, char* argv[])
{



int err;
int retval;
int i;
int flag = 0;
int fd;
int data[N];
int pid;
off_t pos = 0;

/* Make shared memory segment. */
int id1;
sem_t * sp1;
id1 = shmget (IPC_PRIVATE, sizeof(sem_t), SEGMENTPERM);
if (id1 == -1) perror("1 shmget error\n");

sp1 = (sem_t *) shmat(id1,NULL, 0);
if (*(int*) sp1 ==  -1) perror("1 shmat error\n");

/* Initialize the semaphore. */
retval = sem_init(sp1,1,1);
if (retval != 0) {
	perror("Couldn’t initialize.\n");
	exit(3);
}

/* Make shared memory segment. */
int id2;
sem_t * sp2;
id2 = shmget (IPC_PRIVATE, sizeof(sem_t), SEGMENTPERM);
if (id2 == -1) perror("2 shmget error\n");

sp2 = (sem_t *) shmat(id2,NULL, 0);
if (*(int*) sp2 ==  -1) printf("2 shmat error\n");

/* Initialize the semaphore. */
retval = sem_init(sp2,1,0);
if (retval != 0) {
	perror("Couldn’t initialize.");
	exit(3);
}

/* child process*/
pid = fork();
if (pid == 0) {
	
	/* clear file */
	fd = open("sensors_2.txt", O_WRONLY|O_CREAT,S_IRWXU);
	if(fd < 0) perror("error opening file");
	printf("Created %d\n", fd);
	close(fd);
	
	/* loop */
	while(flag == 0){	
		printf("writer\n");
		sem_wait(sp1);
		fd = open("sensors_2.txt",O_WRONLY);
		if(fd < 0) perror("error opening file");
		pos = lseek(fd, 0, SEEK_END);
		for(int i = 0; i < N; i++) {
			scanf("%d", &data[i]);
			retval = write(fd, &data[i], sizeof(int));
			pos = lseek(fd, 0, SEEK_CUR);		
			if(retval != sizeof(int))perror("retval \n" );
			printf("write %d, i: %d, pos: %ld, rtval: %d\n", data[i], i, pos, retval);	
		}
		close(fd);
		sem_post(sp2);
	}
	exit(0);
}

/* back to parent process */
int new_avg = 0;
int old_avg = 0;
int tmp = 0;

while(flag == 0) {
	i = 0;

	sem_wait(sp2);
	printf("monitor\n");
	fd = open("sensors_2.txt", O_RDONLY);
	if(fd < 0) perror("error opening file");
	pos = lseek(fd, pos, SEEK_CUR); //bring back to last read
	/* read file */
	while( (retval = read(fd, &tmp, sizeof(int)) ) == sizeof(int)) {
		pos = lseek(fd, 0, SEEK_CUR);
		printf("read %d, i: %d, pos: %ld, rtval: %d\n", tmp, i, pos, retval);
		data[i] = tmp;
		i++;
		if(i == N){		
			i = 0;
		}
	}

	if (retval != 0) {
		perror("not EOF");
	}
		//printf("test read %d, i: %d, pos: %ld, rtval: %d\n", tmp, i, pos, retval);	

	for(int i = 0; i < N; i++) {		
		//printf("sum %d\n", data[i]);
		new_avg += data[i]; 			
	}
	new_avg /= N;

	if(new_avg > 2*old_avg && old_avg != 0) {
		printf("avg %d error\n", new_avg);
 		kill(pid, SIGINT); //maybe something more elegant
		flag = 1;
        }
	else {
		printf("avg %d ok\n", new_avg);
		old_avg = new_avg;
		new_avg = 0;	
	}
	close(fd);
	sem_post(sp1);	
}

printf("closing\n");
sem_destroy(sp1);
sem_destroy(sp2);

/* Remove segment. */
err = shmctl(id1, IPC_RMID, 0);
if (err == -1) perror("Removal.");
err = shmctl(id2, IPC_RMID, 0);
if (err == -1) perror("Removal.");

return 0;
}
