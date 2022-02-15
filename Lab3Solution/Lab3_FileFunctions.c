/* 
* Name: Jordan Murtiff
* Date: 4/14/2020
* Title: Lab #3 - Part 1 
* Description: This program reads a binary/text file and makes a copy of it.
* This program relies on only file functions (fopen, fclose, fread, and fwrite) 
* to perform its task.
*/

#include<stdio.h> /* EOF, FILE, fopen, fread, fwrite, fclose char, size_t */
#include<assert.h> /* assert */
#define SIZE 10 /* used for the size of the character buffer used to store parts of the original file while reading  
                   it and writing to the new file /*

/* Main function of the program with 2 command line arguments that are the original file 
(to be read) and the file that is to be written to (the copy of the original file). Two file
pointers are created and a character buffer that will hold parts of the original file as it is
being read by the program. Both files are opened and a while loop continually reads the original 
file while subsequently writing to the new file. The fread function reads 10 chunks of 1 byte each 
of the original file and places these chunks into a buffer and then returns the number of elements 
successfully read into the "count" variable. Once the fread function has completed, the fwrite 
function writes to the new file based on "count's" number of 1 byte chunks that have been read. So
in essence, the program reads 1 byte chunks and only writes at most 10 chunks of 1 byte each to the 
new file. If less than 10 chunks are read, then only the number of chunks in the "count" variable 
will be written to the new file. Once the original file has been completed read (when the FILE pointer
is at the EOF) and written, then both FILE pointers are closed and the program ends.  */
int main(int argc, char* argv[])
{
  if(argc!=3)
  {
    printf("Error: expected two arguments needed for copy of binary/text file\n");
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