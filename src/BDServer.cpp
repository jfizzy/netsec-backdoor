#include "BDServer.h"

/*
  Default constructor, initiates a default port number
 */
BDServer :: BDServer(){
  _PortNo = DEFAULT_PORTNO;

  //initialize server socket and client socket to known state
  _ServerSocket = 0;
  _ClientSocket = 0;
}

/*
  Alternative constructor, given a port number it will set the specified
  port number as long as the port number falls in a valid range.

  Otherwise, default port is specified.
 */
BDServer :: BDServer(int portNo){
  if(portNo > 256 && portNo <= 65535){
    _PortNo = portNo;
  }else{
    _PortNo = DEFAULT_PORTNO;
  }

  //initialize server socket and client socket to known state
  _ServerSocket = 0;
  _ClientSocket = 0;

}

/*
  startServer
  
  initializes the server socket then waits for a connection
 */
void BDServer :: startServer(){

  _ServerSocket = socket(PF_INET, SOCK_STREAM, 0);
  _ServerAddress.sin_family = AF_INET;
  _ServerAddress.sin_port = htons(_PortNo);
  _ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(_ServerAddress.sin_zero, '\0', sizeof _ServerAddress.sin_zero);
  
  bind(_ServerSocket, (struct sockaddr *) &_ServerAddress, sizeof(_ServerAddress));

  waitForConnection();
}

void BDServer :: waitForConnection(){
  char buffer[1024];
  
  if (_ServerSocket > 0){
    if(listen(_ServerSocket, 5) == 0){
      printf("Listening\n");
    }

    /* accept call creates a new socket for incomming connection */
    _AddrSize = sizeof _ServerStorage;
    _ClientSocket = accept(_ServerSocket, (struct sockaddr *) &_ServerStorage, &_AddrSize);

    //ideally tcp handshake will happen here
    strcpy(buffer, "Server: Connected.\n");
    write(_ClientSocket, buffer, 40);
    //once a conncetion has been made, wait for a command
    waitForCommand();
  }else{
    printf("Initialize server socket first ya dunce");
  }
}

/*
  waitForCommand

  waitForCommand assumes that a TCP connection has been made to a client.
  It then waits for a command to be sent from the client to parse
 */
void BDServer :: waitForCommand()
{
  int amountRead;
  char buffer[256];
  amountRead = read(_ClientSocket, buffer, 255);
  while(strcmp(buffer, "end_session") != 0){
    executeCommand(buffer);
    amountRead = read(_ClientSocket, buffer, 255);
  }
}

/*
  executeCommand

  executeCommand takes in a command and executes it and redirects the output
  of the response so that the output can be sent to the client socket.

 */
char* BDServer :: executeCommand(char* command)
{
  printf("%s\n", command);
}

/* Getters/Setters */
void BDServer :: setPort(int port){
  if(port > 256 && port <= 65535){
    _PortNo = port;
  }
}

int BDServer :: getPort(){
  return _PortNo;
}
