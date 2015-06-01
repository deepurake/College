#include "gtthread.h"

int  gtthread_mutex_init(gtthread_mutex_t * mutex)
{
	gtthread_mutex_t* lock_point = (gtthread_mutex_t *)malloc(sizeof(gtthread_mutex_t));
	memset(lock_point,0,sizeof(gtthread_mutex_t));
	mutex->lock = FALSE;
}
int  gtthread_mutex_lock(gtthread_mutex_t * mutex)
{
	bool* lock = &(mutex->lock);
	while(1)
	{
		//assert(intLock == FALSE);
		intLock = TRUE;
		if(*lock == FALSE)
		{
			*lock = TRUE;
			intLock = FALSE;
			return 0;
		}
		else
		{
			intLock = FALSE;
			scheduler(-1);
		}
	}
	return 0;
}
int  gtthread_mutex_unlock(gtthread_mutex_t * mutex)
{
	bool* lock = &(mutex->lock);
	assert(*lock == TRUE);
	*lock = FALSE;
	return 0;
}
