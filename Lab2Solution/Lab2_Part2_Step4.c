/*
# Name: Jordan Murtiff
# Date: 4/7/20
# Title: Lab2 Part 2 Step #4 - Computing time elapsed for differing connections 
# Description: Computing total RTT times given a number of parallel connections, additional objects,
# and both persistent and non persistent HTTP.
*/

#include <stdio.h>      /* printf */
#include <stdlib.h>
#include <math.h>       /* ceil */

/* The main function of the program, which computes the amount of time it takes 
for a client to receive a number of HTML objects after clicking on a given link.
The main function asks the user for the number of parallel TCP connections to use
for the final two calculations, since the lab description only specified N connections.  */
int main()
{
    //Setting all the variables to the given values in the Lab #2 document
    int RTT_0 = 3;
    int RTT_1 = 20;
    int RTT_2 = 26;
    int RTT_HTTP = 47;
    int parallel_connections;

    //Asking the user for the number of parallel connections to use for the final two calculations.
    printf("Enter number of parallel connections: ");
    scanf("%d", &parallel_connections);
    
    //Calculating and printing the time elapsed for the client to receive one HTML object after clicking a link.
    int temp = RTT_0 + RTT_1 + RTT_2 + (2*RTT_HTTP);
    printf("Time elapsed: %d ms\n", temp);
    
    //Calculating and printing the time elapsed for the client to receive six HTML objects after clicking a link.
    temp = RTT_0 + RTT_1 + RTT_2 + (6*2*RTT_HTTP);
    printf("Time elapsed 6 additional objects: %d ms\n", temp);

    /* Calculating and printing the time elapsed for the client to receive six HTML objects after clicking a link
    using a given number of parallel TCP connections (given a non-persistent HTTP) */
    printf("Non-persistent case: ");
    double temporary = 6.0/parallel_connections;
    temporary = ceil(temporary);
    temp = RTT_0 + RTT_1 + RTT_2 + (2*RTT_HTTP*temporary);
    printf("%d ms\n", temp);    

    /* Calculating and printing the time elapsed for the client to receive six HTML objects after clicking a link
    using a given number of parallel TCP connections (given a persistent HTTP) */
    printf("Persistent case: ");
    temp = RTT_0 + RTT_1 + RTT_2 + (RTT_HTTP*temporary) + (2*RTT_HTTP);
    printf("%d ms\n", temp); 

}
