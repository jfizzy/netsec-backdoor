#include <stdio.h>
#include <sys/socket.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
using namespace std;

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
}

void BDServer :: setPort(int port){
  if(port > 256 && port <= 65535){
    _PortNo = port;
  }
}

int BDServer :: getPort(){
  return _PortNo;
}
