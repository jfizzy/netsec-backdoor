/* bd_server.h  */

#ifndef BD_SERVER_H
#define BD_SERVER_H

#define DEFAULT_PORTNO 2737

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
using namespace std;

class BDServer{
public:
  BDServer();
  BDServer(int);
  void startServer();
  int getPort();
  void setPort(int);
  
private:
  int _ServerSocket;
  int _ClientSocket;
  int _PortNo;

  struct sockaddr_in _ServerAddress;
  struct sockaddr_storage _ServerStorage;
  socklen_t _AddrSize;
  

  void waitForConnection();
};

#endif
