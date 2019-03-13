#include <netdb.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#define MAX 80 
#define PORT 8080 
#define PORT2 8088
#define SA struct sockaddr 
int in_socket, out_socket;
int my_port;
// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
	char buff[MAX]; 
	int n; 
	// infinite loop for chat 
	for (;;) { 
		bzero(buff, MAX); 

		// read the message from client and copy it in buffer 
		read(sockfd, buff, sizeof(buff)); 
		// print buffer which contains the client contents 
		printf("From client: %s\t To client : ", buff); 
		bzero(buff, MAX); 
		n = 0; 
		// copy server message in the buffer 
		while ((buff[n++] = getchar()) != '\n') 
			; 

		// and send that buffer to client 
		write(sockfd, buff, sizeof(buff)); 

		// if msg contains "Exit" then server exit and chat ended. 
		if (strncmp("exit", buff, 4) == 0) { 
			printf("Server Exit...\n"); 
			break; 
		} 
	} 
} 

void initialize(){
	//hold info about addres and prot
	struct sockaddr_in servaddr;

	//create new socket for listening
	in_socket = socket(AF_INET, SOCK_STREAM, 0);

	//initialize listening 
	if(in_socket == -1){
		printf("Socket creation failed\n");
		exit(0);
	}else{
		printf("Socket created\n");
	}

	bzero(&servaddr, sizeof(servaddr));
	//assign port and addres
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(my_port);

	if((bind(in_socket, (SA*)&servaddr, sizeof(servaddr))) != 0){
		printf("binding failed\n");
		exit(0);
	}
	else{
		printf("bind succesfull\n");
	}

	if((listen(in_socket, 5)) !=0 ){
		printf("Listen failed\n");
		exit(0);
	}	
	else{
		printf("Listening..");
	}
		

}

// Driver function 
int main(int argc, char** argv) 
{ 

	if(argc!=3){
		printf("Not enough args\n");
		return 0;
	}

	int port_num = (int) strtol(argv[1],NULL,10);
	char* ip_adr = argv[2];
	my_port = port_num;
	
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(port_num); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	// Now server is ready to listen and verification 
	if ((listen(sockfd, 5)) != 0) { //second arg is number of possible connections in queue
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(cli); 

	// Accept the data packet from client and verification 
	connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		{
			printf("server acccept the client...\n");
			char str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &cli.sin_addr, str, INET_ADDRSTRLEN);
			printf("%s\n", str); 
		}
	// Function for chatting between client and server 
	func(connfd); 
	// After chatting close the socket 
	close(in_socket); 
} 

