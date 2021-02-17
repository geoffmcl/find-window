
#pragma once

extern BOOL EnumProcessList (VOID);
extern VOID Kill_Process_List(VOID);
extern void Kill_ALL_Process_List(void);

typedef struct tagMODULELIST {
    void *        next; // list NEXT, if any
    HWND          hwnd; // Window handle
    MODULEENTRY32 s_me32;   // info
}MODULELIST, * PMODULELIST;

typedef struct tagTHREADLIST {
    void *        next; // list NEXT, if any
    HWND          hwnd; // Window handle
    THREADENTRY32  s_te32;  // info
}THREADLIST, * PTHREADLIST;

typedef struct tagALLPROCESSLIST {
    void *         next;
    DWORD          dwPriorityClass; 
    BOOL           bGotModule;
    HWND           hwnd;    // window handle
    PROCESSENTRY32 pe32;
    void *         pme32;   // Module list (PMODULELIST)
    void *         pth32;   // Thread list (PTHREADLIST)
}ALLPROCESSLIST, * PALLPROCESSLIST;

extern PALLPROCESSLIST FindProcID( HWND hwnd, DWORD dwProcID, DWORD dwThreadID );

// eof - find_Enum.h

