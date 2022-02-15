/*
* Name: Jordan Murtiff
* Date: 5/5/2020
* Title: Lab #6 - Stop and Wait for an Unreliable Channel with Loss
* Description: This program implements rdt 3.0 on the server side.
*/

/*QUESTION: Explain what each library does in the context of this lab. Do we need all of them?

1. arpa/inet.h allows for the htons() function

2. stdio.h allows for the print statements as well as the file pointers, the EOF variable, 
   the NULL variable, the size_t type variable, and finally the perror() messages

3. stdlib.h allows for the atoi() function as well as the rand() function

4. string.h allows for the memset() and strlen() function

5. sys/socket.h allows for the AF_INET variable as well as the SOCK_DGRAM variable, additionally it defines the 
socket(), bind(), recvfrom(), and sendto() functions

6. netinet/in.h allows for the sockaddr_in struct (which includes the sin_family, sin_port, and sin_addr variables)

7. fcntl.h allows for the O_CREAT flag used by the write() function

8. time.h isn't used anywhere in this program, its unecessary 

9. unistd.h allows for the read() and write() functions 

So the only library that is unecessary is time.h, so no, we do not need all the libraries that are used below. */

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
#include "lab6.h"

//Include the variable that will be used for the probability of a dropped acknowledgment (same as a lost message).
int PLOSTMSG = 10;

/* The checksum function is used to ensure that a given packet is not tampered with or altered in some way as 
it goes from server to client or from client to server.  */
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

/* This function prepares a packet that is used to acknowledge the the client, i.e., letting the client 
know what was the status of the packet it sent. There is a chance, however, that the server will not send
an acknowledgment back to the client, in which case the client will resend its original packet. */
void ServerSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum)
 {
    //Simulating a chance that ACK gets lost
    if (rand() % PLOSTMSG == 5) 
    {
        return;
    }
    //Prepare and send the ACK
    Packet packet;
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
        //Receive a packet from the client
        recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);

        //Log what was received
        printf("Received: ");
        logPacket(packet);

        int check = getChecksum(packet);

        //Verify the checksum and the sequence number
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

/* The main function takes in arguments by the user and begins receiving packets from a client
using rdt 3.0. Once the input file is entirely received to the server (correctly,
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

    //Initialize the server address structure
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(argv[1]));
    address.sin_addr.s_addr = INADDR_ANY;

    //Bind the socket
    if((bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr))) < 0)
    {
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    //Please answer the following questions:
        //[Q1] Briefly explain what is "AF_INET" used for.
        //[Q2] Briefly explain what is "SOCK_DGRAM" used for.
        //[Q3] Briefly explain what is "htons" used for.
        //[Q4] Briefly explain why bind is required on the server and not on the client.
    
        /*Answer for [Q1]: AF_INET is an address family that is used to designate the type of 
        addresses that a socket can communicate with (in this case, Internet Protocol v4 addresses). 
        Since we are createing a new socket, we have to specify its address family, and then we 
        can only use addresses of that type with the socket. So in essence AF_INET just specificies that
        the address we will use for the socket will be IPV4 addresses. */

        /*Answer for [Q2]: SOCK_DGRAM means our socket supports datagrams (which are connectionless, unreliable
        messages of a fixed maximum length). This is essentially telling our socket will be of UDP type. */

        /*Answer for [Q3]: The htons function converts an unsigned short integer from host byte order to network byte order.
        Essentially, the function takes our IP address and converts it to a value that can be read
        by machines with different byte order conventions and allow them to communicate with one another. */

        /*Answer for [Q4]: Because we are using UDP and UDP is connectionless, we only need to use the bind function on the server
        and not the client. The server is essentially specifying a name in which a client can send requests to the server. */
    
    //Open file using argv[2]
    int fp = open(argv[2], O_RDWR | O_CREAT, 0777);

    //Get the file contents from client and continually write it to the given output 
    //file until a packet of length 0 is received by the server.
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

    //Clean up and close the sockets and file
    close(fp);
    close(sockfd);
    return 0;
}