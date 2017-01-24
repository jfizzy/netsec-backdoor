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

	buffer = (char *)malloc(bufferSize * sizeof(char));

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

	printf("Enter command: ");
	amountRead = getline(&buffer, &bufferSize, stdin);
	
	while (strncmp("end_session", buffer, (int)strlen(buffer)-1) != 0){
		write(sockfd, buffer, 1023);
		//n = read(sockfd, buffer, 1023);
		printf("%s\n", buffer);
		amountRead = getline(&buffer, &bufferSize, stdin);
	}
	
}
