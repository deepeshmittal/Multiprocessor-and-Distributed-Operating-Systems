
#include<stdio.h>
#include<stdlib.h>
#include "threads.h"

int global=0;

void function1(){
	int local=0;

	while (1)
    	{
        	global++; local ++;
       		printf("\n\nFunction 1 : global = %d  local = %d\t\t\t", global, local);
       		sleep(1); 
		yield();
    	}	
}

void function2(){
	int local=0;

	while (1)
   	{
        	global++; local ++;
        	printf("\n\nFunction 2 : global = %d  local = %d\t\t\t", global, local);
        	sleep(1);
		yield();
    	}	
}

int main()
{
 ReadyQ = newQueue(ReadyQ);
 start_thread(function1);
 start_thread(function2);
 run();
}
 
