/* bd_server.h  */

#ifndef BD_SERVER_H
#define BD_SERVER_H

#define DEFAULT_PORTNO 2737

class BDServer{
public:
  BDServer();
  BDServer(int);
  void startServer();
  int getPort();
  void setPort(int);
  
private:
  int _SocketFd;
  int _AcceptSockFd;
  int _PortNo;
};

#endif
