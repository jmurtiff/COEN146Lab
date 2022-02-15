// UDP Server
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){
 //Declare socket file descriptor. All  Unix  I/O  streams  are  referenced  by  descriptors
 int sockfd;
 
 //Declare receiving buffer of size 1k bytes
 char rbuf[1024];

 //Declare server address to which to bind for receiving messages and client address to fill in sending address
 struct sockaddr_in servAddr, clienAddr;
 socklen_t addrLen = sizeof(struct sockaddr);

 //Open a socket, if successful, returns  a descriptor  associated  with  an endpoint 
 if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("Failure to setup an endpoint socket");
    exit(1);
 }
 
 //Setup the server address to bind using socket addressing structure
 servAddr.sin_family = AF_INET;
 servAddr.sin_port = htons(5000); //Port 5000 is assigned

 //Local IP address of any interfaceis assigned (generally one interface IP address)
 servAddr.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1"); 

 //Set address/port of server endpoint for socket socket descriptor
 if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0){
    perror("Failure to bind server address to the endpoint socket");
    exit(1);
 }
 
//Sever continuously waits for messages from client, then prints incoming messages.
 while (1){
    printf("Server waiting for messages from client: \n");
    int nr = recvfrom(sockfd, rbuf, 1024, 0, (struct sockaddr *)&clienAddr, &addrLen);
    rbuf[nr] = '\0';
    printf("Client with IP: %s and Port: %d sent message: %s\n", inet_ntoa(clienAddr.sin_addr),ntohs(clienAddr.sin_port), rbuf);
 }

 return 0;
}
