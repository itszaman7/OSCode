#include <windows.h>
#include <stdio.h>
#include <stdbool.h>  // Required for bool type in C

typedef struct {
    int input;
    bool* result;
    LONG* remaining_tasks;
    HANDLE completion_event;
} OddWorkParams;

VOID CALLBACK OddNumberCallback(
    PTP_CALLBACK_INSTANCE instance,
    PVOID Context,
    PTP_WORK Work
) {
    OddWorkParams* params = (OddWorkParams*)Context;

    // Check if the number is odd
    bool isOdd = (params->input % 2 != 0);
    
    // Store the result
    *(params->result) = isOdd;
    
    // Update remaining tasks counter
    LONG remaining = InterlockedDecrement(params->remaining_tasks);
    
    // Signal completion when all tasks are done
    if (remaining == 0) {
        SetEvent(params->completion_event);
    }
    
    // Cleanup
    free(params);
    CloseThreadpoolWork(Work);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <number1> <number2> ...\n", argv[0]);
        return 1;
    }

    const int num_tasks = argc - 1;
    int* numbers = (int*)malloc(num_tasks * sizeof(int));
    bool* results = (bool*)malloc(num_tasks * sizeof(bool));
    LONG remaining_tasks = num_tasks;
    
    // Create completion event
    HANDLE completion_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    // Parse input numbers
    for (int i = 0; i < num_tasks; i++) {
        numbers[i] = atoi(argv[i + 1]);
    }

    // Submit work items to thread pool
    for (int i = 0; i < num_tasks; i++) {
        OddWorkParams* params = (OddWorkParams*)malloc(sizeof(OddWorkParams));
        params->input = numbers[i];
        params->result = &results[i];
        params->remaining_tasks = &remaining_tasks;
        params->completion_event = completion_event;
        
        PTP_WORK work = CreateThreadpoolWork(OddNumberCallback, params, NULL);
        SubmitThreadpoolWork(work);
    }

    // Wait for all tasks to complete
    WaitForSingleObject(completion_event, INFINITE);

    // Print results
    printf("Number parity check results:\n");
    for (int i = 0; i < num_tasks; i++) {
        printf("%d is %s\n", numbers[i], results[i] ? "odd" : "even");
    }

    // Cleanup
    free(numbers);
    free(results);
    CloseHandle(completion_event);

    return 0;
}