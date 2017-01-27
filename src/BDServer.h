/*	
	BDServer.h
	Authors: Tyrone Lagore (10151950) and James MacIsaac (10063078)
	Contact: tyrone.lagore@ucalgary.ca  james.macisaac@ucalgary.ca
	Tutorial: Tyrone-T01 James-T03
*/
#ifndef BD_SERVER_H
#define BD_SERVER_H

#define DEFAULT_PORTNO 2737
#define MAX_PATH 131072

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


/*
  BDServer
  
  The BDServer class initializes a server socket on a specified port and then waits for connections.
  When a client connects, the server allows the client to send a specific variety of commands, which
  the server will then execute on the users behalf on the running computer.

  If a port is not specified, or the port number is out of the range of available ports, the server
  will initialize on a default port number of 2737

 */
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
