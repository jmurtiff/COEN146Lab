/* Name: Jordan Murtiff
 * Date: 4/21/2020
 * Title: Lab 4 - Step #2 - UDP Peer-to-Peer File Transfer
 * Description: Opens a socket on the localhost as the server or
 * on a remote host as the client and then when the server
 * receives a message from the client requesting the file
 * it transfers the file over. Then they can switch to send a file back
 * if desired. Or continue sending more files the same way.
 */

//Libraries used
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//Declare socket file descriptor.
int sock;

//Declare receiving or sending buffer of size 1k bytes.
char rbuf[1024], sbuf[1024];

//Declare server address to which to bind for receiving messages and client address to fill in sending address.
struct sockaddr_in servAddr, remoteAddr;
socklen_t addrLen = sizeof(struct sockaddr);

struct hostent *host;

//Declare variables to hold the names of the source and destination files as well as the host name to connect to.
char host_name[20], source_file[20], dest_file[20];

void resolve_hostname_open_socket() 
{
    //Converts domain names into numerical IP addresses via DNS
    host = (struct hostent *)gethostbyname(host_name);

    if (host == NULL) {
        puts("Error: unable to resolve hostname.");
        exit(1);
    }

    //Setup the server address to bind using socket addressing structure
    //Infomation for server (if receiving a file)
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5001); //Port 5001 is assigned
    servAddr.sin_addr = *((struct in_addr *)host->h_addr);

    //Information for client (if sending a file)
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(5000); //Port 5000 is assigned
   	remoteAddr.sin_addr = *((struct in_addr *)host->h_addr);

    //Open a UDP socket, if successful, returns a descriptor associated with an endpoint.
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
        perror("Failure to setup an endpoint socket");
        exit(1);
    }
}

//Server function that is called when this program wishes to receive a file from a client.
void server() {
    gethostname(host_name, 20);
    printf("Server hostname: %s\n", host_name);

    resolve_hostname_open_socket();

    //Setup the address/port of the server endpoint for socket socket descriptor.
    if ((bind(sock, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0)
	{
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    //Wait for client to request a file, once you receive the message "REQUEST" from the client,
    //then open the source file and send it to the client.
    int rb;
    while ((rb = recvfrom(sock, rbuf, 1024, 0, (struct sockaddr *)&remoteAddr, &addrLen)) > 0)
	{
        rbuf[rb] = '\0';
        printf("Client sent: %s\n", rbuf);

        if (strcmp(rbuf, "REQUEST") == 0) 
		{
            break;
        }
    }

    //Server opens the source file in read only mode, if the server cannot open the source file, 
    //send the client an error.
    int fp_r = open(source_file, O_RDWR);
    if (fp_r < 0) 
	{
        puts("Error opening source file.");
        sendto(sock, "ERROR", strlen("ERROR"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
        exit(1);
    }

    //Send a message through the socket that the server is sending back to the client.
    sendto(sock, "SENDING", strlen("SENDING"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));

    //Server reads in the source file and sends it in chunks of size sbuf size to the client.
    size_t bytes;
    int file_bytes = 0;
    while (0 < (bytes = read(fp_r, sbuf, sizeof(sbuf)))) 
	{
        sendto(sock, sbuf, bytes, 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
    }

    //Once the file is completely sent to the client, send the "End of File" message to the client.
    sendto(sock, "EOF", sizeof("EOF"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));

    //Close the file since we are done sending it over to the client.
    close(fp_r);
    puts("Sent the file to client.");

    //Close the socket since the function has finished its task.
    close(sock);
}

//Client function that is called when this program wishes to send a file to a server.
void client() 
{
    puts("Please enter the server hostname you'd like to receive from:");
    scanf("%s", host_name);

    resolve_hostname_open_socket();

    //Client sends file request to the server with the message "REQUEST" to begin the file transfer.
    sendto(sock, "REQUEST", strlen("REQUEST"), 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr));
    printf("Sent file request to server.\n");

    //Client waits for a response from the server, if the server sends the message "SENDING", then
    //the servercan begin receiving the file from the server.
    int rb;
    while ((rb = recvfrom(sock, rbuf, 1024, 0, (struct sockaddr *)&remoteAddr, &addrLen)) > 0)
	{
        rbuf[rb] = '\0';
        printf("Server sent: %s\n", rbuf);

        if (strcmp(rbuf, "SENDING") == 0) {
            break;
        }
        else if (strcmp(rbuf, "ERROR") == 0) 
		{
            puts("Remote server could not open file.");
            exit(1);
        }
    }

    /*Client opens the destination file:
     *O_CREAT, If the file does not already exist, create it as a new file
     *O_RDWR, Open for reading and writing to the file
     *0777, Octal value that represents access being granted to users on the system
     */
    int fp_w = open(dest_file, O_RDWR | O_CREAT, 0777);
    if (fp_w < 0) 
	{
        puts("Error opening the destination file.");
        exit(1);
    }

    //Client receives messages from the socket (server) and writes to the output file.
    int file_bytes = 0;
    while ((rb = recvfrom(sock, rbuf, 1024, 0, (struct sockaddr *)&remoteAddr, &addrLen)) > 0)
	{
        rbuf[rb] = '\0';
        if (strcmp(rbuf, "EOF") == 0) 
		{
            puts("EOF reached!");
            break;
        }
        else {
            write(fp_w, rbuf, strlen(rbuf));
        }
    }

    //Close the file since we are done sending it over to the server.
    close(fp_w);
    puts("File was successfully written to.");

    //Close the socket since the function has finished its task.
    close(sock);
}

/* main function */
int main() 
{
    while (1) 
	{
        puts("Would you like to send (1), receive (2), or exit the program (3)?");
        int input;
        scanf("%d", &input);

        //Call the server function if we wish to send a file to a server.
        if (input == 1) 
		{
            puts("Please enter the name of the file that you'd like to send:");
            scanf("%s", source_file);
            server();
        }

        //Call the client function if we wish to receive a file from a server.
        else if (input == 2) 
		{
            puts("Please enter the name of the file that you'd like to write to:");
            scanf("%s", dest_file);
            client();
        }

        //Otherwise, if we are done with the program, then get out of the switch statement.
        else if (input == 3) 
		{
            puts("Closing Program");
            break;
        }
        else 
		{
            puts("Error choosing number, please try a different number.");
        }
    }
    return 0;
}