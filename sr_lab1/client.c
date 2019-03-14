#include "lab1.h"
#define SA struct sockaddr 

//program args
char* name;
int neig_port;
char* neig_ip;
int my_port;
int hasToken;;

int out_socket;

int in_socket;

int accepted_socket;

int logger_socket;

//this structure allows us to gather info about 
//socket of client who we accepted
struct sockaddr_in peer;
int len;

//initialize value of in_socket
//bind it with my port 
//start listeining on my port
void initialize(){
	//hold info about addres and prot
	struct sockaddr_in servaddr;

	// socket create and verification 
	in_socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (in_socket == -1) { 
		perror("Socket creation failed:"); 
		exit(-1); 
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
		perror("Socket bind failed:"); 
		exit(-1); 
	} 
	else
		printf("Socket successfully bound to %d..\n", servaddr.sin_port); 

	// Now server is ready to listen and verification 
	if ((listen(in_socket, 5)) != 0) { //second arg is number of possible connections in queue
		perror("Listen failed:"); 
		exit(-1); 
	} 
	else
		printf("Server listening..\n"); 

}

//initialize out_socket
//It's not done in initilize because we may need to change it 
void set_out_socket(){
	out_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(out_socket == -1){
		perror("Creating out socket failed\n");
		exit(-1);
	}
}

//basicly handle connect based on current value of neig_port
void request_session(){

	struct sockaddr_in sending_addr;
	sending_addr.sin_family = AF_INET;
	// in case of change(new client) 
	sending_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sending_addr.sin_port = htons(neig_port);

	//printf("%d %s \n", neig_port, neig_ip);

	if(connect(out_socket, (SA*)&sending_addr, sizeof(sending_addr)) == -1){
		perror("Connection failed:");
		exit(-1);
	}
	else{
		//printf("Connection succesfull\n");
	}
	

}

//handle accept on earlier initialized in_socket
void accept_session(){

	// Accept the data packet from client and verification 
	len = sizeof(peer); 

	accepted_socket = accept(in_socket, (SA*)&peer, &len); 
	if (accepted_socket < 0) { 
		perror("Server acccept failed:"); 
		exit(-1); 
	} 
	{
		//printf("server acccepted the client...\n");
	}
}

void send_msg(Token msg){
	//in case it was closed earlier or changed
	set_out_socket();

	if(msg.type == TOKEN){

		//we assume that message is just name passed to neighbour
		//so if we got token we change message and and pass it further
		msg.sender_port = my_port;
		msg.destination_port = neig_port;

		//zeroing out message - it helped log messages
		//because earlier logger recived a lot of trash with msg
		bzero(msg.msg, sizeof(msg.msg));
		strcpy(msg.msg, name);
		
		//logging part it's done on udp
		//logger_socket is initilized at begging of client
		//after parsing arguments
		struct sockaddr_in log_addr;
		log_addr.sin_family = AF_INET;
		log_addr.sin_addr.s_addr = inet_addr("224.1.2.3");
		log_addr.sin_port = htons(10000);

		if(sendto(logger_socket, msg.msg, sizeof(msg.msg), 0, (struct sockaddr *) &log_addr, sizeof(log_addr)) < 0){
			perror("Error sending log:");
			//not sure if that means i want to exit
		}
		else{
			//printf("log sent\n");
		}
	}
	request_session();
	write(out_socket, &msg, sizeof(msg));
	close(out_socket);
}

int main(int argc, char** argv) 
{ 
	if(argc != 6){
		printf("Not enough args\n");
		printf("Proper input: ./client [name] [my_port] [neighbour_ip] [neighbour_port] [hasToken]\n");
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
		perror("Coudln't create logger socket: ");
		exit(-1);
	}
	

	//let's assume that if u have token u are first client and if not then u are not
	if(hasToken==1){
		
		Token msg;
		
		initialize();
		set_out_socket();
		//then we wait for someone
		accept_session();
		
		read(accepted_socket, &msg, sizeof(msg)); //i will wait for init
		
		//if it's not init then someone tried to sneak a new token!
		//maybe figure out better way rather just closing 
		if(msg.type!=INIT){
			printf("Second token\n");
			exit(-1);
		}

		//if we got init start sending messages
		printf("Got init from %d\n", msg.sender_port);
		//change port based on info deliverd in init
		neig_port = msg.sender_port;
		
		//set message 
		msg.type = TOKEN;
		//send it
		send_msg(msg);
		
	}
	else{	
		//if we dont have Token we assume 
		//someone is already witing for us on our nieg_port
		initialize();
		set_out_socket();
		
		Token init_msg;
		init_msg.sender_port = my_port; //this is basicly info about our port
		init_msg.destination_port = neig_port; //that's info helps to find where init should be discarded
		init_msg.type = INIT;
		send_msg(init_msg);

	}
	//then we can go on
	while(1){
		//holds recived structure
		Token tk;

		accept_session();
		read(accepted_socket, &tk, sizeof(tk));
		
		if(tk.type==INIT){
			printf("Got init from Sender:%d Wants to be connected to:%d I am connecte to:%d\n", tk.sender_port, tk.destination_port, neig_port);
			//if i am connected to port some one wants to connect
			//i will have to connect to new guy
			if(tk.destination_port == neig_port){
				//i change my sending port to new guy's port
				printf("switched sending ports old:%d- new:%d\n", neig_port, tk.sender_port);
				neig_port = tk.sender_port;
			}
			else if(my_port != tk.sender_port){
				send_msg(tk);
			}
		}
		else if(tk.type==TOKEN){
			//if got token just rest for a second & pass it further 
			printf("Got msg: %s from %d\n", tk.msg, tk.sender_port);
			sleep(1);
			send_msg(tk);
		}

	}

	return 0;
} 

