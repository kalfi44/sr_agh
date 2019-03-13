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
#include "lab1.h"
#define SA struct sockaddr 

char* name;
int neig_port;
char* neig_ip;
int my_port;
int hasToken;;
int out_socket;
int in_socket;
int accepted_socket;
int logger_socket;
struct sockaddr_in peer;
int len;

void initialize(){
	//hold info about addres and prot
	struct sockaddr_in servaddr;

	// socket create and verification 
	in_socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (in_socket == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(my_port); 

	// Binding newly created socket to given IP and verification 
	if ((bind(in_socket, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded to %d..\n", servaddr.sin_port); 

	// Now server is ready to listen and verification 
	if ((listen(in_socket, 5)) != 0) { //second arg is number of possible connections in queue
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(peer); 

}

void set_out_socket(){
	out_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(out_socket == -1){
		perror("Out:");	
		printf("Creating out socket failed\n");
		exit(0);
	}
}

void request_session(){

	struct sockaddr_in sending_addr;
	sending_addr.sin_family = AF_INET;
	sending_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sending_addr.sin_port = htons(neig_port);

	//printf("%d %s \n", neig_port, neig_ip);

	if(connect(out_socket, (SA*)&sending_addr, sizeof(sending_addr)) == -1){
		perror("conn:");
		printf("Connection failed\n");
		exit(0);
	}
	else{
		//printf("Connection succesfull\n");
	}
	

}

void accept_session(){
	// Accept the data packet from client and verification 
	accepted_socket = accept(in_socket, (SA*)&peer, &len); 
	if (accepted_socket < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
	{
		//printf("server acccepted the client...\n");
	}
}

void send_msg(Token msg){
	//in case it was closed earlier
	set_out_socket();

	if(msg.type == TOKEN){
		msg.sender_port = my_port;
		msg.destination_port = neig_port;
		bzero(msg.msg, sizeof(msg.msg));
		strcpy(msg.msg, name);
		
		//logging part
		struct sockaddr_in log_addr;
		log_addr.sin_family = AF_INET;
		log_addr.sin_addr.s_addr = inet_addr("224.1.2.3");
		log_addr.sin_port = htons(10000);

		if(sendto(logger_socket, msg.msg, sizeof(msg.msg), 0, (struct sockaddr *) &log_addr, sizeof(log_addr)) < 0){
			printf("erro sending log\n");
		}
		else{
			//printf("log sent\n");
		}
	}
	//printf("I will try to open new con\n");
	request_session();
	//printf("probably error with out_socket\n");
	write(out_socket, &msg, sizeof(msg));
	close(out_socket);
}

/*
char* get_ip(){

	struct ifreq ifr;
	int fd = socket(AF_INET,SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	
	char *ip = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
	return ip;
}
*/

int main(int argc, char** argv) 
{ 
	if(argc != 6){
		printf("Not enough args\n");
		return 0;
	}

	name = argv[1];
	my_port = (int) strtol(argv[2], NULL, 10);
	neig_ip = argv[3];
	neig_port = (int) strtol(argv[4], NULL, 10);
	hasToken = (int) strtol(argv[5], NULL, 10);

	if(hasToken != 1 && hasToken !=0){
		printf("hasToken must be 0 or 1\n");
	}

	//init looger socket
	logger_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (logger_socket == -1){
		printf("Coudln't create logger socket\n");
		exit(0);
	}
	

	//let's assume that if u have token u are first client
	if(hasToken==1){
		
		Token msg;
		
		initialize();
		set_out_socket();
		
		accept_session();
		
		read(accepted_socket, &msg, sizeof(msg)); //i will wait for init
		
		printf("Got init from %d-listener\n", msg.sender_port);
		neig_port = msg.sender_port;
		msg.type = TOKEN;
		bzero(msg.msg, sizeof(msg.msg));
		strcpy(msg.msg, name);
		send_msg(msg);
		
	}
	else{
		initialize();
		set_out_socket();
		Token init_msg;
		init_msg.sender_port = my_port;
		init_msg.destination_port = neig_port;
		init_msg.type = INIT;
		send_msg(init_msg);

	}
	printf("into loop\n");
	while(1){
		Token tk;
		accept_session();
		read(accepted_socket, &tk, sizeof(tk));
		if(tk.type==INIT){
			printf("Got init from %d %d %d\n", tk.sender_port, tk.destination_port, neig_port);
			if(tk.destination_port == neig_port){
				neig_port = tk.sender_port;
				printf("switched ports my:%d- n:%d\n", my_port, neig_port);
				//send_msg(tk);// next hop will discard init but this will establish connection
			}
			else if(my_port != tk.sender_port){
				send_msg(tk);
			}
		}
		else if(tk.type==TOKEN){
			printf("Got msg: %s from %d\n", tk.msg, tk.sender_port);
			bzero(tk.msg, sizeof(tk.msg));
			strcpy(tk.msg, name);
			sleep(1);
			send_msg(tk);
		}

	}

	return 0;
} 

