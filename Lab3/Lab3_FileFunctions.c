/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab3 - Part 1 
* Description: This program copies a binary/text file to another file. 
*/

#include<stdio.h>
#include<assert.h>
#define SIZE 10

int main(int argc, char* argv[])
{
    if(argc!=3)
    {
	printf("expected two arguments\n");
	return(-1);
    }

    FILE* old;
    FILE* new;
    char buff[SIZE];
    size_t count=0;
    
    old=fopen(argv[1], "rb");
    new=fopen(argv[2], "wb");
    assert(old!=NULL && new!=NULL);

    while(!feof(old))
    {
		count=fread(buff,1,SIZE,old);
		fwrite(buff,1,count,new);
    }

    fclose(old);
    fclose(new);
    
    return(0);
}
