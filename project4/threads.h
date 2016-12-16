#include<stdio.h>
#include<ucontext.h>
#include "q.h"

TCB_t *Curr_Thread;
Queue *ReadyQ;
int counter = 0;

int getThreadID(TCB_t *item){
    return item->thread_id;
}

/* initializes context for the passes function thread and adds to ready queue */
void start_thread(void (*function)(void)){
    char *stack = (char *) malloc(8192*sizeof(char));
    TCB_t *tcb = (TCB_t *) malloc(sizeof(TCB_t));
    init_TCB(tcb, function, (void *) stack, 8192);
    tcb->thread_id = ++counter;
    //printf("\n\nAssigning thread ID : %d \n",tcb->thread_id);
    AddQueue(ReadyQ,tcb);
}

/* Start the first thread */
void run() {
    Curr_Thread = DelQueue(ReadyQ);
    ucontext_t parent;
    getcontext(&parent);
    swapcontext(&parent, &(Curr_Thread->context));

}

/* Current thread swaps context with that of last element in queue */
void yield(){
    TCB_t *Prev_Thread;
    AddQueue(ReadyQ, Curr_Thread);
    Prev_Thread = Curr_Thread;
    Curr_Thread = DelQueue(ReadyQ);	
    if (Curr_Thread == NULL){	//Empty queue exception
	return;
    }
    //printf("Sleeping Thread %d : Waking Thread %d\n", getThreadID(Prev_Thread), getThreadID(Curr_Thread));
    swapcontext(&(Prev_Thread->context), &(Curr_Thread->context));    
}
