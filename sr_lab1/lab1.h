#ifndef LAB1_H
#define LAB1_H

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
