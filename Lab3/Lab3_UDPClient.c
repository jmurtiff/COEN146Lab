/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab3 - Part 5
* Description: This program is the client side of a Client – Server with UDP/IP 
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
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h> 
#define SIZE 1024
/* main function*/
    int main()
    {
        int sock;
        struct sockaddr_in server_addr;
        struct hostent *host;
        char send_data[SIZE];
        socklen_t addr_len;
        host = (struct hostent *)gethostbyname((char *)"127.0.0.1");
        FILE *fptr1;
        fptr1 = fopen("src.dat", "rb");
         if (fptr1 == NULL) 
    { 
        printf("Cannot open file %s \n"); 
        exit(0); 
    } 
        // open socket
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            perror("socket");
            exit(1);
        }
        // set address
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(5000);
        server_addr.sin_addr = *((struct in_addr *)host->h_addr);
        //reading the file 
        
        while (!feof(fptr1))
        {
            int x = fread(send_data, 1, sizeof(send_data), fptr1);
                sendto(sock, send_data, x, 0,
                       (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
            //send to server
        }
		printf("\nFile sent to server\n");
    }
