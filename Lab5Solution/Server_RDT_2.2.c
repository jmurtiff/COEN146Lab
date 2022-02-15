/*
* Name: Jordan Murtiff
* Date: 4/28/2020
* Title: Lab #5 - Stop and Wait for an Unreliable Channel
* Description: This program implements rdt 2.2 on the server side.
*/

//Libraries used
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include "lab5.h"

//NOTE: beware of sign extension if the checksum is stored in a char, then converted to an int!
int getChecksum(Packet packet) 
{
    //Reset to 0 because we want to calculate the checksum the same way the client did to ensure that the values are the same.
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
	if(rand() %2 == 1)
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

/* This function prepares a packet that is used to acknowledge the the client, i.e., letting the client 
know what was the status of the packet it sent. */
void ServerSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) 
{
    Packet packet;
	//-Prepare the packet headers
	//-send the packet
    packet.header.seq_ack = seqnum;
    packet.header.len = sizeof(packet.data);
    packet.header.cksum = getChecksum(packet);
    sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);

    printf("Sent ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
}

/* This function continually recives packets from the client and prints out the information of the packet. The server
then calculates the checksum and then compares both the sequence number and the checksum to the packet the client sent.  
If the checksum or sequence number was incorrect, then the server sends an acknowledgment that tells the client to 
retransmit the packet, otherwise it asks sends an acknowledgment to the client to send the next packet as part of 
the sequence of packets used to transmit a given file. */
Packet ServerReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) 
{
    Packet packet;
    while (1) 
    {
        //Recv packets from the client
        recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        //Log what was received
        printf("Received: ");
        logPacket(packet);
        int check;
		check = getChecksum(packet);

        //Check the checksum
        if (packet.header.cksum != check) 
        {
            printf("Bad checksum, expected checksum was: %d\n", check);
            ServerSend(sockfd, address, *addrlen, !seqnum);
        }

        //Check the sequence number
        else if (packet.header.seq_ack != seqnum) 
        {
            printf("Bad seqnum, expected sequence number was:%d\n", seqnum);
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

/* The main function takes in arguments by the user and begins receiving packets from a client
using rdt 2.2. Once the input file is entirely received to the server (correctly,
barring that the client may need to retransmit packets) the server waits for a final packet
with a length of "0" to signal that the connection should be ended by the server. */
int main(int argc, char *argv[]) 
{
    //Check arguments
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <port> <outfile>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //Seed the RNG
    srand((unsigned)time(NULL));

    //Create a socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    //Initialize the server address structure using argv[1]
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
	// [Question]: Do we need to bind the socket on the client side? Why?/Why not?
	// [Answer]:No, since we do not need to use a specific client-side port for this scenario
    if((bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr))) < 0)
    {
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    //Open file using argv[2]
    int fp = open(argv[2], O_RDWR | O_CREAT, 0777);

    //Get file contents from client
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

    close(fp);
    close(sockfd);
    return 0;
}
