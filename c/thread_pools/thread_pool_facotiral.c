#include <windows.h>
#include <stdio.h>

typedef struct {
    int input;               // Number to compute factorial for
    ULONGLONG* result;       // Where to store the result
    LONG* remaining_tasks;   // Counter for completed tasks
    HANDLE completion_event; // Event to signal when all tasks complete
} FactorialWorkParams;

// Thread pool work callback function
VOID CALLBACK FactorialWorkCallback(
    PTP_CALLBACK_INSTANCE Instance,
    PVOID Context,
    PTP_WORK Work
) {
    FactorialWorkParams* params = (FactorialWorkParams*)Context;
    
    // Compute factorial
    ULONGLONG factorial = 1;
    for(int i = 2; i <= params->input; i++) {
        factorial *= i;
    }
    
    // Store result
    *(params->result) = factorial;
    
    // Atomically decrement remaining task counter
    LONG remaining = InterlockedDecrement(params->remaining_tasks);
    
    // If last task, signal completion event
    if(remaining == 0) {
        SetEvent(params->completion_event);
    }
    
    // Cleanup
    free(params);
    CloseThreadpoolWork(Work);
}

int main() {
    // Input numbers and results array
    int numbers[] = {5, 6, 7, 8, 9, 10, 12, 15};
    const int num_tasks = sizeof(numbers)/sizeof(numbers[0]);
    ULONGLONG results[num_tasks];
    
    // Task tracking
    LONG remaining_tasks = num_tasks;
    HANDLE completion_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    
    // Submit work items to thread pool
    for(int i = 0; i < num_tasks; i++) {
        // Allocate parameters for this work item
        FactorialWorkParams* params = (FactorialWorkParams*)malloc(sizeof(FactorialWorkParams));
        params->input = numbers[i];
        params->result = &results[i];
        params->remaining_tasks = &remaining_tasks;
        params->completion_event = completion_event;
        
        // Create and submit work item
        PTP_WORK work = CreateThreadpoolWork(FactorialWorkCallback, params, NULL);
        SubmitThreadpoolWork(work);
    }
    
    // Wait for all tasks to complete
    WaitForSingleObject(completion_event, INFINITE);
    
    // Print results
    printf("Factorial Results:\n");
    for(int i = 0; i < num_tasks; i++) {
        printf("%2d! = %20llu\n", numbers[i], results[i]);
    }
    
    // Cleanup
    CloseHandle(completion_event);
    return 0;
}