#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>

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
    MSG msg;
    UINT timer_id = 0;
    bool bStillBusy = true;

    UINT_PTR timer_ptr = SetTimer(/* Window Handle */ NULL, 
                                  /* Timer ID*/ timer_id, 
                                  /* Elasped Time in MSec */ seconds_to_run * 1000, 
                                  /* Callback */ (TIMERPROC)win_timer_callback);
    if (timer_ptr == NULL)
    {
        std::cerr << "Failed to create timer." << std::endl;
        return 1;
    }

    // Setup message pump to dispatch messages else the callback won't fire.
    while (bStillBusy)
    {
        GetMessage(&msg, NULL, 0, 0);
        DispatchMessage(&msg);
    }

    if (!KillTimer(NULL, 0))
    {
        std::cout << "Failed to kill the timer!\n";
        return -1;
    }

    return 0;
}

BOOL WINAPI consoleHandler(DWORD signal)
{
	// Gracefully exit if the user Ctrl-Cs.
    if (signal == CTRL_C_EVENT)
    {
        exit(0);
    }

    // Don't handle any other signals.
    return TRUE;
}

int run_thread_timer(int seconds_to_run)
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

    // Stop the timer thread
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

    if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
    {
        std::cerr << "ERROR: Could not set control handler\n";
        return 1;
    }

    std::cout << "Running Memory Load Timer every " << seconds_to_run << " second(s). To quit: Press Ctrl+C. \n";
    return run_thread_timer(seconds_to_run);
}
