// SetThreadNameTest.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <process.h>

// test options
#define DEFAULT_NAMING 0
#define CREATE_THREAD_NAMING 1
#define CUSTOM_NAMING 2

#define TEST_CASE DEFAULT_NAMING

//////////////////////////////////////////////////////////////////////////
// SetThreadName implementation

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(DWORD dwThreadID, const char* threadName)
{
#if TEST_CASE != CUSTOM_NAMING
    return;
#endif
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;

    __try
    {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }
}

//////////////////////////////////////////////////////////////////////////
// test code



static const char thread_names[][80] =
{
    "Main thread",
    "Worker thread",
    "Delivery thread"
};

volatile LONG thread_index = 0;
HANDLE stop_event = 0;

unsigned __stdcall TestThread(void *context)
{
    LONG current_thread_index = InterlockedIncrement(&thread_index);
    SetThreadName(-1, thread_names[current_thread_index]);

    WaitForSingleObject(stop_event, INFINITE);
    return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    stop_event = CreateEvent(NULL, TRUE, FALSE, NULL);

    SetThreadName(-1, thread_names[0]);

    for (int i = 1; i < _countof(thread_names); i++)
    {
#if TEST_CASE != CREATE_THREAD_NAMING
        _beginthreadex(NULL, 0, TestThread, NULL, 0, NULL);
#else
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TestThread, NULL, 0, NULL);
#endif
    }

    Sleep(1000);

    DebugBreak();

    return 0;
}

