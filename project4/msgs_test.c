
#include<stdlib.h>
#include<stdio.h>
#include "msgs.h"

int global_ID=0;
Semaphore_t sem_global;

#define SERVER_PORT 99

void Sever_Thread(void){
  int ID;
  P(&sem_global); ID = global_ID++; V(&sem_global);
  struct message *recv_message;
  struct message *send_message;

  int global_counter = 0;

  while(1){
	sleep(1);
  	printf("Sever listening messages on server port %d \n",SERVER_PORT);
  	recv_message = receive(SERVER_PORT);
  	int reply_port = recv_message->arr_message[0];
  	printf("Sever processing client request for client port %d \n",reply_port);
  	P(&sem_global); global_counter++; V(&sem_global);
  	send_message = (struct message*) malloc(sizeof(struct message));
  	send_message->arr_message[0] = SERVER_PORT;
  	send_message->arr_message[1] = global_counter;
  	for(int i = 2; i<10 ; i++){
  		send_message->arr_message[i] = 0;
  	}
  	printf("Sever sending reply on client port %d \n",reply_port);
  	send(reply_port,send_message);
  }


}

void Client_Thread(void){
  
  int ID;
  P(&sem_global); ID = global_ID++; V(&sem_global);
  int client_port;
  struct message *recv_message;
  struct message *send_message;

   while(1){
   	sleep(1);
   	client_port = rand() % 98;

  	send_message = (struct message*) malloc(sizeof(struct message));
  	send_message->arr_message[0] = client_port;
  	for(int i = 1; i<10 ; i++){
  		send_message->arr_message[i] = 0;
  	}
  	printf("\t\t\tClient %d sending message on server port %d and waiting for response on port %d \n",ID,SERVER_PORT,client_port);
  	send(SERVER_PORT,send_message);
  	recv_message = receive(client_port);
  	int server_port = recv_message->arr_message[0];
  	printf("\t\t\tClient %d received server response on port %d from  server port %d\n",ID,client_port,server_port);
  	printf("\t\t\tPrinting received global counter value from server %d \n",recv_message->arr_message[1]);
  }
}

int main()
{
        ReadyQ = newQueue(ReadyQ);
        CreateSem(&sem_global,1);
	for(int i = 0; i < 100; i++){
            init_port(i); 
	}
        start_thread(Sever_Thread);
        printf("Welcome Sever Thread (Server Thread created)\n");
        start_thread(Client_Thread);
        printf("Welcome Client (Client 1 created)\n");
        start_thread(Client_Thread);
        printf("Welcome Client (Client 2 created)\n");
        start_thread(Client_Thread);
        printf("Welcome Client (Client 3 created)\n");        
        run();
        while(1)
        {
                sleep(1);
        }

}


