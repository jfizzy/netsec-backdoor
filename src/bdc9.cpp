#include <string>
#include <iostream>
using namespace std;

#include "BDServer.h"

int main(int argc, char *argv[])
{
  BDServer server;
  int portNum;
  
  if (argc != 2){
    cout << "Invalid number of arguments, please specify a port to run on" << endl;
    cout << "Proper usage: " << endl << argv[0] << " portNo" << endl;
  }else{
    try{
      portNum = stoi(argv[1]);
      server.setPort(portNum);
      server.startServer();
    }catch(const std::exception &e){
      cout << "Error in " << e.what();
    }
  }
  //maintains socket read/write information
  /*char ioBuffer[1024];
  //holds socket addresses
  struct sockaddr_in serverAddress, clientAddress;
  int socketFd,
    acceptSockFd,
    portNo,
    ioRead;

  
  */
}

void error(const char *errorMessage){
  perror(errorMessage);
  exit(1);
}
