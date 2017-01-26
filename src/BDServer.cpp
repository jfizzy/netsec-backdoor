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

  _ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
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
    if(listen(_ServerSocket, 1) == 0){
      printf("Listening\n");
    }

    /* accept call creates a new socket for incomming connection */
    _AddrSize = sizeof _ServerStorage;
    _ClientSocket = accept(_ServerSocket, (struct sockaddr *) &_ServerStorage, &_AddrSize);

    printf("Client connected. Handshaking.\n");

    strcpy(buffer, "Server: Connected.\n");
    write(_ClientSocket, buffer, strlen(buffer));
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
  char buffer[MAX_PATH];
  
  printf("Waiting for command...\n");
  amountRead = read(_ClientSocket, buffer, MAX_PATH);
  while(amountRead > 0){
    trimLeft(buffer, strlen(buffer));
    trimRight(buffer, strlen(buffer));
    if (amountRead == 0){
      try{
	memcpy(buffer, "Empty command\n", 14);
	write(_ClientSocket, buffer, 14);
      }catch(const std::exception& e){
	//client has disconnected
	break;
      }
    }else if(strncmp(buffer, "off", strlen(buffer)) == 0){
      memcpy(buffer, "Shutting down.\n", 15);
      write(_ClientSocket, buffer, 14);
      return;
    }else if(strncmp(buffer, "help", strlen(buffer)) == 0){
      helpMenu();
    }else{
      //executeCommand(buffer);
      parseCommand(buffer);
      printf("Waiting for command...\n");
    }
    memset(buffer, 0, MAX_PATH);
    amountRead = read(_ClientSocket, buffer, MAX_PATH);
  }

  printf("Client disconnected. Resetting client socket.\n");
  resetConnection();
  waitForConnection();
}

/*
  parseCommand

  parses commannds
*/

void BDServer :: parseCommand(char* command){
  /* todo 
     pwd
     cd <dir>
     - should be handled with system calls instead of popen
     
     ls
     cat <file>
     help
     off

     optional:
     who
     net
     ps
     nmap <params>
     ext
  */
  char* function;
  char* arguments;
  
  trimLeft(command, strlen(command));
  trimRight(command, strlen(command));

  //extract function - first characters up to space or tab
  function = strtok(command, " \t");
  arguments = strtok(NULL, " \t");
  
  //if next strtok read on NULL is NULL then there are no arguments (nothing following function)
  if(arguments == NULL){
    if(strcmp(function, "cd") == 0){
      //cd with no arguments, switch to home directory
      printf("cd no arguments\n");
    }else if(strcmp(function, "ls") == 0){
      //ls with no
      printf("ls no arguments\n");
    }else{
      printf("other command, no arguments\n");
    }
  }else{
    if(strcmp(function, "cd") == 0){
      printf("cd with arguments\n");
    }else{
      printf("other command with arguments\n");
    }
  }
}

/*
  executeCommand
    
  executeCommand takes in a command and executes it and redirects the output
  of the response so that the output can be sent to the client socket.

*/
void BDServer :: executeCommand(char* command)
{
  FILE *fp;
  char path[MAX_PATH];
  char *output = (char*)malloc(MAX_PATH * sizeof(char));
  char *placeholder = output;
  
  printf("%s\n", command);

  fp = popen(command, "r");
  if(fp != NULL){
    while(fgets(path, sizeof(path) -1, fp) != NULL){
      memcpy(output, path, strlen(path));
      output += strlen(path);
      //write(_ClientSocket, path, MAX_PATH);
      memset(path, 0, 256);
    }
  }else{
    output[0] = '-';
    output[1] = '1';
    output[2] = '\0';
  }

  output = placeholder;

  /* TODO make a parse command function to parse the command, tell user if bad command there - not in execute command. */
  if(strlen(output) == 0){
    memcpy(output, command, strlen(command));
    output += strlen(command) - 1;
    memcpy(output, ": is not a valid command.\n", 26);
    output = placeholder;
  }

  write(_ClientSocket, output, MAX_PATH);
}

void BDServer :: helpMenu(){
  printf("Help!\n");
}

void BDServer :: changeDirectory(char *directory){
  int ret = chdir(directory);
}


void BDServer :: resetConnection(){
  close(_ClientSocket);

  _ClientSocket = 0;
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

void BDServer :: trimLeft(char *input, int length){
  int index = 0, i = 0;

  while(index < length && (input[index] == ' ' || input[index] == '\n' || input[index] == '\t')){
    index ++;
  }

  while(i + index < length){
    input[i] = input[i + index];
    i++;
  }

  input[i] = '\0';
}

void BDServer :: trimRight(char *input, int length){
  int index = length - 1;
  
  while(index > 0 && (input[index] == ' ' || input[index] == '\n' || input[index] == '\t')){
    input[index] = '\0';
    index --; 
  }
}

bool BDServer :: contains(char *input, char ch){
  int i = 0;
  bool found = false;
  while(i < strlen(input) && !found){
    if(input[i] == ch)
      found = true;
  }

  return found;
}
