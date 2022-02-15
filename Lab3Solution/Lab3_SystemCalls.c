/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab #3 - Part 1 
* Description: This program reads a binary/text file and makes a copy of it.
* This program relies on only system functions (open, read, write, and close) 
* to perform its task.
*/

#include <stdio.h> /* EOF, FILE, fopen, fread, fwrite, fclose char, size_t */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#define SIZE 10 

/* Main function of the program with 2 command line arguments that are the original file 
(to be read) and the file that is to be written to (the copy of the original file). The 
program uses system functions   */
int main(int argc, char* argv[])
{
    if(argc!=3)
    {
	    printf("expected two arguments\n");
	    return -1;
    }
    char buff[SIZE];

    //Open the original file to be read.
    int fdold = open(argv[1],O_RDONLY);
    if (fdold < 0)
    {
        return -1;
    }

    //Open the new file to be written to. If it does not already exist,
    //it is then created.
    int fdnew = open(argv[2],O_CREAT|O_WRONLY);
    if (fdnew < 0)
    {
        return -1;
    }
    
    //Write to the new file until we reach the end of original file.
    int checkIfRead;
    while ((checkIfRead = read(fdold,buff,sizeof(buff))) > 0)
    {
        write(fdnew,buff,checkIfRead);
    }

    //Close both files once the writing of the new file is complete.
    close(fdold);
    close(fdnew);
    return 0;
}
