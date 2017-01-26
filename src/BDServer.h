/* bd_server.h  */

#ifndef BD_SERVER_H
#define BD_SERVER_H

#define DEFAULT_PORTNO 2737
#define MAX_PATH 32768

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <errno.h>
#include <sstream>

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
  void waitForCommand();
  void parseCommand(char* command);
  void executeCommand(char* command);
  void helpMenu();
  void changeDirectory(char* command);
  void resetConnection();
  void badFunction(char *command);
  void printWorkingDirectory();
  void printDirectoryStats();
  void terminalLine();

  void trimLeft(char *input, int length);
  void trimRight(char *input, int length);
  bool contains(char *input, char ch);
};



#endif
