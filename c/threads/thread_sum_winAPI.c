#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


int Sum = 0;

// Thread function that performs the summation
DWORD WINAPI Summation(LPVOID param) {
    int num = *((int*)param);  
    Sum = num * (num + 1) / 2;  
    return 0;
}

int main(int argc, char *argv[]) {
    DWORD Threadld; 
    HANDLE ThreadHandle; 
    int Param; 

    if (argc != 2) {
        printf("Usage: %s <number>\n", argv[0]);
        return 1;
    }

    Param = atoi(argv[1]);  

    // Create the thread to calculate the summation
    ThreadHandle = CreateThread(
        NULL,                // default security attributes
        0,                   // default stack size
        Summation,           // thread function
        &Param,              // parameter to thread function
        0,                   // default creation flags
        &Threadld);          // returns the thread identifier

    if (ThreadHandle == NULL) {
        printf("Thread creation failed. Error code: %lu\n", GetLastError());
        return 1;
    }

   
    WaitForSingleObject(ThreadHandle, INFINITE);

 
    CloseHandle(ThreadHandle);

    
    printf("sum = %d\n", Sum);

    return 0;
}
