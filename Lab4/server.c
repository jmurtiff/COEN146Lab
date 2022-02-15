/* 
* Name: Jordan Murtiff
* Date: 4/21/2020
* Title: Lab #4 Step #1
* Description: To build a concurrent server using TCP/IP socket to accept connections from multiple clients at the same time.
* This is the server file of the client-server pair.
*/

//Libraries used
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>

//Define the number of possible parallel client connections.
#define NCONNS 5

//File pointer that the server is going to write to.
FILE *filePTR;

//Function template.
void *start_routine(void *arg);

//An array of threads based on the total number of possible parallel client connections.
pthread_t tids[NCONNS];

/* main function */
int main()
{
    //Declare socket file descriptor.
    int sock;
    int sin_size;

    //Variable used for the for loops.
    int i;

    //Declare server address to which to bind for receiving messages and client address to fill in sending address
    struct sockaddr_in server_addr, client_addr;

    //Open a TCP socket, if successful, returns a descriptor associated with an endpoint.
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }

    //Setup the server address to bind using socket addressing structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //Setup the address/port of the server endpoint for socket socket descriptor.
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Unable to bind");
        exit(1);
    }
    printf("\n\t\t\tTCPServer Waiting for client on port 5000: \n");
    
    //Wait for a client to connect to the server.
    if (listen(sock, 5) == -1)
    {
        perror("Listen");
        exit(1);
    }

    /* The for loop makes is so that we only create the highest number of threads based on the
    number of parallel client connections that is defined in this file. Each time a client connects
    to the server, a new thread is created and runs the start_routine function. */
	for(i = 0; i < NCONNS; i++)
	{
		int CONN;
		CONN = accept(sock, (struct sockaddr *) &client_addr, (socklen_t *)&sin_size);
		puts("Server: A new connection accepted");
		pthread_create(&tids[i], NULL, start_routine, &CONN);
    }

    //Join all of the created threads so that all threads finish before the thread that runs the 
    //"main" finishes.
	for(i = 0; i < NCONNS; i++)
	{
		pthread_join(tids[i],NULL);
	}

    //Close the file being written to by the server.
	fclose(filePTR);

    //Close the socket since we are done with the program.
	close(sock);

    return 0;
}

/*This function takes an address of a socket and recives the messages from 
that socket and writes the information to a file. Since this program allows
for multiple clients to connect at the same time, the file is being appendended 
to since otherwise the file would be overriden by each thread created.*/
void *start_routine(void *arg)
{	
    char receive_data[1024];
	int bytes_received;
	int connfd = *(int*)arg;
	while((bytes_received = recv(connfd, receive_data, 1024, 0)) > 0)
    {
            filePTR = fopen("dest.dat", "a");
            if (!feof(filePTR))
            {
                fwrite(receive_data,1, bytes_received, filePTR);
            }
            else
            {
                printf("\n Unable to create or open the dest.datFile");
            }
            fflush(filePTR);
    } 
}