// Name: Jordan Murtiff
// Date: 3/31/20
// Title: Lab 1 Steps 6 and 7 - Unix/Linux Commands and Overview of C Programming
// Description: This program performs computations for steps 6 and 7 of Lab #1. 
// The computation relates to both Circuit Switching as well as a Packet Switching scenarios 
// and how they differ from one another.

//Necessary Libraries
#include <stdio.h>      /* printf, stderr */
#include <stdlib.h>     /* atoi */
#include <math.h>		/*division, multiplication, and power function*/


/* Function to calculate the factorial of a given number, used in
part g of part #6 of the lab. Takes in a double and returns a double. */
double factorial(double num)
{
    double i, result;
    result = 1;
    for(i=1; i <= num; i++)
    {
        result = (result * i);
    }
    return result;
}

/* Binomial distribution function used in parts g and h of part 6 of the lab.
Takes in two doubles and returns a double. This function uses the same formula
as the one learned in AMTH 108. */
double binomial(double x, double y)
{
    double r = (factorial(x)/((factorial(y)* factorial((x-y)))));
    return r;
}



/* The main function of the program. This function makes quantitative comparisons
between circuit switching and packet switching according to the given variables 
listed in the Lab #1 handout. A circuit-switching scenario is one in which "nCSusers" 
users, each requiring a bandwidth of "userBandwidth" Mbps, must share a link of capacity 
"linkBandwidth" Mbps. On the other hand, a packet-switching scenario is one in which 
"nPSusers" users share a "linkBandwidth" Mbps link, where each user again requires 
"userBandwidth" Mbps when transmitting, but only needs to transmit at a percentage of 
"tPSuser". Each and every given calculated quantity (except those given 
strictly by the lab handout) is printed out to the terminal. */
int main(int argc, char *argv[]) 
{
    //The bandwidth of a network link.
    double linkBandwidth = 200;
	
	//The bandwidth required for a given user.
    double userBandwidth = 20;
	
	//The percentage of time a packet switching user needs to transmit.
    double tPSuser  = 0.10;
	
	//The number of packet switching users.
    double nPSusers = 19;
	
	//The probability that a given (specific) packet switching user is busy transmitting.
	double pPSusersBusy;

    //Step 6 - Circuit switching scenario
	//The number of circuit switching users that can be supported is computed as follows:
    double nCSusers = (linkBandwidth / userBandwidth);
    printf("\t6a: %lf \n", nCSusers);

    //Step 7 Part a
	//The probability that a given (specific) user is busy transmitting is computed as:
    double pPSusers = tPSuser;
    printf("\t7a: %lf \n", pPSusers);

    //Step 7 Part b
	//The probability that one specific other user is not busy is computed as:
    double pPSusersNotBusy = 1 - pPSusers;
    printf("\tb: %lf \n", pPSusersNotBusy);

    //Step 7 Part c
    //The probability that all of the other specific other users are not busy is computed as:
    double all = pow((1-pPSusers),(nPSusers-1));
    printf("\tc: %e \n", all);

    //Step 7 Part d
    //The probability that one specific user is transmitting and the remaining users are not transmitting is computed as:
    double notTransmit = pPSusers * pow(pPSusersNotBusy,(nPSusers-1));
    printf("\td: %e \n", notTransmit);

    //Step 7 Part e
    //The probability that exactly one (any one) of the nPSusers users is busy is (pPSusers) * the probability that a given specific 
    //user is transmitting and the remaining users are not transmitting:
    double oneBusy = nPSusers * notTransmit;
    printf("\te: %e \n", oneBusy);

    //Step 7 Part f
    //The probability that 10 specific users of nPSusers are transmitting and the others are idle is: 
    double anyTen = pow(pPSusers,10) * pow(pPSusersNotBusy,(nPSusers-10));
    printf("\tf: %e \n", anyTen);

    //Step 7 Part g
    //The probability that any 10 users of nPSusers are transmitting and the others are idle is computed as:
    double y = binomial(nPSusers, 10);
    double tenTransmit = y * (pow(pPSusers,10))* pow(pPSusersNotBusy,(nPSusers-10));
    printf("\tg: %e \n", tenTransmit);

    //Step 7 Part h
    //The probability that more than 10 users of nPSusers are transmitting and the others are idle is computed as:
    double i,sum;
    double g = nPSusers;
    for (i = 11;  i <= nPSusers; i++){
        sum += (binomial(nPSusers, i) * pow(pPSusers,i) * pow(pPSusersNotBusy,(nPSusers-i)));
    }
    printf("\th: %e \n", sum);
    return 0;
}