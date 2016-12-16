
#include<stdlib.h>
#include<stdio.h>
#include "msgs.h"
#include<time.h>

int global_ID=0;
Semaphore_t sem_global;
char** files;
int No_Of_Clients;

#define FILE_EXT ".server"

int SERVER_PORT = 0;


void Sever_Thread(void){
  struct message *recv_message;
  struct message *send_message;

  char client_files[No_Of_Clients][256];
  int current_client_count = 0;

  while(1){
	  //sleep(1);
  	char filename[22];
    strcpy(filename,"");
    
  	printf("Sever listening messages on server port %d \n",SERVER_PORT);
  	recv_message = receive(SERVER_PORT);
    int reply_port = recv_message->arr_message[0];
    send_message = (struct message*) malloc(sizeof(struct message));
  	if (recv_message->arr_message[1] == 1){
      if(current_client_count == 3){
        send_message->arr_message[0] = 3;
      }else{
        current_client_count++;
        send_message->arr_message[0] = 2;
      }
      printf("Sever sending reply on client port %d \n",reply_port);
      send(reply_port,send_message);
    }else if(recv_message->arr_message[1] == 4 || recv_message->arr_message[1] == 11){
      int terminate = 1;
      memset(filename,0,sizeof(filename));
      strcpy(filename,client_files[reply_port]);
      int file_len = strlen(filename);
      for(int i = 2; i < 10;i++){
        if(recv_message->arr_message[i] != -1){
          if(file_len >= 15){
            send_message->arr_message[0] = 6;
            terminate = 0;
            current_client_count--;
            break;    
          }else{
            filename[file_len++] = recv_message->arr_message[i];
          }
        }
      }
      if (terminate){
          send_message->arr_message[0] = 5;
          if(recv_message->arr_message[1] == 11){
            strcat(filename,FILE_EXT);
          }

      }
      strcpy(client_files[reply_port],filename);
      printf("Sever sending reply on client port %d \n",reply_port);
      send(reply_port,send_message);
    }else if (recv_message->arr_message[1] == 7 || recv_message->arr_message[1] == 8){
      strcat(filename,client_files[reply_port]);
      int terminate = 1;
      FILE * file;
      file = fopen(filename, "a");
      for(int i = 2; i < 10;i++){
        if(recv_message->arr_message[i] != -1){
          int results = fputc(recv_message->arr_message[i], file);  
        }
        fseek(file, 0, SEEK_END); // seek to end of file
        unsigned long size = ftell(file); // get current file pointer
        if(size > 1048576){
          send_message->arr_message[0] = 10;
          fclose(file);
          int err = remove(filename);
          current_client_count--;
          terminate = 0;
          break;
        }
      }
      if(terminate){
          send_message->arr_message[0] = 9;
          fclose(file);
          if(recv_message->arr_message[1] == 8){
            current_client_count--;
          }
      }
      printf("Sever sending reply on client port %d \n",reply_port);
      send(reply_port,send_message);
    }else{
      printf("Unknown request type - Discarding message \n");
    }
  }
}

void Client_Thread(void){
  
    int ID;
    int client_port;
    P(&sem_global); 
    ID = global_ID++;
    client_port = ID;
    char *filename = files[ID];
    printf("Client : %d | File : %s - Port assigned %d \n",ID,filename,client_port);
    V(&sem_global);
    struct message *recv_message;
    struct message *send_message;

    /* Establishing server connection - Sending file name */
    send_message = (struct message*) malloc(sizeof(struct message));
    send_message->arr_message[0] = client_port;
    send_message->arr_message[1] = 1;

    printf("Client : %d | File : %s - Trying to establish connection with server\n",ID,filename);
    while(1){
      send(SERVER_PORT,send_message);
      recv_message = receive(client_port);   
      if (recv_message->arr_message[0] == 2){
        printf("Client : %d | File : %s - Server accepted transfer request\n",ID,filename);
        break;
      }else if (recv_message->arr_message[0] == 3){
        printf("Client : %d | File : %s - Server rejected transfer request because it is serving 3 clients already, Will retry later\n",ID,filename);
        //sleep(2);
        printf("Client : %d | File : %s - Retrying to establish connection with server\n",ID,filename);
        continue;
      }else{
        printf("Client : %d | File : %s - Unknown response from server for establishing connection request \n",ID,filename);
        printf("Terminating client %d\n", ID);
        thread_exit();
      }
    }

    /* Server connection successfull - Sending file name */

    send_message->arr_message[1] = 4;
    int msg_pointer = 2;
    
    for(int i = 0;i<strlen(filename);i++){
      send_message->arr_message[msg_pointer++] = filename[i];
      if(msg_pointer == 10){
        printf("Client : %d | File : %s - Client sending file content \n",ID,filename);
        send(SERVER_PORT,send_message);
        recv_message = receive(client_port);
        if (recv_message->arr_message[0] == 5){
          printf("Client : %d | File : %s - Server added file name \n",ID,filename);
          msg_pointer = 2;
        }else if (recv_message->arr_message[0] == 6){
          printf("Client : %d | File : %s - File name exceeded 15 characters limit, aborting file transfer\n",ID,filename);
          printf("Terminating client %d\n", ID);
          thread_exit();
        }else{
          printf("Client : %d | File : %s - Unknown response from server for file transfer request \n",ID,filename);
          printf("Terminating client %d\n", ID);
          thread_exit();
        }
      }
    }  
    while(msg_pointer != 10){
      send_message->arr_message[msg_pointer++] = -1;
    }
    send_message->arr_message[1] = 11;
    send(SERVER_PORT,send_message);
    recv_message = receive(client_port);
    if (recv_message->arr_message[0] == 5){
    }else if (recv_message->arr_message[0] == 6){
      printf("Client : %d | File : %s - File name exceeded 15 characters limit, aborting file transfer\n",ID,filename);
      printf("Terminating client %d\n", ID);
      thread_exit();
    }else{
      printf("Client : %d | File : %s - Unknown response from server for file transfer request \n",ID,filename);
      printf("Terminating client %d\n", ID);
      thread_exit();
    }
    
    /* Server connection successfull - Starting file transfer */

    printf("Client : %d | File : %s - Starting file transfer to server\n",ID,filename);
    send_message->arr_message[0] = client_port;
    send_message->arr_message[1] = 7;
    msg_pointer = 2;
    FILE* file = fopen(filename, "r"); 
    int file_char;
    while ((file_char = fgetc(file)) != EOF) {
      if(msg_pointer == 10){
        send(SERVER_PORT,send_message);
        recv_message = receive(client_port);
        if (recv_message->arr_message[0] == 9){
          printf("Client : %d | File : %s - Server added file content \n",ID,filename);
          msg_pointer = 2;
        }else if (recv_message->arr_message[0] == 10){
          printf("Client : %d | File : %s - File size exceeded 1MB, aborting file transfer\n",ID,filename);
          printf("Terminating client %d\n", ID);
          thread_exit();
        }else{
          printf("Client : %d | File : %s - Unknown response from server for file transfer request \n",ID,filename);
          printf("Terminating client %d\n", ID);
          thread_exit();
        }
      }
      
      send_message->arr_message[msg_pointer++] = file_char;
    }
    
    while(msg_pointer != 10){
      send_message->arr_message[msg_pointer++] = -1;
    }
    send_message->arr_message[1] = 8;
    send(SERVER_PORT,send_message);
    recv_message = receive(client_port);
    if (recv_message->arr_message[0] == 9){
    }else if (recv_message->arr_message[0] == 10){
      printf("Client : %d | File : %s - File size exceeded 1MB, aborting file transfer\n",ID,filename);
      printf("Terminating client %d\n", ID);
      thread_exit();
    }else{
      printf("Client : %d | File : %s - Unknown response from server for file transfer request \n",ID,filename);
      printf("Terminating client %d\n", ID);
      thread_exit();
    }

    fclose(file);
    printf("Client : %d | File : %s - Transfer complete\n",ID,filename);
    printf("Terminating client %d\n", ID);
    thread_exit();
}

int main(int argc, char* argv[])
{
	srand(time(0));
  ReadyQ = newQueue(ReadyQ);
  CreateSem(&sem_global,1);
	No_Of_Clients = atoi(argv[1]);
	
	for(int i = 0; i < No_Of_Clients; i++){
    init_port(i); 
	}
	SERVER_PORT = No_Of_Clients;
	init_port(SERVER_PORT);
	printf("Number of files passed %d \n", No_Of_Clients);
  start_thread(Sever_Thread);
  printf("Welcome Sever Thread (Server Thread created)\n");

	files = malloc((No_Of_Clients) * sizeof *files);
	
	for(int i = 0; i < No_Of_Clients; i++){
    if(argv[i+2]){
  		files[i] = malloc(strlen(argv[i+2])+1);
  		strcpy(files[i], argv[i+2]);
    }
	}
	
	for(int i = 0; i < No_Of_Clients; i++){
    FILE * file;
    file = fopen(argv[i+2], "r");
    if(file) {
		  printf("Creating client for file %s\n", argv[i+2]);
		  start_thread(Client_Thread);
    } else {
		  if(argv[i+2]){
		    printf("File %s does not exists \n", argv[i+2]);
        printf("No Client Created ...\n");
		  }
    }
	}
  run();
  while(1)
  {
    sleep(1);
  }

}


