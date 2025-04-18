#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

int n;
bool o;

void *factorial(void *param);
void *odd(void *param);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <positive-integer>\n", argv[0]);
        return -1;
    }

    pthread_t tid, tidO;
    pthread_attr_t attr, attrO;

    pthread_attr_init(&attr);
    pthread_attr_init(&attrO);

    pthread_create(&tid, &attr, factorial, argv[1]);
    pthread_create(&tidO, &attrO, odd, argv[1]);

    pthread_join(tid, NULL); 
    pthread_join(tidO, NULL);

    printf("Factorial = %d\n", n);
    printf("Is the number odd: %s\n", o ? "Yes" : "No");
    return 0;
}

void *factorial(void *param) {
    int fac = atoi(param); 
    int c;

    n = 1;
    for (c = 1; c <= fac; c++)
        n = n * c;

    printf("Thread: Calculated factorial of %d = %d\n", fac, n);
    pthread_exit(0);
}

void *odd(void *param){
    int num = atoi(param);
    o = (num % 2 != 0);
    
    printf("Thread: Checked odd, result = %s\n", o ? "Yes" : "No");
    pthread_exit(0);
}
