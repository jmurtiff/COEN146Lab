/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab #3 - Part 6
* Description: This program is the client side of a Client – Server using TCP/IP
* The client sends a file to a server and the server makes a copy of it and sends 
* it back.
*/

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

/* The main function of the program, reads in a file and sends it to a server via socket 
programming. */
int main()
{
    //Declare socket file descriptor.
    int sock;
    
    int bytes_recieved;

    //Declaring the sending buffer of size 1 kB.
    char send_data[1024];

    //We want to convert the domain name into a numerical IP address through use of DNS.
    struct hostent *host;
    host = gethostbyname("127.0.0.1");

    struct sockaddr_in server_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }

    //Set the sever address that we are going to send using the socket addressing structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);

    //Open the socket we are going to use.
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Connect");
        exit(1);
    }

    //Open up the file we are going to read, otherwise send an error.
    FILE *fptr1;
    fptr1 = fopen("src.dat", "rb");
    if (fptr1 == NULL) 
   	{ 
        printf("Cannot open file %s \n"); 
        exit(0); 
    } 

    //Now we continually read the file and then we send its contents to the sever. 
    while (!feof(fptr1))
    {
        int x = fread(send_data, 1, sizeof(send_data), fptr1);
        sendto(sock, send_data, x, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    }
	printf("\n File sent to server.\n");
	
    //close(sock);
    return 0;
}
    
