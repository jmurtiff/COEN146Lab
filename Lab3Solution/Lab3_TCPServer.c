/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab #3 - Part 6
* Description: This program is the server side of a Client – Server using TCP/IP
* The sever receives a file from the client and the server makes a copy of it and sends 
* it back.
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

/* The main function of the program, takes in a file and makes a copy of it and 
sends it to a client via socket programming. */
int main()
{
    int sock; 
	int connected;
    int bytes_recieved;
	int true = 1;
    char send_data[1024], recv_data[1024];
    struct sockaddr_in server_addr, client_addr;
    int sin_size;
    //Open socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }
    //Set address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    //Bind socket
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Unable to bind");
        exit(1);
    }
    printf("\t\t\t\t\t\t\nTCPServer Waiting for client on port 5000: \n");
    //Listen
    if (listen(sock, 5) == -1)
    {
        perror("Listen");
        exit(1);
    }
    sin_size = sizeof(struct sockaddr_in);
    while (1)
    {
        //Accept
        connected = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *)&sin_size);
        printf("I got a connection from (%s , %d)\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        while ((bytes_recieved = recv(connected, recv_data, 1024, 0)) > 0)
        {
            printf("\t\t\t\t Server: The Client (%s , %d) said: % s\n ",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), recv_data);
             // Open another file for writing 
            FILE *fileAddress;
            fileAddress = fopen("dest.dat", "wb");

            if (!feof(fileAddress))
            {
                fwrite(recv_data,1, bytes_recieved, fileAddress);
            }
            else
            {
                printf("\n Unable to Create or Open the dest.dat File");
            }
            fclose(fileAddress);
		  	printf("\t\t\t\t\t\t\n File recieved by server.\n");	
        }
        close(connected);
    }
    close(sock);
    return 0;
}
