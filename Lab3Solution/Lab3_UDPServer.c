/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab #3 - Part 5
* Description: This program is the server side of a Client – Server with UDP/IP
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
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h> 
#define SIZE 1024

/* The main function of the program, takes in a file and makes a copy of it and 
sends it to a client via socket programming. */
int main()
{
    //Declare socket file descriptor.
    int sock;

    int bytes_read;

    //Declaring the receiving buffer of size 1 kB.
    char recv_data[SIZE];

    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;

    //Open the socket we are going to use.
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Socket");
        exit(1);
    }

    //Set the sever address that we are going to send using the socket addressing structure.
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY; 

   //Setting the address/port of server endpoint for socket socket descriptor.
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Bind");
        exit(1);
    }

    addr_len = sizeof(struct sockaddr);
    printf("\t\t\t\t\t\t\nUDPServer Waiting for client on port 5000\n");

    //Infinite loop to continually receive files from the client
    while (1)
    {
        //Receive from client, and print out what the server is receiving.
        bytes_read = recvfrom(sock, recv_data, SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        recv_data[bytes_read] = '\0';
		printf("\t\t\t\t Server: The Client (%s , %d) said: ", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		printf("%s\n", recv_data);

        // Open another file for writing, the one to send to the client.
        FILE *fileAddress;
        fileAddress = fopen("dest.dat", "wb");

        //While you are not at the end of the source file, continue writing the destination file.
        if (!feof(fileAddress))
        {
           	 fwrite(recv_data,1, bytes_read, fileAddress);
        }

        else
        {
            printf("\n Unable to Create or Open the dest.dat File");
        }

        fclose(fileAddress);
        printf("\t\t\t\t\t\t\n File received by server.\n");        
    }
    return 0;
}

