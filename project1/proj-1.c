#include <stdio.h>
#include <stdlib.h>
#include "sem.h"
#define N 3 // Change the value here to increase number of threads and buffer size

semaphore_t mutex, reader_lock;
semaphore_t writer_lock[N];
int global_ID = 0;
int buffer[N];
int wc = 0;

void writer(void)
{ 
  int ID;
  P(&mutex); ID = global_ID++; V(&mutex);
  while(1){
	P(&writer_lock[ID]);
	buffer[ID]++;
	printf("[writer #%d]\t****Incrementing buffer %d : %d\n", ID,ID,buffer[ID]);
	//sleep(1);
	P(&mutex);
        wc++;
	if(wc == N){
	    V(&reader_lock);
	}
	V(&mutex);
  };
}

//-------------------------------------------------------

int main()
{
    init_sem(&mutex, 1);
    init_sem(&reader_lock,0);
    for(int x = 0; x < N; x++){
	buffer[x] = 0;
	init_sem(&writer_lock[x], 1);
	start_thread(writer, NULL);
    }
    
    while(1){
        P(&reader_lock);
        for(int x = 0; x < N;x++){
            printf("[Reader]\t****Reading Buffer %d : %d\n", x, buffer[x]);
        } 
        //sleep(1); 
        P(&mutex);
        wc = 0;
        V(&mutex);
        for(int x = 0; x < N;x++){
            V(&writer_lock[x]);
        }
  }

}   
