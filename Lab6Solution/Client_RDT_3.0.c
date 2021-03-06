/*
* Name: Jordan Murtiff
* Date: 5/5/2020
* Title: Lab #6 - Stop and Wait for an Unreliable Channel with Loss
* Description: This program implements rdt 3.0 on the client side.
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
#include "lab6.h"

//Include the variable that will be used for the probability of a dropped packet (same as a lost message).
int PLOSTMSG = 10;


/* The checksum function is used to ensure that a given packet is not tampered with or altered in some way as 
it goes from server to client or from client to server.  */
int getChecksum(Packet packet) 
{
    //Reset to 0 because we want to calculate the checksum the same way the server did to ensure that the values are the same.
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
	if(rand() %10 == 5)
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

/* This function continually sends packets to the server (unless they are dropped randomly) and then waits 
a set amount of time for acknowledgment by the server. If the client waits the set amount of time and the client
has not received an acknowledgment, the client then increments the "attempts" variable and tries to resend the 
same packet back to the server. When the client does receive an acknowledgment from the server (with ACK number and 
checksum) the client checks to see if it has received the correct ACK value and checksum, if it has, then the client 
will end the function, otherwise, the client will resend the same packet and wait for a new acknowledgment from the 
server. The function will continue until it receives the correct ACK value and checksum from the server. */
void ClientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, unsigned retries) 
{
    int attempts = 0;
    while (1) 
    {
        if (attempts == retries)
        {
            break;
        }
        packet.header.cksum = getChecksum(packet);
        
        //Log what is being sent to the server.
        printf("Created: ");
        logPacket(packet);

        //Simulate a lost packet with a random number generator.
        if (rand() % PLOSTMSG == 5) 
        {
            printf("Dropping packet\n");
        } 
        else 
        {
            sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        }

        //Now wait until either a packet is received or timeout by use of the select statement.
        struct timeval tv;
        int rv;
        fd_set readfds;
        fcntl(sockfd, F_SETFL, O_NONBLOCK);
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        //If timeout, then increment attempts and move back to the top of the loop.
        rv = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        if(rv == 0)
        {
            attempts++;
        }

        //If packet is received, then carry out functions as in rdt 2.2.
        else if (rv == 1)
        {
            Packet recvpacket;
            recvfrom(sockfd, &recvpacket, sizeof(Packet), 0, NULL, NULL);
            printf("Received ACK %d, checksum %d - ", recvpacket.header.seq_ack, recvpacket.header.cksum);

            int check = getChecksum(recvpacket);

            //Validate the ACK
            if (recvpacket.header.cksum != check) 
            {
                //Bad checksum, resend packet
               printf("Bad checksum, expected %d\n", check);
            } 
            else if (recvpacket.header.seq_ack != packet.header.seq_ack) 
            {
                //Incorrect sequence number, resend packet
                printf("Bad seqnum, expected %d\n", packet.header.seq_ack);
            } 
            else 
            {
                //Good ACK, we're done
                printf("Good ACK\n");
                break;
            }
        }
    }
    printf("\n");
}

/* The main function takes in arguments by the user and begins sending packets to
a server using rdt 3.0. Once the input file is entirely sent to the server (correctly,
bearing in mind that the client may need to retransmit packets) the client sends a final 
packet with a length of "0" to signal that the connection should be ended. The number of 
retires for a packet sent to the server is given to the "ClientSend" function (100 in the case
of sending the file, and 3 in the case of receiving the final acknowledgment for the packet
of length 0). */
int main(int argc, char *argv[]) 
{
    //Check arguments given by the user.
    if (argc != 4) {
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

    //Send file contents to server
	//-Create a packet
	//-Set the seq_ack field 
	//-Set the len field
	//-Calculate the checksum
	//-Send the packet
    Packet packet;
    int seqnum = 0;
    while(read(fp, packet.data, sizeof(packet.data)) > 0)
    { // Reads from file then sends to server
        printf("Client sending file contents to server...\n");
        packet.header.seq_ack = seqnum;
        packet.header.len = strlen(packet.data);
		packet.header.cksum = getChecksum(packet);
        ClientSend(sockfd, (struct sockaddr *)&address, addr_len, packet, 100);
        seqnum = (seqnum + 1) % 2;
    }

    //Send a zero-length packet to the server to end connection according the description provided in the Lab manual
    Packet zero_length;
    zero_length.header.seq_ack = seqnum;
    zero_length.header.len = 0;
    zero_length.header.cksum = getChecksum(zero_length);
    ClientSend(sockfd, (struct sockaddr *)&address, addr_len, zero_length, 3);

    //Clean up and close the sockets and file
    close(fp);
    close(sockfd);
    return 0;
}