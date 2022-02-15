/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab #3 - Part 5
* Description: This program is the client side of a Client – Server with UDP/IP. 
* The client sends a file to a server and the server makes a copy of it and sends 
* it back.
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h> 
#define SIZE 1024

/* The main function of the program, reads in a file and sends it to a server via socket 
programming. */
    int main()
    {
        //Declare socket file descriptor.
        int sock;

        //Delcaring the sever address being used.
        struct sockaddr_in server_addr;

        //We want to convert the domain name into a numerical IP address through use of DNS.
        struct hostent *host;
        host = (struct hostent *)gethostbyname((char *)"127.0.0.1");

        //Declaring the sending buffer of size 1 kB.
        char send_data[SIZE];

        socklen_t addr_len;

        //Open up the file we are going to read, otherwise send an error.
        FILE *fptr1;
        fptr1 = fopen("src.dat", "rb");
         if (fptr1 == NULL) 
    { 
        printf("Cannot open file %s \n"); 
        exit(0); 
    } 
        //Open the socket we are going to use.
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            perror("socket");
            exit(1);
        }

        //Set the sever address that we are going to send using the socket addressing structure.
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(5000);
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        
        //Now we continually read the file and then we send its contents to the sever. 
        while (!feof(fptr1))
        {
            int x = fread(send_data, 1, sizeof(send_data), fptr1);
                sendto(sock, send_data, x, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        }
		printf("\nFile sent to server\n");

        return 0;
    }
