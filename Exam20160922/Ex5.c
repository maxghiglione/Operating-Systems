#include <stdio.h> 	// printf
#include <stdlib.h> 	// exit
#include <semaphore.h>  // semaphore
#include <sys/types.h>	// sleep
#include <sys/ipc.h> 	// ipc
#include <sys/shm.h>    // shmem
#include <unistd.h> 	// process
#include <signal.h> 	// signal

#define SEGMENTPERM 0666
#define N 3

int main(int argc, char* argv[])
{



int err;
int retval;
int i;
int flag = 0;
FILE* fptr;
int data[N];
int pid;

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
	fptr = fopen("sensors.txt","w");
	fclose(fptr);
	
	/* loop */
	while(flag == 0){	
		printf("writer\n");
		sem_wait(sp1);
		fptr = fopen("sensors.txt","a");
		if(!fptr) printf("error opening file\n");
		for(int i = 0; i < N; i++) {
			scanf("%d", &data[i]);
			fprintf(fptr, "%d\n", data[i]);			
		}
		fclose(fptr);
		sem_post(sp2);
	}
	exit(0);
}

/* back to parent process */
int new_avg = 0;
int old_avg = 0;
int tmp = 0;
//off_t pos = 0;

while(flag == 0) {
	i = 0;

	sem_wait(sp2);
	printf("monitor\n");
	fptr = fopen("sensors.txt","r");
	if(!fptr) printf("error opening file\n");
	
	/* read file */
	while(fscanf(fptr, "%d", &tmp) == 1) {
		//pos = lseek(fptr, pos, SEEK_SET);
		printf("read %d, i: %d, pos: %d\n", tmp, i, pos);
		data[i] = tmp;
		i++;
		if(i == N){		
			i = 0;
		}
	}
	if (!feof(fptr)) {
		printf("error reading\n");	
	}

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
	fclose(fptr);
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
