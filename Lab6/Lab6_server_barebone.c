/*
* Name: Jordan Murtiff
* Date: 5/5/2020
* Title: Lab #6 - Stop and Wait for an Unreliable Channel with Loss
* Description: This program implements rdt 3.0 on the server side.
*/

//Explain what each library does in the context of this lab. Do we need all of them?

//arpa/inet.h allows for the htons function

//stdio.h allows for the print statement as well as the file pointers, the EOF variable, 
//the NULL variable, the size_t type variable, and finally the perror() messages

//stdlib.h allows for the atoi() function as well as the rand() function

//string.h allows for the memset() and strlen() function

//sys/socket.h allows for the AF_INET variable as well as the SOCK_DGRAM variable, additionally it defines the 
//socket(), bind(), recvfrom(), and sendto() functions

//netinet/in.h allows for the sockaddr_in struct (which includes the sin_family, sin_port, and sin_addr variables)

//fcntl.h allows for the O_CREAT flag used by the write() function

//time.h isn't used anywhere in this program, its unecessary 

//unistd.h allows for the read() and write() functions 

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
//#include <time.h>
#include <unistd.h>
#include "lab6.h"
int PLOSTMSG = 10;

//NOTE: beware of sign extension if the checksum is stored in a char, then converted to an int!
int getChecksum(Packet packet) 
{
    //Reset to 0 because we want to calculate the checksum the same way the client did to ensure that the values are the same.
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
	if(rand() %10  == 5)
	{
    	while (ptr < end) 
    	{
        //Changed "ptr" to "*ptr" since we need to xor with the bits, not the address of where ptr actually is
        	checksum ^= *ptr++;
    	}
    	return checksum;
	}
	return 0;
}

/* This function prints out all the information in a given packet including its sequence number,
packet length (which is at maximum 10) and the checksum value. */
void logPacket(Packet packet) 
{
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

void ServerSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum)
 {
    // Simulating a chance that ACK gets lost
    if (rand() % PLOSTMSG == 5) 
    {
        return;
    }
    //prepare and send the ACK
    Packet packet;
    packet.header.seq_ack = seqnum;
    packet.header.len = sizeof(packet.data);
    packet.header.cksum = getChecksum(packet);
    sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
    printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}

Packet ServerReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum)
{   
    Packet packet;
    while (1) 
    {
        //Receive a packet from the client
        recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        // log what was received
        printf("Received: ");
        logPacket(packet);

        int check = getChecksum(packet);

        //verify the checksum and the sequence number
        if (packet.header.cksum != check) 
        {
            printf("Bad checksum, expected %d\n", check);
            ServerSend(sockfd, address, *addrlen, !seqnum);
        } 
        else if (packet.header.seq_ack != seqnum) 
        {
            printf("Bad seqnum, expected %d\n", seqnum);
            ServerSend(sockfd, address, *addrlen, !seqnum);
        } 
        else 
        {
            printf("Good packet\n");
            ServerSend(sockfd, address, *addrlen, seqnum);
            break;
        }
    }
    printf("\n");
    return packet;
}

int main(int argc, char *argv[])
{
    // check arguments
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create a socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // initialize the server address structure
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;


    // bind the socket
    if((bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr))) < 0)
    {
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    //Please answer the following questions
        //[Q1] Briefly explain what is "AF_INET" used for.
        //[Q2] Briefly explain what is "SOCK_DGRAM" used for.
        //[Q3] Briefly explain what is "htons" used for.
        //[Q4] Briefly explain why bind is required on the server and not on the client.
    
        //Answer for [Q1]: 
        //Answer for [Q2]:
        //Answer for [Q3]:
        //Answer for [Q4]:
    
    
    int fp = open(argv[2], O_RDWR | O_CREAT, 0777);

    // get file contents from client and save it to the file
    int seqnum = 0;
    Packet packet;
    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    do 
    {
        packet = ServerReceive(sockfd, (struct sockaddr *)&clientaddr, &clientaddr_len, seqnum);
        write(fp, packet.data, strlen(packet.data));
        seqnum = (seqnum + 1) % 2;
    } while (packet.header.len != 0);

    //cleanup
    close(fp);
    close(sockfd);
    
    return 0;
}
