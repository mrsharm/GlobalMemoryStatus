// GlobalMemoryStatus.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <tchar.h>

// Assumptions: 
// 1. 64 bit.
// 2. With no parameters => display the counter ever 1 second.

VOID win_timer_callback(
    HWND hwnd,        // handle to window for timer messages 
    UINT message,     // WM_TIMER message 
    UINT idTimer,     // timer identifier 
    DWORD dwTime)     // current system time 
{
    MEMORYSTATUSEX memStatus; 
	memStatus.dwLength = sizeof(memStatus);
    BOOL fRet = GlobalMemoryStatusEx(&memStatus);
    if (fRet)
    {
		std::cout << "Memory Load: " << memStatus.dwMemoryLoad << "%.\n";
    }
}

int run_win_timer(int seconds_to_run)
{
    UINT id;
    MSG msg;
    bool bStillBusy = true;

	UINT_PTR timerId = 
        ::SetTimer(NULL,                         // Handle to Window - this is null. 
				 0,                              // timer identifier 
				 seconds_to_run * 1000,          // 1-second interval 
				 (TIMERPROC)win_timer_callback); // Timer callback 
    if (timerId == 0)
    {
        std::cerr << "Failed to create timer." << std::endl;
        return 1;
    }

    while (bStillBusy)
    {
        GetMessage(&msg, NULL, 0, 0);
        DispatchMessage(&msg);
    }

    KillTimer(NULL, 0);
    return 0;
}

void thread_timer_callback(int seconds_to_run)
{
    while (true) 
    {
        std::this_thread::sleep_for(std::chrono::seconds(seconds_to_run));
        MEMORYSTATUSEX memStatus; 
        memStatus.dwLength = sizeof(memStatus);
		BOOL fRet = GlobalMemoryStatusEx(&memStatus);
        if (fRet)
        {
            std::cout << "Memory Load: " << memStatus.dwMemoryLoad << "%.\n";
        }
    }
}

int run_thread_timer(int seconds)
{
    std::thread timerThread(thread_timer_callback, seconds);
    std::cin.get(); // Wait for user to press Enter

    // Stop the timer thread
    timerThread.detach();
    return 0;
}

int main(int argc, char** argv)
{
    int seconds_to_run = 1;
    if (argc > 1)
    {
        seconds_to_run = std::atoi(argv[1]);
        if (seconds_to_run <= 0)
        {
            std::cout << "Invalid number of seconds to run: " << argv[1] << ".\n";
			return -1;
        }
    }

    std::cout << "Running Memory Load Timer every " << seconds_to_run << " second(s). To quit: Press Ctrl+C. \n";
    return run_thread_timer(seconds_to_run);
}
