/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab3 - Part 1 
* Description: This program copies a binary/text file to another file. 
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#define SIZE 10 

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
	    printf("expected two arguments\n");
	    return -1;
    }
    char buff[SIZE];

    int fdold = open(argv[1],O_RDONLY);
    if (fdold < 0)
    {
        return -1;
    }

    int fdnew = open(argv[2],O_CREAT|O_WRONLY);
    if (fdnew < 0)
    {
        return -1;
    }
    int checkIfRead;
    while ((checkIfRead = read(fdold,buff,sizeof(buff))) > 0)
    {
        write(fdnew,buff,checkIfRead);
    }
    close(fdold);
    close(fdnew);
    return 0;
}
