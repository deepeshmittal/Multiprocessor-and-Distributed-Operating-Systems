#include <stdlib.h>
#include <unistd.h>
#include "TCB.h"

typedef struct Queue				//Point to the 1st ele of the queue
{
	TCB_t *header;
}Queue;

Queue* newQueue(Queue*);	// creates an empty queue, pointed to by the variable head
void AddQueue(Queue*, TCB_t*);	// adds a queue item, pointed to by "item", to the queue pointed to by head
TCB_t* DelQueue(Queue*);	// deletes an item from head and returns a pointer to the deleted item
TCB_t* newItem();		// Allocate space for new queue element	

Queue* newQueue(Queue *Q)				//Initialize an empty queue
{
	Q = (Queue *)malloc(sizeof(Queue));
	Q->header = (TCB_t *)malloc(sizeof(TCB_t));	//dummy element
	Q->header->thread_id = -1;
	Q->header->next=Q->header;
	Q->header->prev=Q->header;
	return Q;
}

int isEmpty(Queue *Q) {
   if(Q->header->next == Q->header)
      return 1;
   return 0;
}

void AddQueue(Queue *Q,  TCB_t *item)
{
	if(isEmpty(Q)){
		Q->header->next = item;
		Q->header->prev= item;
        item->prev= Q->header;
        item->next= Q->header;	
	}else{
		item->next= Q->header;
        item->prev=Q->header->prev;
		Q->header->prev->next=item;
        Q->header->prev=item;
	}
}

TCB_t* DelQueue(Queue *Q)		//have to return the pointer to the item and delete from queue
{
	TCB_t *item;

	if(isEmpty(Q)){			//error if the queue is empty, cannot delete
		return NULL;	
	}
	else{	//if the queue is nt empty
		item = Q->header->next;
		Q->header->next=Q->header->next->next;
		Q->header->next->prev=Q->header;
	}
	item->next = NULL;
	item->prev = NULL;
	return item;
}


TCB_t* newItem()
{
	TCB_t *item= (TCB_t *) malloc(sizeof(TCB_t));
	return item;
}
