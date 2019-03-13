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
#define MAX 80 
#define PORT 8081 
#define SA struct sockaddr 

char* name;
int neig_port;
char* neig_ip;
int my_port;
int hasToken;;
int out_socket;
int in_socket;
int accepted_socket;
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
	sending_addr.sin_addr.s_addr = inet_addr(neig_ip);
	sending_addr.sin_port = htons(neig_port);

	//printf("%d %s \n", neig_port, neig_ip);

	if(connect(out_socket, (SA*)&sending_addr, sizeof(sending_addr)) == -1){
		perror("conn:");
		printf("Connection failed\n");
		exit(0);
	}
	else{
		printf("Connection succesfull");
	}
	

}

void accept_session(){
	printf("dupa1\n");
	// Accept the data packet from client and verification 
	accepted_socket = accept(in_socket, (SA*)&peer, &len); 
	if (accepted_socket < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
	{
		printf("server acccepted the client...\n");
	}
}

void send_msg(Token msg){
	//in case it was closed earlier

	msg.sender_port = my_port;
	msg.destination_port = neig_port;

	if(msg.type == TOKEN){;
	msg.msg = "Message from";
	strcat(msg.msg, name);
	strcat(msg.msg, "\n");
	}
	
	request_session();
	write(out_socket, &msg, sizeof(msg));
	close(out_socket);
}

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

//void send_initial_msg(){
//	
//}

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

	//let's assume that if u have token u are first client
	if(hasToken==1){
		
		Token msg;
		
		initialize();
		//set_out_socket();
		printf("po init\n");
		accept_session(); //i will onect with me here
		
		//wait for someone to speak
		read(accepted_socket, &msg, sizeof(msg));
		//check if it's INIT - should be!
		//if(msg.type != INIT){
		//	printf("Two tokens in ring! Aborting token\n");
		//	exit(0); //to do - find solution other than drop client
		//}
		//printf("Got init from %d-listener", msg.sender_port);
		neig_port = msg.sender_port;
		neig_ip = msg.sender_ip;
		msg.type = TOKEN;
		send_msg(msg);
	
	}
	else{
		initialize();
		set_out_socket();
		request_session();

		Token init_msg;
		init_msg.type = INIT;
		init_msg.sender_ip = get_ip();
		init_msg.sender_port = my_port;
		send_msg(init_msg);

	}

	while(1){


	}

	return 0;
} 

