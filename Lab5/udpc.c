/*
 * Name: Jordan Murtiff
 * Date: 4/28/2020
 * Title: Lab 5
 * Description: Client Stop and Wait reliable protocol on top of UDP to provide a reliable transport service
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
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
void client_send(int sockfd,struct sockaddr*address,socklen_t addrlen,Packet packet);
int seq = 0;

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

Packet createpacket(char *buff, int n)
{
	printf("inside the createpacket function\n");
	Packet packet;
	strcpy(packet.data,buff);
	packet.header.seq_ack=seq;
	packet.header.len=n;
	packet.header.checksum=0;
	packet.header.checksum=get_checksum(packet);
	return (packet);
}

void client_send(int sockfd,struct sockaddr*address,socklen_t addrlen,Packet packet)
{
	Packet recv;
	if(sendto(sockfd,&packet,sizeof(Packet),0,address,addrlen)==-1)
	{
		perror("sendto");
		exit(1);
	}
	printf("send the packet, with context: %s\n", packet.data);
	while (recvfrom(sockfd,&recv,sizeof(Packet),0,NULL,NULL)!=0)
	{
		printf("receive from worked\n");
		if (recv.header.seq_ack==seq)
		{
			printf("inside the if\n");
			break;
		}
		if (sendto(sockfd,&packet,sizeof(Packet),0,address,addrlen)==-1)
		{
			perror("second sendto");
			exit(1);
		}
	}
	seq=!seq;
	printf("the client send finished\n\n\n");
};


int main(int argc,char* argv[])
{
	FILE *input;										//the input file pointer 
	input=fopen(argv[1],"rb");							//open the file name from the command line
	int sock;											//socket
	struct sockaddr_in server_addr;
	struct hostent *host;
	Packet packet;
	int count;
	char buff[10];

	printf("inside the client main\n");
	
	socklen_t addr_len=sizeof(server_addr);
	host= (struct hostent *) gethostbyname((char *)"127.0.0.1");

	// open socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	// set address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5000);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);

	while (1)
	{
		if (!feof(input))
		{
			count=fread(&buff,1,sizeof(buff),input);
		}
		else
		{
			count=0;
		}
		printf("read file successfully, and the count is %d, the context read in is: %s\n\n", count,buff);
		
		if (count==0)
		{
			printf("we send a null file to tell the server to stop\n");
			packet=createpacket("",0);				//if the number of bytes is zero, we purposely send a null data
			if(sendto(sock,&packet,sizeof(Packet),0,(struct sockaddr*)&server_addr,addr_len)==-1) //the send to funtion send sock, a NULL data,in 0 length.
			{
				perror("send a null packet");
				exit(1);
			}			
			break;
		}

		packet=createpacket(buff, count);
		client_send(sock,(struct sockaddr*)&server_addr,addr_len,packet);
   	}
	close(sock);//close the socket
}
