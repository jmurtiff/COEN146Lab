#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(){
    int RTT_0 = 3;
    int RTT_1 = 20;
    int RTT_2 = 26;
    int RTT_HTTP = 47;
    int parallel_connections;

    printf("Enter number of parallel connections: ");
    scanf("%d", &parallel_connections);
    
    int temp = RTT_0 + RTT_1 + RTT_2 + (2*RTT_HTTP);
    printf("Time elapsed: %d ms\n", temp);
    
    temp = RTT_0 + RTT_1 + RTT_2 + (6*2*RTT_HTTP);
    printf("Time elapsed 6 additional objects: %d ms\n", temp);

    printf("Non-persistent case: ");
    double temporary = 6.0/parallel_connections;
    temporary = ceil(temporary);
    temp = RTT_0 + RTT_1 + RTT_2 + (2*RTT_HTTP*temporary);
    printf("%d ms\n", temp);    

    printf("Persistent case: ");
    temp = RTT_0 + RTT_1 + RTT_2 + (RTT_HTTP*temporary) + (2*RTT_HTTP);
    printf("%d ms\n", temp); 

}
