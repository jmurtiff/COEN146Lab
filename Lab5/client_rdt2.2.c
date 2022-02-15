/*
* Name: Jordan Murtiff
* Date: 4/28/2020
* Title: Lab #5 - Stop and Wait for an Unreliable Channel
* Description: This program implements rdt2.2 on the client side.
*/

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "lab5.h"


//NOTE: beware of sign extension if the checksum is stored in a char, then converted to an int!
int getChecksum(Packet packet) 
{
    //Reset to 0 because we want to calculate the checksum the same way the server did
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
	if(rand() %2 == 1)
	{
    	while (ptr < end) 
    	{
        //changed "ptr" to "*ptr" since we need to xor with the bits, not the address where ptr actually is
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



void ClientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet) 
{
    while (1) 
    {
        //Send the packet
        printf("Sending packet\n");
        sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        //Receive an ACK from the server
        Packet recvpacket;
        recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, address, &addrlen);

        //Log what was received
        printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);

		int check = getChecksum(recvpacket);
		printf("recv packet is %d\n", recvpacket.header.cksum);
		printf("check is %d\n", check);

        //Validate the ACK
        if (recvpacket.header.cksum != check) 
        {
            //Bad checksum, resend packet
            printf("Bad checksum, expected checksum was: %d\n", check);
        }
        else if (recvpacket.header.seq_ack != packet.header.seq_ack) 
        {
            //Incorrect sequence number, resend packet
            printf("Bad seqnum, expected sequence number was: %d\n", packet.header.seq_ack);
        }
        else 
        {
            // good ACK, we're done
            printf("Good ACK\n");
            break;
        }
    }
    printf("\n");
}

int main(int argc, char *argv[]) 
{
    // check arguments
    if (argc != 4) 
    {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }

    // seed the RNG
    srand((unsigned)time(NULL));

    // create and configure the socket [name the socket sockfd]
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    // initialize the server address structure using argv[2] and argv[1]
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    socklen_t addr_len = sizeof(address);
    struct hostent *host;
    host = (struct hostent *) gethostbyname(argv[1]);
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));
    address.sin_addr = *((struct in_addr *)host->h_addr);

    // open file using argv[3]
    int fp = open(argv[3], O_RDWR);

    // send file contents to server
	//-create a packet
	//-set the seq_ack field 
	//-set the len field
	//-calculate the checksum
	//-send the packet
    Packet packet;
    int seqnum = 0;
    while(read(fp, packet.data, sizeof(packet.data)) > 0)
    { // Reads from file then sends to server
        printf("Client sending file contents to server...\n");
        packet.header.seq_ack = seqnum;
        packet.header.len = strlen(packet.data);
		packet.header.cksum = getChecksum(packet);
        ClientSend(sockfd, (struct sockaddr *)&address, addr_len, packet);
        seqnum = (seqnum + 1) % 2;
    }

    // send zero-length packet to server to end connection
    Packet zero_length;
    zero_length.header.seq_ack = seqnum;
    zero_length.header.len = 0;
    zero_length.header.cksum = getChecksum(zero_length);
    ClientSend(sockfd, (struct sockaddr *)&address, addr_len, zero_length);

    // clean up
	//-close the sockets and file
    close(fp);
    close(sockfd);

    return 0;
}
