/*	
	main for backdoor program
	Authors: Tyrone Lagore (10151950) and James MacIsaac (10063078)
	Contact: tyrone.lagore@ucalgary.ca  james.macisaac@ucalgary.ca
	Tutorial: Tyrone-T01 James-T03
*/

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
}

void error(const char *errorMessage){
  perror(errorMessage);
  exit(1);
}
