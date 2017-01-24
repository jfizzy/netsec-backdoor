#include "BDClient.h"


int main(int argc, char* argv[])
{
	for(int i = 0; i < argc; i++) 
		cout << "argv[" << i << "] = " << argv[i] << endl; 

	cout << "Welcome to the cool BDClient...\n";
	int client_sock;
	char buffer[1024];
	struct sockaddr_in server_addr;
	socklen_t addr_size;

	printf("Configuration: ip = [%s], port = [%s]\n", argv[0], argv[1]);

	//create the socket
	//args: inet domain, stream sock, default protocol (tcp here)
	client_sock = socket(PF_INET, SOCK_STREAM, 0);

	//config server address struct settings
	server_addr.sin_family = AF_INET;
	//port num. using htons func to use proper byte order 	
	server_addr.sin_port = htons(stoi(argv[2]));
	//IP of localhost
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	//set all padding bits to 0
	memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

	//conn the sock to the server using configed addr struct
	addr_size = sizeof server_addr;
	connect(client_sock, (struct sockaddr *) &server_addr, addr_size);
	
	//read msg from server into buffer
	recv(client_sock, buffer, 1024, 0);

	//print the recv msg
	printf("Response from server: %s", buffer);
	return  0;
}
