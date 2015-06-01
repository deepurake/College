#include <gtthread.h>
#include <time.h>
#include <stdlib.h>

#define numPhil 10
#define iterations 100
#define unused -1
/* The precompiler will replace this piece of code wherever waittime is called. 
Hence it is random evertime it is called */
#define waitTime rand()%500

int philId[numPhil];
int sticks[numPhil];

gtthread_mutex_t mutex;

void waitFor (unsigned int msecs,int myID) {
		unsigned int start = clock();
    unsigned int retTime = start + msecs;     /* Get finishing time. */
    while (clock() < retTime){
			if(clock()<start) break;
	    gtthread_yield();/* Loop until it arrives. */
		}
}

void philosopher(void *ID)
{
	int myID = (int)ID;
	int i = 0;
	int left = myID;
	int right = (myID+1)%numPhil;
	int first = left;
	int second = right;
	int r = 0;

	printf("Philosospher %d has left %d right %d id %d\n",myID+1,left,right, (int)gtthread_self());

	if(myID == 0)
	{
		printf("First philosopher, picks right stick first \n");
		first = right;
		second = left;
	}
	for (i = 0; i< iterations; i++)
	{
		printf("Philosopher %d is hungry %d'th time\n",myID+1, i);
		printf("Philosopher %d is picking first chopstick %d\n", myID+1, first);
		while(1)
		{
			gtthread_mutex_lock(&mutex);
			if(sticks[first] == -1)
			{
				sticks[first] = myID+1;
				gtthread_mutex_unlock(&mutex);
				break;
			}
			gtthread_mutex_unlock(&mutex);
		}
		printf("Philosopher %d picked first stick and is picking second chopstick %d\n", myID+1,second);
		while(1)
		{
			gtthread_mutex_lock(&mutex);
			if(sticks[second] == -1)
			{
				sticks[second] = myID+1;
				gtthread_mutex_unlock(&mutex);
				break;
			}
			gtthread_mutex_unlock(&mutex);
		}
		
		printf("Philosopher %d is eating\n", myID+1);
		waitFor(waitTime,myID);
		printf("Philosopher %d is dropping his chopsticks\n",myID+1);
		gtthread_mutex_lock(&mutex);
		sticks[first] = -1;
		sticks[second] = -1;
		gtthread_mutex_unlock(&mutex);
		printf("Philosopher %d is thinking\n",myID+1);
		waitFor(waitTime,myID);
	}
	printf("+++++++++++++++++++++++++++++++++\n Philosopher %d is exiting the room\n+++++++++++++++++++++++++++++++++\n",myID+1);
}

int main()
{
	gtthread_t threadId[numPhil];
	int i = 0;
	
	gtthread_init(500);
	gtthread_mutex_init(&mutex);
	for ( i = 0; i<numPhil; i++)
	{
		philId[i] = i;
		sticks[i] = unused;
		printf("i:%d\n",i);
	}
	for( i = 0; i<numPhil; i++)
	{
		gtthread_create((gtthread_t *)&threadId[i], philosopher, (void *)i);
	}

	for (i = 0; i<numPhil; i++)
	{
		gtthread_join(threadId[i],NULL);
	}


	return 0;
}
