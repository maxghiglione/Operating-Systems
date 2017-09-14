#include <semaphore.h> 	// semaphore
#include <unistd.h> 	// process
#include <stdio.h> 	// printf, open, read
#include <sys/shm.h>    // shmem
#include <sys/types.h> 	// sleep
#include <stdlib.h>     // exit


#define SSIZE sizeof(sem_t)
#define SPERM 0666

int main()
{

/*initizalize  shmem*/
key_t key;
key = IPC_PRIVATE;
int id, err, retval, pid;
id = shmget (key, SSIZE, SPERM);
if ( id ==  -1 ) printf ("shmget error\n");
else printf("Allocated  %d\n", id);

pid = fork();

/*error*/
if (pid < 0) {
	printf("fork error\n");
}
/*child*/
else if (pid == 0) {
	printf("child pid: %d- i: %d\n", pid, getpid());
	/*attach shmem*/
	sem_t *sp;
	sp = (sem_t  *) shmat (id, NULL, 0);
	if (*(int*) sp ==  -1) {
		printf("shmat error\n");
		exit (2) ;
		}
	/*wait*/
	printf("%d: wait \n", getpid());
	sem_wait(sp);
	printf("%d: wake \n", getpid());

	/*detach shmem*/
	err = shmdt ((void*) sp);
	if(err  ==  -1) printf("shmdt error\n");
}

/*parent*/
else {
	sleep(5);
	printf("parent pid: %d- i: %d\n", pid, getpid());
	/*attach shmem*/
	sem_t *sp;
	sp = (sem_t  *) shmat (id, NULL, 0);
	if (*(int*) sp ==  -1) {
		printf("shmat error\n");
		exit (2) ;
		}
	/*initialize semaphore*/
	retval = sem_init (sp ,1,0);
	if (retval != 0) {
		printf("sem_init error\n");
		exit (3);
	}
	else printf("Semaphore init %d\n", retval);

	/*post*/
	sleep(5);
	printf("%d: post \n", getpid());
	sem_post(sp);
	sleep(5);
	printf("%d: wake \n", getpid());		

	/*destroy semaphore*/
	sem_destroy (sp);

	/*remove shmem*/
	err = shmctl (id , IPC_RMID , 0);
	if ( err  ==  -1 ) printf("shmctl error\n");
	else printf("Removed %d\n",err);
}

exit(0);
}
