/* 
* Name: Jordan Murtiff
* Date: 4/21/2020
* Title: Lab #4 Step #1
* Description: To build a concurrent server using TCP/IP socket to accept connections from multiple clients at the same time.
* This is the server file of the client-server pair.
*/

//Libraries used
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

/* main function */
int main(int argc, char *argv[])
{
    //Declare socket file descriptor.
    int sock;

    //Declare receiving or sending buffer of size 1kB.
    char send_data[1024], recv_data[1024];

    //Declare server address to which to bind for receiving messages and client address to fill in sending address.
    struct hostent *host;
    struct sockaddr_in server_addr;
    host = gethostbyname("127.0.0.1");
    
    //Open a TCP socket, if successful, returns a descriptor associated with an endpoint.
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }

    //Setup the server address to bind using socket addressing structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);

    //Connect to the server.
    if (connect(sock,(struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Connect");
        exit(1);
    }

    //Open up the file we are going to read, otherwise send an error.
    FILE *fptr;
    fptr = fopen(argv[1], "rb");
    if (fptr == NULL) 
    { 
        printf("Cannot open file %s \n"); 
        exit(0); 
    } 

    //Now we continually read the file and then we send its contents to the server until we reach
    //the end of the file.
    while (!feof(fptr))
    {
        int x = fread(send_data, 1, sizeof(send_data), fptr);
        sendto(sock, send_data, x, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    }

    //Close the socket once the file has been completely sent.
    close(sock);
    
    return 0;
}