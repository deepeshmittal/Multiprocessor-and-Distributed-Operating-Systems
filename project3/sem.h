#include "threads.h"

typedef struct Semaphore
{
	int counter;
	Queue *queue;
}Semaphore_t;

void CreateSem(Semaphore_t *sem, int InputValue)
{
	sem->queue = newQueue(sem->queue);
	sem->counter=InputValue;
}

void P(Semaphore_t *sem)
{
	TCB_t *previous_thread;
	sem->counter--;
	if(sem->counter<0) 
	{	
		AddQueue(sem->queue,Curr_Thread);
		previous_thread = Curr_Thread; 
		Curr_Thread = DelQueue(ReadyQ);
		swapcontext(&(previous_thread->context), &(Curr_Thread->context));
	}
}

void V(Semaphore_t *sem)
{
	TCB_t *temp;
	sem->counter++;
	if(sem->counter<=0)
	{
		temp=DelQueue(sem->queue);
		AddQueue(ReadyQ,temp);
	}
	yield();
}
