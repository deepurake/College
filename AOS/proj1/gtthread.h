#include <stdio.h>
#include <ucontext.h>
#include <signal.h>

#include <string.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h> 
#include <unistd.h>

#include <assert.h>

#define logging 0

#define xlogging 0

typedef enum {EMPTY,RUNNABLE,WAITING, FINISHED, EXITING, CANCELLED} tstate;
typedef enum {JOINABLE,JOINED} tjoin;

typedef struct gtstate
{
  tstate state;
  tjoin join_state;
} gtstate;

typedef struct gtthread
{
  ucontext_t ctx;
  struct gtthread* dep;
  gtstate agg_state;
  void** return_q;
  int choosing;
  int ticket;
  struct gtthread* next;
  struct gtthread* prev;
} *gtthread_t;


/* GT thread APIS */
void gtthread_init(long period);
int  gtthread_create(gtthread_t *thread,
                     void *(*start_routine)(void *),
                     void *arg);
int  gtthread_join(gtthread_t thread, void **status);
void gtthread_exit(void *retval);
int  gtthread_yield(void);
int  gtthread_equal(gtthread_t t1, gtthread_t t2);
int  gtthread_cancel(gtthread_t thread);
gtthread_t gtthread_self(void);


/* Mutex protjection*/
typedef int bool;

#define TRUE 1
#define FALSE 0

bool intLock;
typedef struct gtthread_mutex
{
	bool lock;
}gtthread_mutex_t;

int  gtthread_mutex_init(gtthread_mutex_t * mutex);
int  gtthread_mutex_lock(gtthread_mutex_t * mutex);
int  gtthread_mutex_unlock(gtthread_mutex_t * mutex);

