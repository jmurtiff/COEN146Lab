/*
 * Name: Jordan Murtiff
 * Date: 4/28/2020
 * Title: Lab 5
 * Description:  Server Side: Stop and Wait reliable protocol on top of UDP to provide a reliable transport service
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
//int sock;
int seq=0;
typedef struct
{	
	int seq_ack;
	int len;
	int checksum;
}Header;

typedef struct
{
	Header header;
	char data[10];
}Packet;


int get_checksum(Packet);
Packet createpacket(char*buff, int n);
Packet myrecv(int sockfd);


int main()
{
	int sock;
    Packet packet;
    struct sockaddr_in server_addr , client_addr;
	FILE *output;														//file pointer

	output=fopen("out.txt","wb");
	printf("inside the server main\n");										//open the file in write form
    socklen_t addr_len;
// Open socket
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }
// Set address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

 //Bind socket to address
    if (bind(sock,(struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Bind");
        exit(1);
    }
    addr_len = sizeof(struct sockaddr);
		
	printf("\t\t\t\t\t\t\nUDPServer Waiting for client on port 5000\n");
	while (1)
	{
		printf("inside the while loop\n");
        //Receive from client
		packet=myrecv(sock);
		printf("the packet length is %d\n", packet.header.len);
		if (packet.header.len==0)
		{
			printf("we recieve the end and about to break\n");
			break;
		}
	    fwrite(&packet.data,1,packet.header.len,output);
	    printf("finished writing to the file\n\n\n");
    }
    printf("we are ready to close the sock\n");
    printf("close funciton return: %d\n", close(sock));
	printf("the fclose return:%d\n",fclose(output));
	exit(1);
	return 0;
}

int get_checksum(Packet packet)
{
	int checksum = 0;
	packet.header.checksum=0;
	char* ptr= (char *)&packet;
	char* ptr2 = ptr;
	srand(time(0));
	if(rand()%2 == 1) //Corrupt if even
	{
		for (ptr; ptr < ptr2 + sizeof(packet.header)+ packet.header.len; ptr++)
		{
			checksum ^= *ptr;
		}
		return checksum;
	}
	return 0;
}

Packet createpacket(char *buff, int n){
	printf("inside the createpacket function\n");
	Packet packet;
	strcpy(packet.data,buff);
	packet.header.seq_ack=seq;
	packet.header.len=n;
	packet.header.checksum=0;
	packet.header.checksum=get_checksum(packet);
	return (packet);
}

Packet myrecv(int sockfd)
{
	printf("inside the myrecv function\n");
	struct sockaddr_in client_addr;
	socklen_t addr_len;
	int checksum;
	char buff[10]="";
	Packet packet;
	Packet ack;
	while(1){
		recvfrom(sockfd,&packet,sizeof(Packet),0,(struct sockaddr*)&client_addr,&addr_len);
		printf("recvfrom finished\n");
		checksum=packet.header.checksum;
		if (checksum!=get_checksum(packet))
		{
			seq=!seq;
			ack=createpacket(buff,0);
			seq=!seq;
			if (sendto(sockfd,&ack,sizeof(Packet),0,(struct sockaddr*)&client_addr,addr_len)==-1)
			{
				perror("send to nak");
				exit(1);
			}
			printf("send the nak successfully\n");
		}
		else
		{
			ack=createpacket(buff,0);
			printf("created ack with seq number: %d\n", ack.header.seq_ack);
			if (sendto(sockfd,&ack,sizeof(Packet),0,(struct sockaddr*)&client_addr,addr_len)==-1)
			{
				perror("sendto ack");
				exit(1);
			}
			printf("send ack successfully\n");
			break;
		}
	}
	seq=!seq;
	printf("the current ack is %d\n", seq);
	return (packet);
}