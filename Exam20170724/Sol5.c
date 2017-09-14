#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <pthread.h>

FILE *fp;
int numbers[10];
int DONE = 0, begin = 0, variable, count = 0;
float media = 0;

void myfunc1();
void myfunc2();

sem_t mutex;
sem_t mutex1;

int main()
{
	pthread_t th1, th2;
	int ret1, ret2;
	
	if((sem_init(&mutex, 1, 1))<0)
	{
		perror("ERROR in creating semaphore");
		return -1;
	}
	
	if((sem_init(&mutex1, 1, 0))<0)
	{
		perror("ERROR in creating semaphore");
		return -1;
	}
	
	ret1 = pthread_create(&th1, NULL, (void *) &myfunc1, NULL);
	if(ret1<0)
	{
		perror("ERROR in creating thread1");
		return -1;
	}
	
	ret2 = pthread_create(&th2, NULL, (void*) &myfunc2, NULL);
	if(ret2<0)
	{
		perror("ERROR in creating thread2");
		return -1;
	}

	pthread_join(th1, NULL);
	pthread_join(th2, NULL);
	
	sem_destroy(&mutex);
	sem_destroy(&mutex1);
	
	return 0;
}

void myfunc1()
{
	srand(time(NULL));
	
	while(DONE == 0)
	{
		int i;
		sem_wait(&mutex);
		printf("operation number: %d\n", ++count);
		for(i=0; i<10; i++)
		{
			numbers[i] = rand()%41+10;
			printf("numbers[%d]: %d\n", i+1, numbers[i]);
		}
		
		for(i=0; i<10;i++)
		{
			media = media + numbers[i];
		}
		media = media/10.0;
		printf("MEDIA: %f\n", media);
		if(media>30)
		{
			printf("FINISH, media > 30\n");
			DONE = 1;
			sem_post(&mutex1);
		}
		else
		{
			if(begin==0)
			{
				fp = fopen("results.txt", "w+");
				begin = 1;
			}
			else
			{
				fp = fopen("results.txt", "a");
			}
		fprintf(fp, "MEDIA: %f\n", media);
		fclose(fp);
		media = 0;
		sem_post(&mutex1);
		}
	}
}

void myfunc2()
{	
	while(DONE == 0)
	{
		int i,j;
		sem_wait(&mutex1);
		if(DONE == 0)
		{
			for(j=0; j<10;s j++)
			{
				for(i=0; i<10; i++)
				{
					if(numbers[i]>numbers[i+1])
					{
						variable = numbers[i];
						numbers[i] = numbers[i+1];
						numbers[i+1] = variable;
					}
				}
			}
			printf("numbers after sort\n");
			fp = fopen("results.txt", "a");
			for(i=0; i<10; i++)
			{
				fprintf(fp, "NUMBER %d: %d\n", i, numbers[i]);
				printf("numbers[%d]: %d\n", i+1, numbers[i]);
			}
		
			fclose(fp);
			sem_post(&mutex);
		}
	}
}

