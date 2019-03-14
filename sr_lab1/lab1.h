#ifndef LAB1_H
#define LAB1_H

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <arpa/inet.h>

typedef enum {
	TOKEN,
	INIT
}msg_type;

typedef struct{
	msg_type type;
	int sender_port;
	int destination_port;
	char msg[256]; 
} Token;

#endif
