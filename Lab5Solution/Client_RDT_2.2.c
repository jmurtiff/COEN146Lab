/*
* Name: Jordan Murtiff
* Date: 4/28/2020
* Title: Lab #5 - Stop and Wait for an Unreliable Channel
* Description: This program implements rdt2.2 on the client side.
*/

//Libraries used
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

/* The checksum function is used to ensure that   */
//NOTE: beware of sign extension if the checksum is stored in a char, then converted to an int!
int getChecksum(Packet packet) 
{
    //Reset to 0 because we want to calculate the checksum the same way the server did to ensure that the values are the same.
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

/* This function continually sends packets to the server and then waits for acknowledgment 
by the server. When the server sends an acknowledgment (with ACK number and checksum) the client 
checks to see if it has received the correct ACK value and checksum, if it has, then the client 
will end the function, otherwise, the client will resend the same packet and wait for a new 
acknowledgment from the server. The function will continue until it receives the correct 
ACK value and checksum from the server.  */
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

/* The main function takes in arguments by the user and begins sending packets to
a server using rdt 2.2. Once the input file is entirely sent to the server (correctly,
barring that the client may need to retransmit packets) the client sends a final packet
with a length of "0" to signal that the connection should be ended. */
int main(int argc, char *argv[]) 
{
    //Check arguments given by the user.
    if (argc != 4) 
    {
        fprintf(stderr, "Usage: %s <ip> <port> <infile>\n", argv[0]);
        return 1;
    }

    //Seed the RNG
    srand((unsigned)time(NULL));

    //Create and configure the socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    //Initialize the server address structure using argv[2] and argv[1]
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    socklen_t addr_len = sizeof(address);
    struct hostent *host;
    host = (struct hostent *) gethostbyname(argv[1]);
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[2]));
    address.sin_addr = *((struct in_addr *)host->h_addr);

    //Open file using argv[3]
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
        printf("Client sending file contents to server...\n")
        packet.header.seq_ack = seqnum;
        packet.header.len = strlen(packet.data);
		packet.header.cksum = getChecksum(packet);
        ClientSend(sockfd, (struct sockaddr *)&address, addr_len, packet);
        seqnum = (seqnum + 1) % 2;
    }

    //Send zero-length packet to server to end connection
    Packet zero_length;
    zero_length.header.seq_ack = seqnum;
    zero_length.header.len = 0;
    zero_length.header.cksum = getChecksum(zero_length);
    ClientSend(sockfd, (struct sockaddr *)&address, addr_len, zero_length);

    //Clean up and close the sockets and file
    close(fp);
    close(sockfd);

    return 0;
}