#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
using namespace std;

int main(int argc, char *argv[]){
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  size_t bufferSize = 256;
  size_t amountRead;

  char *buffer;


  buffer = (char *)malloc(bufferSize * sizeof(char) + 1 * sizeof(char));
  
  portno = atoi(argv[2]);


  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0 ){
    printf("FUCK\n");
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
  serv_addr.sin_port = htons(portno);
  if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
    fprintf(stderr, "ERROR connecting\n");
  }

  n=read(sockfd, buffer, 255);
  printf("%s\n", buffer);

  printf("Enter command: ");
  amountRead = getline(&buffer, &bufferSize, stdin);

  while (strncmp("end_session", buffer, (int)strlen(buffer) - 1) != 0){
      write(sockfd, buffer, amountRead);
      amountRead = getline(&buffer, &bufferSize, stdin);
  }

  write(sockfd, buffer, amountRead);

    
}
