#include "BDServer.h"

BDServer :: BDServer(){
  _PortNo = DEFAULT_PORTNO;
}

BDServer :: BDServer(int portNo){
  if(portNo > 256 && portNo <= 65535){
    _PortNo = portNo;
  }
}

void BDServer :: startServer(){
  //TODO implement

  waitForConnection();
  
  _ServerSocket = socket(PF_INET, SOCK_STREAM, 0);
  _ServerAddress.sin_family = AF_INET;
  _ServerAddress.sin_port = _PortNo;
  _ServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(_ServerAddress.sin_zero, '\0', sizeof _ServerAddress.sin_zero);
  
  bind(_ServerSocket, (struct sockaddr *) &_ServerAddress, sizeof(_ServerAddress));

  waitForConnection();
}

void BDServer :: waitForConnection(){
  char buffer[1024];
  
  if (0 != _ServerSocket){
    if(listen(_ServerSocket, 5) == 0){
      printf("Listening\n");
    }

    /* accept call creates a new socket for incomming connection */
    _AddrSize = sizeof _ServerStorage;
    _ClientSocket = accept(_ServerSocket, (struct sockaddr *) &_ServerStorage, &_AddrSize);

    strcpy(buffer, "Hello World!\n");
    send(_ClientSocket, buffer, 13, 0);
  
  }else{
    printf("Initialize server socket first ya dunce");
  }
}

void BDServer :: setPort(int port){
  if(port > 256 && port <= 65535){
    _PortNo = port;
  }
}

int BDServer :: getPort(){
  return _PortNo;
}
