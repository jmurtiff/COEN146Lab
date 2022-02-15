// Name: Jordan Murtiff
// Date: 3/31/20
// Title: Lab 1 Step 5 – Unix/Linux Commands and Overview of C Programming
// Description: This program is a rewrite of the program given in step 2 of Lab #1. 
// Instead of using forking, this program uses threads to process through the two
// different for loops that were done in part 1 of Lab #1.

//Necessary Libraries
#include <stdio.h>      /* printf, stderr */
#include <stdlib.h>     /* atoi */
#include <pthread.h> 	/* pthread_create*/

/* This function acts as the start of the thread created in the main function. 
This function takes in a void pointer that acts as the number of microseconds
of delay before moving to the next iteration of the for loop. With each 
iteration of the for loop, the function will print out which iteration 
it is currently on accompanied with the words "Parent Process". There is no
return value for this function. */
void *start_routine(void *arg)
{
    //Parent process
    int i;
    for (i=0;i<100;i++) 
    {
        printf("\t \t \t Parent Process %d \n",i);
        usleep(*(int*)arg);
    }
}

/* Main function of the program with a command line argument that acts as the 
number of microseconds delay before moving to the next iteration of a for loop.
The function creates a thread that runs the start_routine function and then 
afterwards begins to start its own for loop that then continues until both 
for loops are complete, after which the program ends. The return value for
this function is only necessary because it is the main function. */
int main(int argc, char *argv[]) 
{
    int i, n = atoi(argv[1]); // n microseconds to input from keyboard for delay
    printf("\n Before threads.\n");
    pthread_t tid;
    pthread_create(&tid, NULL, start_routine, &n);
    // Child process
    for (i=0;i<100;i++) {
        printf("Child process %d\n",i);
        usleep(n);
    }
    pthread_join(tid,NULL);
    return 0;
}