/* 
* Name: Jordan Murtiff
* Date: 4/19/2020
* Title: Lab #7
* Description: To develop a link state routing algorithim for a given network of routers. Allow the user to update the
* network of routers at any given point and still be able to find the shortest path from any two given routers. 
*/

//Libraries used
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>

//Define the value that two nodes will be am infinite distance from each other (so they are not neighbors of each other).
#define INF 10000

//Define the struture of a Machine containing a name, ip address, and port number.
typedef struct 
{
  char name[50];
  char ip[50];
  int port;
} Machine;

//Cost matrix, machines array, thread mutex lock, machineId, and numMachines are all global variables.
int cost_matrix[4][4];
Machine machines[4];
int machineId;
int numMachines;
pthread_mutex_t lock;

//Function definitions
void* receiveInfo(void * arg);
void* linkState(void * arg);
void setNumMachines(char * number);
void setMachineId(char * id);
void initializeCostTable(char * name);
void initializeMachineTable(char * name);
void printTable();
void changeCost();
void updateCost(int * edge);

/* Main function, setting up the number of machines, their ID numbers, as well as the cost table of
the costs between different machines. Next, the function has two threads running two functions, one that 
is the receiveInfo function that keeps track if any cost changes are made from any other machine, and the other function 
being linkState that runs Dijkstra's algorithim to find the lowest cost path from a given machine to any 
other machine. Finally, the changeCost function is continually run by the main thread that updates the cost table of 
other machines so that all machines know of any new cost values between their neighbors. */
int main (int argc, char ** argv) 
{
  //Make sure right amount of arguments are used.
  if(argc != 5) 
  {
    fprintf(stderr,"Usage: %s machineID 2totalMachines costFile machineFile\n",argv[0]);
    exit(EXIT_FAILURE);  
  }

  //Setup for randomization.
  srand(time(NULL));

  //Set up machineId, numMachines, and machines array.
  setNumMachines(argv[2]);
  setMachineId(argv[1]);
  initializeMachineTable(argv[4]);

  //Set up cost table array.
  initializeCostTable(argv[3]);

  //Set up the additional two threads necessary to run the receiveInfo and linkState functions.
  pthread_t thr1, thr2;

  //Run Receive Info thread
  pthread_create(&thr1,NULL,receiveInfo,NULL);

  //Run Link State thread
  pthread_create(&thr2,NULL,linkState,NULL);

  //Start locking mechanism
  pthread_mutex_init(&lock,NULL);

  //Function that allows user to change cost to any given edge in the network of routers.
  changeCost();
}

/* This function sets the number of machines for a given network of routers (the machines the being the routers).
Usually this number is equivalent to the number of lines in the "machines" file that contains each machine's name, 
ip address, and port number. If the number of machines cannot be read or is not a valid number, the function will
input a corresponding erro message and end the program. */
void setNumMachines(char * number) 
{
  //Get numMachines from argument safely
  char * end = NULL;
  int errno;
  long longNumMachines = strtol(number,&end,10);

  //If the number is NULL, then it can't be read and used.
  if(end == number) 
  {
    fprintf(stderr,"Couldn't read number of machines\n");
    exit(EXIT_FAILURE);
  }

  //If the machine number is not within the range of possible long integers, return an error message.
  if(errno == ERANGE && (longNumMachines == LONG_MAX || longNumMachines == LONG_MIN)) 
  {
    fprintf(stderr,"Failed to get number of machines\n");
    exit(EXIT_FAILURE);
  }
  //Initialize variable
  numMachines = (int) longNumMachines;
}

/* This function sets the number of machines for a given network of routers (the machines the being the routers).
Usually this number is equivalent to the number of lines in the "machines" file that contains each machine's name, 
ip address, and port number. If the number of machines cannot be read or is not a valid number, the function will
input a corresponding error message and end the program. */
void setMachineId(char * id) 
{
  //Get machineId from argument safely
  char * end = NULL;
  int errno;
  long longID = strtol(id,&end,10);

  //If the number is NULL, then it can't be read and used.
  if(end == id) 
  {
    fprintf(stderr,"Couldn't read machine number\n");
    exit(EXIT_FAILURE);
  }
  
  //If the machine ID is not within the range of possible long integers, return an error message.
  if(errno == ERANGE && (longID == LONG_MAX || longID == LONG_MIN)) 
  {
    fprintf(stderr,"Failed to get machine number\n");
    exit(EXIT_FAILURE);
  }
  //Initialize variable
  machineId =  (int) longID;
}

/* This function reads in the file containing the machine informaiton (name, ip address, port number) and puts this 
information into the "machines" array. If the file is not completely read, or cannot be read for some reason, the function
outputs an error message. */
void initializeMachineTable(char * name) 
{
  //Open the machines file
  FILE * machineFile;
  int i = 0;
  machineFile = fopen(name, "r");

  if(machineFile != NULL) 
  {
    while(1) 
    {
      //Store file contents into machine array.
      if(fscanf(machineFile,"%s %s %d", machines[i].name, machines[i].ip, &machines[i].port) == 3) {
        ++i;
      } else 
      {
        //Done loading contents, either close file or instead return an error based on number of lines read.
        if(i == numMachines) {
          fclose(machineFile);
          break;
        }
        else 
        {
          fprintf(stderr,"Did not scan all lines of machine file\n");
          exit(EXIT_FAILURE);
        }
      }
    }
  } 
  else 
  {
    //Exit if cannot open file
    fprintf(stderr,"Could not open machines file\n");
    exit(EXIT_FAILURE);
  }
}

/* This function reads in the file containing the costs between the differnet machines and puts this 
information into the "costs" array. If the file is not completely read, or cannot be read for some reason, the function
outputs an error message. */
void initializeCostTable(char * name) 
{
  //Open file
  FILE * costFile;
  int i = 0;
  costFile = fopen(name,"r");

  if(costFile != NULL) 
  {
    while(1) 
    {
      //Load contents into cost matrix
      if(fscanf(costFile,"%d %d %d %d",&cost_matrix[i][0],&cost_matrix[i][1],&cost_matrix[i][2],&cost_matrix[i][3]) == 4)
        ++i;
      else 
      {
        //Done loading contents, either close file or instead return error based on amount of data read.
        if(i == numMachines) 
        {
          fclose(costFile);
          break;
        }
        else 
        {
          fprintf(stderr,"Did not scan all lines of cost file\n");
          exit(EXIT_FAILURE);
        }
      }
    }
  } else 
  {
    //Exit if cannot open file
    fprintf(stderr,"Could not open costs file\n");
    exit(EXIT_FAILURE);
  }
}

/* This function prints out the total cost table from each machine to every other machine. While the 
function is printing out the table, the threads are locked so that no new cost informaiton can be changed
while the table is being printed by the function. */
void printTable() {
  int i, j;
  //Action of printing table should be locked
  pthread_mutex_lock(&lock);
  //Loop through each machine
  for(i = 0; i < numMachines; ++i) {
    printf("Machine %d:\t",i);
    //For each machinie, print the cost to every other machine
    for(j = 0; j < numMachines; ++j) {
      printf("%d\t",cost_matrix[i][j]);
    }
    printf("\n");
  }
  pthread_mutex_unlock(&lock);
}

/* This function sets up a socket connection, and then waits for cost changes from any machine as part 
of the network. Once a cost change is received, then every machine can update their respective cost 
tables with the new costs being received. Essentially this function waits for a cost change, then updates
the new cost to each of the cost tables (since each machine can essentially have its own cost table) and 
finally prints out a new cost table with the new costs. */
void * receiveInfo(void * arg) 
{
  int edge[3];

  //Set up Socket to receive
  int sock;
  int portNum = machines[machineId].port;
  struct sockaddr_in server_addr, client_addr;
  struct hostent *host;
  int addrSize = sizeof(struct sockaddr);

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portNum);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
  {
    fprintf(stderr,"Socket Error\n");
    exit(EXIT_FAILURE);
  }

  if (bind(sock,(struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) 
  {
    fprintf(stderr,"Socket bind error\n");
    exit(EXIT_FAILURE);
  }

  while(1) 
  {
    printf("Waiting to receive cost change\n");
    //Wait to receive cost change from any machine.
    if(recvfrom(sock,edge,sizeof(edge),0,(struct sockaddr *) &client_addr,&addrSize) > 0) 
    {
        printf("Received new cost\n");
        //Change cost_matrix
        updateCost(edge);
        //Print new cost_matrix
        printf("Updated cost\n");
        printTable();
    }
  }
}

/* This function runs Dijkstra's algorithim every 10-20 seconds on the network to find the lowest cost path
from the machine to every other machine on the network. This function has to lock threads (so that no new cost
changes can be done) and then once the calculations are complete, the function prints out the lowest cost values
from the given machine (the one running this program) to every other machine within the network. */
void * linkState(void * arg) 
{
  //Declare variables for Dijkstra's alg
  int leastDistance[4];
  int visited[4];
  int wait;
  int i, j;
  int curCost;
  int minCost;
  int minIndex;
  
  while(1) 
  {
    //Print current table
    printf("Current cost table:\n");
    printTable();

    //Running the algorithm should be locked
    pthread_mutex_lock(&lock);

    //Initialize current edges as least distance and mark all machines as not visited
    for(i = 0; i < numMachines; ++i) 
    {
      leastDistance[i] = cost_matrix[machineId][i];
      visited[i] = 0;
    }

    //Don't need to visit current machine
    visited[machineId] = 1;

    //Visit the remaining machines
    for(i = 1; i < numMachines; ++i) 
    {
        //Find Closest unvisited machine, save machine Id
        minCost = INF;        
        for(j = 0; j < numMachines; ++j) 
        {
          if(visited[j] != 1 && minCost > leastDistance[j]) 
          {
            minCost = leastDistance[j];
            minIndex = j;
          }        
        }
        visited[minIndex] = 1;

        //Update Least distances to every other node based on edges of the machine last visited
        for(j = 0; j < numMachines; ++j) {
          if(visited[j] != 1 && leastDistance[minIndex] + cost_matrix[minIndex][j] < leastDistance[j])
          {
            leastDistance[j] = leastDistance[minIndex] + cost_matrix[minIndex][j];
          }
        }    
    }

    //Finished algorithm, can unlock
    pthread_mutex_unlock(&lock);

    //Print stored least costs
    printf("\nLeast Costs from Machine %d:\n",machineId);
    for(i = 0; i < numMachines; ++i) 
    {
      printf("Least cost to Machine %d is %d\n",i,leastDistance[i]);    
    }

    //Wait for 10 to 20 seconds
    wait = rand() % 11 + 10;
    sleep(wait);
  }
}

/* This function takes in a machineID and a cost, and then changes the distance from the machine running the program
to the new cost entered by the user. So for example, if this program was running on machine 0, and the user enters in the 
values of "1" and "10", then the distance from machine 0 to machine 1 will be set to "10". The function waits every 10
seconds between cost changes and will also not allow machines running the program to change their self cost (the cost 
from machine 1 to machine 1 will be 0) since we cannot have machines that have a cost to move information to themselves. */
void changeCost() 
{
  int edge[3];
  edge[0] = machineId;
  
  //Set up some of the socket information
  int sock;
  int portNum;
  struct sockaddr_in server_addr;
  struct hostent *host;
  int addrSize = sizeof(struct sockaddr);

  int i;
  while(1) 
  {
    sleep(10);
    printf("Wait for input: MachineId Cost\n");

    //Scan new machineId and cost value into edge[1] and edge[2] respectively
    scanf("%d %d", &edge[1], &edge[2]);

    //We cannot change the cost to self machine and we can't use an invalid Machine Id
    if(edge[1] >= numMachines || edge[0] == edge[1]) 
    {
      printf("Invalid input, try again\n");
      continue;
    }

    //Change cost matrix of this machine
    updateCost(edge);

    //Send the new cost to every machine that is part of the network
    for(i = 0; i < numMachines; ++i) 
    {
      if(i != edge[0]) 
      {
        //Use that macines's port number
        portNum = machines[i].port;
        host = (struct hostent *) gethostbyname(machines[i].ip);
        //Make a new socket
        if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
        {
          fprintf(stderr,"Socket error\n");
          exit(EXIT_FAILURE);        
        }
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(portNum);
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        //Send updated edge to other machines
        printf("Sending to Machine %d on port %d\n",i,portNum);
        sendto(sock, edge, sizeof(edge), 0, (struct sockaddr *)&server_addr , addrSize);
      }
    }
    printTable();
  }
}

/* This function takes in a given machine (that being edge[1]) and changes the cost between the machine
running the program (which in this case is edge[0]) and the given machine. The new cost value is stored in
edge[2] and so the function changes the value from the given machine to the machine running the program and 
vice versa (since both directions should have the same cost). */
void updateCost(int * edge) 
{
  //Lock cost_matrix while it is updated
  pthread_mutex_lock(&lock);

  //Change the cost given
  cost_matrix[edge[0]][edge[1]] = edge[2];

  //Change cost of the same edge reversed
  cost_matrix[edge[1]][edge[0]] = edge[2];

  //Unlock cost_matrix since we are done updating it
  pthread_mutex_unlock(&lock);
}