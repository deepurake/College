#include "gtthread.h"
#include <stdio.h>
#include <ucontext.h>
#include <signal.h>

#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h> 
#include <unistd.h>

#include <assert.h>

#define max_threads 10
#define STACK_SIZE SIGSTKSZ

/* Sched_period */
int sched_period = 30000;



/*========================================================================================================== 
										Following threads are special threads used int thread API
===========================================================================================================*/
/* current thread pointer */
gtthread_t curr_thread;
/* last thread where new threads to be appended */
gtthread_t last_thread;
/* Junk thread is the graveyard of threads */
gtthread_t junk;
/* Cleanup thread which runs after main thread exit and from where process gets cleaned up. */
gtthread_t cleanup;


/*========================================================================================================== 
										Following functions are functions related to scheduler
===========================================================================================================*/
/* Local function declarations */

/* Scheduler which is periodically called by timer schedules tasks */
void scheduler(int signum);
/* Timer which is called initially which setups periodic interrupts to scheduler */
void set_timer();


/*========================================================================================================== 
							Following functions are helper function to Initialize GTthread structures
===========================================================================================================*/
/* Initialize threads */
void init_threads();
void init_list();

/*========================================================================================================== 
Following three functions are helper function to control the state of any thread spawned by GTthread library
===========================================================================================================*/
/* A wrapper around the threads used to control their behaviour such as cleanup after exiting etc.. */
void thread_wrap(void *arg,void *(*start_routine)(void *));

/* This runs before thread init as a part of wrapper */
void thread_init();

/* This cleans up thread database after thread returns and is joined.*/
void thread_clean();

 /* RK_DBG add comments */
gtthread_t gtthread_create_empty();
void cleanup_routine();




/*========================================================================================================== 
		Func: Gtthread_init
		return: void
===========================================================================================================*/
void gtthread_init(long period)
{
	/* Lock the scheduler interrupt */
	intLock = TRUE;
	
	init_threads();

	init_list();

	sched_period = period;

	set_timer();

#if logging
	printf("RK_DBG: Initialized threads curr: %d\n",curr_thread);
#endif

	/* Unlock the scheduler interrupt */
	intLock = FALSE;
}

void init_list()
{
	/* Start chaining for list of threads
			We use bidirectional circular lined list which assists us greatly in handling threads.		
		*/
  curr_thread->next = curr_thread;
  curr_thread->prev = curr_thread;

  last_thread = curr_thread;

}
void init_threads()
{
#if logging
	printf("RK_DBG: Initializing threads\n");
#endif


	/* Create special threads 
			1) Curr_thread where pointer to current thread is stored.
			2) junk where destroyed threads are dumped
			3) Cleanup thread where process cleanup happens. Main thread will point to this thread after it exists
	*/
	/* Currently executing thread is indicated by curr_thread */
	curr_thread = (gtthread_t)malloc(sizeof(struct gtthread));
  memset(curr_thread,0,sizeof(struct gtthread));
	getcontext(&(curr_thread->ctx));
	curr_thread->agg_state.state = RUNNABLE;

	/* Junk is the graveyard of threads. We can cleanup in cleanup thread */
	junk = (gtthread_t)malloc(sizeof(struct gtthread));
  memset(junk,0,sizeof(struct gtthread));
	getcontext(&(junk->ctx));
	junk->agg_state.state = EMPTY;

	/* Main Thread is linked to this thread. After main thread is done, we will keep running this thread which checks if all threads are doen executing. If so we can exit the process. */
	cleanup = (gtthread_t)malloc(sizeof(struct gtthread));
  memset(cleanup,0,sizeof(struct gtthread));
	getcontext(&(cleanup->ctx));
	cleanup->agg_state.state = RUNNABLE;

	cleanup->ctx.uc_stack.ss_sp = malloc(STACK_SIZE);
	cleanup->ctx.uc_stack.ss_size = STACK_SIZE;
	cleanup->ctx.uc_stack.ss_flags = 0;
	makecontext(&(cleanup->ctx), cleanup_routine, 0);

}

/*========================================================================================================== 
		Func: gtthread_create
		return: int - status
===========================================================================================================*/
int  gtthread_create(gtthread_t *thread,
                     void *(*start_routine)(void *),
                     void *arg)
{
	/* Lock scheduler interrupt */
	intLock = TRUE;
#if logging
	printf("RK_DBG: Creating threads\n");
#endif
	*thread = gtthread_create_empty();

	(*thread)->agg_state.state = RUNNABLE;

	/* Allocate a thread */
  getcontext(&((*thread)->ctx));
	(*thread)->ctx.uc_stack.ss_sp = malloc(STACK_SIZE);
	(*thread)->ctx.uc_stack.ss_size = STACK_SIZE;
	(*thread)->ctx.uc_stack.ss_flags = 0;
  (*thread)->ctx.uc_link = &(curr_thread->ctx);
	makecontext(&((*thread)->ctx), thread_wrap, 2, arg, start_routine);
#if logging
	printf("RK_DBG: Created thread %d \n",*thread);
	fflush(stdout);
#endif
  (*thread)->agg_state.join_state = JOINABLE;

	/*UNLOCK scheduler interrupt */
	intLock = FALSE;

	return 0;
	
}

/*========================================================================================================== 
		Func: gtthread_create_empty
		return: gtthread_t - thread descriptor
===========================================================================================================*/
gtthread_t gtthread_create_empty()
{
  gtthread_t new_thread = (gtthread_t)malloc(sizeof(struct gtthread));
  memset(new_thread,0,sizeof(struct gtthread));
  new_thread->next = last_thread->next;
  new_thread->prev = last_thread;
  
  last_thread->next = new_thread;
  last_thread = new_thread;
  return new_thread; 
}

/*========================================================================================================== 
		Func: gtthread_join
		return: int - status
===========================================================================================================*/
int  gtthread_join(gtthread_t thread, void **status)
{
	/* Lock scheduler interrupt*/
	intLock = TRUE;

	/* curr_thread--> WAITING, thread --> add dependency */
	curr_thread->return_q = status;
	thread->dep = curr_thread;
  thread->agg_state.join_state = JOINED;
	
	curr_thread->agg_state.state = WAITING;
	if(thread->agg_state.state == CANCELLED)
	{
		thread_clean(((void *)(size_t) -1),thread);
	}
	/* UnLock scheduler interrupt */
#if logging
	printf("RK_DBG: joining %d and curr %d \n", thread,curr_thread);
	fflush(stdout);
#endif
	intLock = FALSE;
	scheduler(-1);
	return 0;
}

/*========================================================================================================== 
		Func: gtthread_exit
		return: int - status
===========================================================================================================*/
void gtthread_exit(void *retval)
{	
	curr_thread->agg_state.state = FINISHED;

	scheduler(-1);
#if logging
	printf("RK_DBG: cleaning from exit %d \n",curr_thread);
	fflush(stdout);
#endif
	thread_clean(retval,curr_thread);	

	/* Invoke scheduler */
	scheduler(-1);
}

/*========================================================================================================== 
		Func: gtthread_join
		return: int - status
===========================================================================================================*/
int  gtthread_yield(void)
{
	scheduler(-1);
	return 0;
}

/*========================================================================================================== 
		Func: gtthread_equal
		return: non 0 if threads or equal. 0 otherwise.
===========================================================================================================*/
int  gtthread_equal(gtthread_t t1, gtthread_t t2)
{
	return (t1==t2);
}

/*========================================================================================================== 
		Func: gtthread_cancel
		return: int - status
===========================================================================================================*/
int  gtthread_cancel(gtthread_t thread)
{
	assert((thread->agg_state.state!=EMPTY)&&(thread->agg_state.state!=EXITING));
	if(thread->agg_state.state == FINISHED)
		return 0;
	thread->agg_state.state = CANCELLED;

#if logging
	printf("RK_DBG: cleaning from cancel %d \n",thread);
	fflush(stdout);
#endif
	if(thread == curr_thread)
	{
		thread->agg_state.state = FINISHED;
		scheduler(-1);
		thread_clean(((void *)(size_t) -1),thread);
		scheduler(-1);
	}
	return 0;
} 

/*========================================================================================================== 
		Func: gtthread_self
		return: return current thread descriptor
===========================================================================================================*/
gtthread_t gtthread_self()
{  
	return curr_thread;
}

/*========================================================================================================== 
		Func: scheduler a local function runs periodically to reschedule threads
		return: void
===========================================================================================================*/
void scheduler(int signum)
{
	int return_val = 0;
	gtthread_t next_thread = curr_thread;
	gtthread_t temp = curr_thread;
	gtthread_t temp1 = curr_thread;
	int loop = 0;
	if(intLock && (signum != -1))
	{
		fflush(stdout);
		intLock = 0;
		return;
	}
	if(signum == -1)
	{
		intLock = 1;
	}
	
	if(curr_thread->agg_state.state == EXITING)
	{
#if logging
		printf("RK_DBG: freeing %d %d %d\n",curr_thread,curr_thread->next,curr_thread->next->next);
#endif
		curr_thread->agg_state.state = EMPTY;
    
    /* link prev and next */
    curr_thread->prev->next = curr_thread->next;
    curr_thread->next->prev = curr_thread->prev;
    
    /*free current thread*/
		if(curr_thread->next == curr_thread)
		{
			exit(0);
		}
		if(curr_thread == last_thread)
		{
			last_thread = curr_thread->prev;
		}
		temp = junk;
		next_thread = curr_thread->next;
#if logging
		printf("RK_DBG: freeing %d %d %d\n",curr_thread,curr_thread->next,curr_thread->next->next);
		printf("next_thread %d %d %d\n",next_thread,next_thread->next->next,next_thread->prev);
#endif
    free(curr_thread);
    curr_thread = temp;
		temp1 = next_thread;
		
	}
	while (1) {
#if xlogging
		if(next_thread->next == NULL)
			printf("sched next_thread %d %d\n",next_thread,temp1);
#endif
		next_thread = next_thread->next;
    assert(next_thread != NULL);
		if((next_thread->agg_state.state == RUNNABLE)||
				((next_thread->agg_state.join_state == JOINED) && (next_thread->agg_state.state == FINISHED))/*||
				((next_thread->agg_state.join_state == JOINED) && (next_thread->agg_state.state == CANCELLED))*/)
		{
#if xlogging
			printf("RK_DBG: found runnable thread %d %d %d %d\n", next_thread, next_thread->agg_state.state,next_thread->agg_state.join_state, signum);
			fflush(stdout);
#endif
			break;
		}
		if(next_thread == temp1)
		{
			loop++;
			if(loop == 2)
			{
#if logging
				printf("RK_DBG: No runnable thread found, exiting\n");
#endif
				exit(0);
			}
		}
	}
	if(next_thread == curr_thread)
	{
#if logging
		printf("RK_DBG: running thread unchanged \n");
		fflush(stdout);
#endif
		/*Only one thread-- do nothing*/
		return;
	}
	curr_thread = next_thread;
#if xlogging
	printf("RK_DBG:temp %d sig %d, exiting %d \n",temp, signum, next_thread);
#endif
	return_val = swapcontext(&(temp->ctx), &(next_thread->ctx));
	if(return_val != 0)
	{
		printf("RK_DBG: Critical Error %d\n",return_val);
	}
}

/*========================================================================================================== 
		Func: set_timer sets up periodic alarm for scheduler
		return: void
===========================================================================================================*/
void set_timer()
{
    struct itimerval tv;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = scheduler;
    if (sigaction(SIGVTALRM, &sa, NULL) == -1) {
        printf("error with: sigaction\n");
        exit(0);
    }

    tv.it_value.tv_sec = 0;
    tv.it_value.tv_usec = sched_period;
    tv.it_interval.tv_sec = 0;
    tv.it_interval.tv_usec = sched_period;
    setitimer(ITIMER_VIRTUAL, &tv, NULL);
}

/*========================================================================================================== 
Following three functions are helper function to control the state of any thread spawned by GTthread library
===========================================================================================================*/
/* A wrapper around the threads used to control their behaviour such as cleanup after exiting etc.. */
void thread_wrap(void *arg,void *(*start_routine)(void *))
{
	void* return_val = start_routine(arg);
	/* initialise */
	thread_init();
	/* execute */
  
	curr_thread->agg_state.state = FINISHED;
#if logging
	printf("RK_DBG: thread %d returned %lu \n",curr_thread,(long *)return_val);
#endif
	{
		scheduler(-1);
	}	
	/* clean	*/
	thread_clean(return_val,curr_thread);
	
	/* Invoke scheduler */
	scheduler(-1);
}

void thread_init()
{
	/* RK_DBG nothing for now */
#if logging
	printf("RK_DBG: calling thread init\n");
#endif
	return;
}

void thread_clean(void* return_val,gtthread_t thread_id)
{
#if logging
	printf("RK_DBG: cleaning thread %d \n",thread_id);
#endif

	/* Lock scheduler interrupt */
	intLock = TRUE;

	if(thread_id->dep && ((thread_id->dep)->agg_state.state ==  WAITING))
	{
#if logging
		printf("RK_DBG: putting return val %d\n",return_val);
#endif
		if((thread_id->dep)->return_q != NULL)
		{
#if xlogging
			printf("RK_DBG: putting return val %d %d\n",return_val, (thread_id->dep)->return_q);
#endif
			*((thread_id->dep)->return_q) = return_val;
		}
#if xlogging
		printf("cleaning %d joining %d state %d \n",thread_id,thread_id->dep,(thread_id->dep)->agg_state.state);
#endif
		(thread_id->dep)->agg_state.state = RUNNABLE;
		
	}
	thread_id->dep = NULL;

	thread_id->agg_state.state = EXITING;

	/* Lock scheduler interrupt */
	intLock = FALSE;

	return;
}


void cleanup_routine()
{
	printf("cleanup routine called\n");
}
