#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5

void *printFactorial(void *param) {
    int num = *((int *)param);
    int result = 1;

    for (int i = 1; i <= num; i++) {
        result *= i;
    }

    printf("Thread for %d: Factorial = %d\n", num, result);
    free(param); 
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        int *val = malloc(sizeof(int)); 
        *val = i + 1;

        pthread_create(&threads[i], NULL, printFactorial, val);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads finished.\n");
    return 0;
}
