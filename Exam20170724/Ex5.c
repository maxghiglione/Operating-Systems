/* this solution assumes:
- semaphore
- reading from file
- overwriting (ordering only 10)
- global variable avg as flag (not using signals)
*/


#include <stdio.h> 	// printf, open, read
#include <pthread.h> 	// pthread
#include <stdlib.h>     // srand, rand, qsort
#include <time.h>       // time
#include <unistd.h> 	// process
#include <sys/types.h> 	// wait, sleep, exit
#include <semaphore.h> 	// semaphore

#define RRANGE 40
#define RMIN 10
#define RAVG 30
#define N 10

void* random_handler();
void* sort_handler();

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

FILE *fptr;

sem_t s2;
sem_t s1;
int avg = 0;

int main()
{
	srand(time(NULL));
	
	sem_init(&s1, 0, 1);
	sem_init(&s2, 0, 0);

	pthread_t random_thread;
	pthread_t sort_thread;
	
	printf("creating threads\n");
	pthread_create(&random_thread, NULL, random_handler, NULL);
	pthread_create(&sort_thread, NULL, sort_handler, NULL);

	/* waits for random_handler to exit */
	pthread_join(random_thread, NULL);
	printf("closed random_thread\n");
	
	/* waits for exit of sort_thread */
	//global variable used, but another option is a signal or a condition variable
	pthread_join(sort_thread, NULL);
	printf("closed sort_thread\n");
	
	sem_destroy(&s1);
	sem_destroy(&s2);
	return 0;
}

void* random_handler(){
	int value[N];
	int sum = 0;
	int flag = 0;
	int i = 0;
	while(!flag){
		/* not critical */
		for(i = 0; i < N; i++){
			value[i] = (rand()%RRANGE) + RMIN;
			sum += value[i];
		}
		
		printf("random waits\n");
		/* critical */	
		sem_wait(&s1);
		fptr = fopen("file.txt","w");
		if (fptr == NULL){
			printf("Error! opening file\n");
			exit(1);
			}
		
		printf("write: ");
		for(i = 0; i < N; i++){
			fprintf(fptr,"%d\n", value[i]);
			printf("%d, ", value[i]);
		}
		printf("\n");
		avg = sum/N;
		fclose(fptr);
		sem_post(&s2);
		
		/* not critical */
		printf("avg: %d\n", avg);
		if(avg > 30){
			flag = 1;
		}		
		sum = 0;
	}
	
	pthread_exit(NULL);
}

void* sort_handler(){
	int value[N];
	int flag = 0;
	int i = 0;
	while(!flag){
		/* critical */
		printf("sort waits\n");
		sem_wait(&s2);
		fptr = fopen("file.txt","r+");
		
		printf("read: ");
		for(i = 0; i < N; i++){
			fscanf(fptr, "%d", &value[i]);
			printf("%d, ", value[i]);
		}
		printf("\n");
		qsort (value, N, sizeof(int), compare);
		
		
		printf("sort: ");
		for(i = 0; i < N; i++){
			fprintf(fptr,"%d\n", value[i]);
			printf("%d, ", value[i]);
		}		
		printf("\n");		
		printf("max: %d\n", value[N-1]);
		fclose(fptr);
		sem_post(&s1);
	
		/* not critical */
		if(avg > 30){
			flag = 1;
		}
	}
	pthread_exit(NULL);
}
