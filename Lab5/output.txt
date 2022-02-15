/*
* Name: Jordan Murtiff
* Date: 4/28/2020
* Title: Lab #5 - Stop and Wait for an Unreliable Channel
* Description: This program implements rdt2.2 on the server  side.
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
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
	if(rand() %2 == 1)
	{
    	while (ptr < end) 
    	{
        //changed "ptr" to "*ptr" since we need to xor with the bits, not the address of where ptr actually is
        	checksum ^= *ptr++;
    	}
    	return checksum;
	}
	return 0;
}

void logPacket(Packet packet) 
{
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
            packet.header.seq_ack,
            packet.header.len,
            packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\" }\n");
}

//Sending a ACK to the client, i.e., letting the client know what was the status of the packet it sent.
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

Packet ServerReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) 
{
    Packet packet;

    while (1) 
    {
        //recv packets from the client
        recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        // log what was received
        printf("Received: ");
        logPacket(packet);
        int check;
		check = getChecksum(packet);
		printf("check is %d\n",check);

        if (packet.header.cksum != check) {
            printf("Bad checksum, expected checksum was: %d\n", check);
            ServerSend(sockfd, address, *addrlen, !seqnum);
        }
        else if (packet.header.seq_ack != seqnum) {
            printf("Bad seqnum, expected sequence number was:%d\n", seqnum);
            ServerSend(sockfd, address, *addrlen, !seqnum);
        }
        else {
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

    // initialize the server address structure using argv[1]
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
	// [Question]: Do we need to bind the socket on the client side? Why?/Why not?
	// No, since we do not need to use a specific client-side port for this scenario
    if ((bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr))) < 0)
    {
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    // open file using argv[2]
    int fp = open(argv[2], O_RDWR | O_CREAT, 0777);

    // get file contents from client
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
