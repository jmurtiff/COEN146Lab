/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab3 - Part 6
* Description: This program is the client side of a Client – Server using TCP/IP
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

/* main function */
int main()
{
    int sock, bytes_recieved;
    char send_data[1024], recv_data[1024];
    struct hostent *host;
    struct sockaddr_in server_addr;
    host = gethostbyname("127.0.0.1");
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }
    //Set address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    //Connect
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Connect");
        exit(1);
    }

    FILE *fptr1;
    fptr1 = fopen("src.dat", "rb");
    if (fptr1 == NULL) 
   	{ 
        printf("Cannot open file %s \n"); 
        exit(0); 
    } 

    while (!feof(fptr1))
    {
        int x = fread(send_data, 1, sizeof(send_data), fptr1);
        sendto(sock, send_data, x, 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    }
	printf("\n File sent to server.\n");
	
    close(sock);
    return 0;
}
    
