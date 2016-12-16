#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "sem.h"

struct message{
    int arr_message[10];
};

struct port{
    struct message *message_queue;
    int startptr;
    int endptr;
    Semaphore_t full, empty;
    Semaphore_t mutex;   
};

struct port port_array[100];

void init_port(int index){
    port_array[index].startptr = 0;
    port_array[index].endptr = 0;
    port_array[index].message_queue = (struct message *) malloc (10 * sizeof(struct message));
    CreateSem(&(port_array[index].mutex), 1);
    CreateSem(&(port_array[index].full), 10);
    CreateSem(&(port_array[index].empty), 0); 
}


void send(int portnumber, const struct message *sending_msg){
    P(&(port_array[portnumber].full));
    P(&(port_array[portnumber].mutex)); 
    port_array[portnumber].message_queue[port_array[portnumber].endptr] = *sending_msg;
    port_array[portnumber].endptr = (port_array[portnumber].endptr + 1)%10;  
    V(&(port_array[portnumber].mutex));
    V(&(port_array[portnumber].empty));
}

struct message* receive(int portnumber){
    struct message *recv_msg;
    P(&port_array[portnumber].empty);
    P(&port_array[portnumber].mutex);
    recv_msg =&(port_array[portnumber].message_queue[port_array[portnumber].startptr]);
    port_array[portnumber].startptr = (port_array[portnumber].startptr + 1)%10;
    V(&port_array[portnumber].mutex);
    V(&port_array[portnumber].full);
    return recv_msg;
}


