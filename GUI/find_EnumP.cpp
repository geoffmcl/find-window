
// find_EnumP.cpp
#include "find_window.h"
#include <psapi.h>
#include <tlhelp32.h>   // for PROCESSENTRY32, ...
#include <vdmdbg.h>

#ifndef VFH
#define VFH(a)  ( a && ( a != INVALID_HANDLE_VALUE ) )
#endif

#undef PRINTF_PROCESS_INFO
#define EP_PRINTF   sprtf

typedef struct tagMYPROCESSLIST {
    void *         next;
    DWORD          dwPriorityClass; 
    BOOL           bGotModule; 
    MODULEENTRY32  me32;
    PROCESSENTRY32 pe32;
}MYPROCESSLIST, * PMYPROCESSLIST;

PMYPROCESSLIST pProcessList = NULL;

VOID Add_to_Process_List( PMYPROCESSLIST ppl )
{
    PMYPROCESSLIST pn = pProcessList;
    if (pn == NULL)
        pProcessList = ppl;
    else
    {
        void * vp = pn->next;
        while (vp) {
            pn = (PMYPROCESSLIST)vp;
            vp = pn->next;
        }
        pn->next = ppl;
    }
}

VOID Kill_Process_List(VOID)
{
    PMYPROCESSLIST pn = pProcessList;
    while (pn) {
        void * vp = pn->next;   // get NEXT (if any)
        delete pn;              // delete current
        pn = (PMYPROCESSLIST)vp;// setup next
    }
    pProcessList = NULL;
}

/* ==========================================
typedef struct tagMODULELIST {
    void *          next;
    MODULEENTRY32   me32;
}MODULELIST, * PMODULELIST;

typedef struct tagTHREADLIST {
    void *          next;
    THREADENTRY32   te32;
}THREADLIST, * PTHREADLIST;

typedef struct tagALLPROCESSLIST {
    void *         next;
    DWORD          dwPriorityClass; 
    BOOL           bGotModule; 
    PROCESSENTRY32 pe32;
    void *         pme32;   // Module list (PMODULELIST)
    void *         pth32;   // Thread list
}ALLPROCESSLIST, * PALLPROCESSLIST;
  ======================================= */

PALLPROCESSLIST pAllProcessList = NULL;

// The GetWindowThreadProcessId function retrieves the identifier of 
// the thread that created the specified window and, optionally, the 
// identifier of the process that created the window. 
PALLPROCESSLIST FindProcID( HWND hwnd, DWORD dwProcID, DWORD dwThreadID )
{
    PALLPROCESSLIST pn = pAllProcessList;
    void * vp;
    PTHREADLIST ptl;
    // sprtf("Search for Proc ID %08X, Thread ID %08X...\n", dwProcID, dwThreadID );
    if ( pn )
    {
        if ( pn->pe32.th32ProcessID == dwProcID )
            return pn;
        // search the THREAD ID for this process
        ptl = (PTHREADLIST)pn->pth32;
        if (ptl)
        {
            if ( ptl->s_te32.th32ThreadID == dwThreadID )
                return pn;
            vp = ptl->next;
            while (vp)
            {
                ptl = (PTHREADLIST)vp;
                if ( ptl->s_te32.th32ThreadID == dwThreadID )
                    return pn;
                vp = ptl->next;
            }
        }

        vp = pn->next;
        while (vp)
        {
            pn = (PALLPROCESSLIST)vp;
            if ( pn->pe32.th32ProcessID == dwProcID ) {
                if ( pn->hwnd == NULL )
                    pn->hwnd = hwnd;    // set FIRST with this Window handle
                // search the THREAD ID for this process
                ptl = (PTHREADLIST)pn->pth32;
                while (ptl)
                {
                    if ( ptl->s_te32.th32ThreadID == dwThreadID ) {
                        if ( ptl->hwnd == NULL )
                            ptl->hwnd = hwnd;
                        break;
                    }
                    ptl = (PTHREADLIST)ptl->next;
                }
                return pn;
            }
            // search the THREAD ID for this process
            ptl = (PTHREADLIST)pn->pth32;
            if (ptl)
            {
                if ( ptl->s_te32.th32ThreadID == dwThreadID )
                    return pn;
                vp = ptl->next;
                while (vp)
                {
                    ptl = (PTHREADLIST)vp;
                    if ( ptl->s_te32.th32ThreadID == dwThreadID )
                        return pn;
                    vp = ptl->next;
                }
            }
            vp = pn->next;
        }
        pn = (PALLPROCESSLIST)vp;
    }
    return pn;
}

int Add_to_ALL_Process_List( PALLPROCESSLIST papl )
{
    PALLPROCESSLIST pn = pAllProcessList;
    if ( pn == NULL )
        pAllProcessList = papl;
    else
    {
        void * vp = pn->next;
        while (vp) {
            pn = (PALLPROCESSLIST)vp;
            vp = pn->next;
        }
        pn->next = papl;
    }
    return 1;
}

int Add_to_Thread_List( PALLPROCESSLIST papl, PTHREADLIST ptl )
{
    if ( papl == NULL )
        return 0;
    else
    {
        PTHREADLIST pn = (PTHREADLIST)papl->pth32;
        if ( pn == NULL )
            papl->pth32 = ptl;
        else
        {
            void * vp = pn->next;
            while (vp) {
                pn = (PTHREADLIST)vp;
                vp = pn->next;
            }
            pn->next = ptl;
        }
    }
    return 1;
}

int Add_to_Module_List( PALLPROCESSLIST papl, PMODULELIST pml )
{
    if ( papl == NULL )
        return 0;
    else
    {
        PMODULELIST pn = (PMODULELIST)papl->pme32;
        if ( pn == NULL )
            papl->pme32 = pml;
        else
        {
            void * vp = pn->next;
            while (vp) {
                pn = (PMODULELIST)vp;
                vp = pn->next;
            }
            pn->next = pml;
        }
    }
    return 1;
}

void Kill_ml( void * vp )
{
    PMODULELIST pn = (PMODULELIST)vp;
    while (pn)
    {
        vp = pn->next;
        delete pn;
        pn = (PMODULELIST)vp;
    }
}
void Kill_tl( void * vp )
{
    PTHREADLIST pn = (PTHREADLIST)vp;
    while (pn)
    {
        vp = pn->next;
        delete pn;
        pn = (PTHREADLIST)vp;
    }
}

void Kill_ALL_Process_List(void)
{
    PALLPROCESSLIST pn = pAllProcessList;
    while (pn)
    {
        void * vp = pn->next;
        Kill_ml( pn->pme32 );
        Kill_tl( pn->pth32 );
        delete pn;
        pn = (PALLPROCESSLIST)vp;
    }
    pAllProcessList = NULL;
}

// forward reference
void Enum_Process_List(void);

// ======================================================
//#include <windows.h>
//#include <tlhelp32.h>
//#include <stdio.h>

// priority classes
typedef struct tagBIT2STG {
    UINT    bit;
    PTSTR   pdesc;
    PTSTR   pinfo;
}BIT2STG, * PBIT2STG;

// Return code/value Description 
BIT2STG sPriority_Classes[] = {
    { ABOVE_NORMAL_PRIORITY_CLASS, "ABOVE_NORMAL", "0x00008000 Process that has priority above NORMAL_PRIORITY_CLASS but below HIGH_PRIORITY_CLASS. Windows NT and Windows Me/98/95:  This value is not supported." },
    { BELOW_NORMAL_PRIORITY_CLASS, "BELOW_NORMAL", "0x00004000 Process that has priority above IDLE_PRIORITY_CLASS but below NORMAL_PRIORITY_CLASS. Windows NT and Windows Me/98/95:  This value is not supported." },
    { HIGH_PRIORITY_CLASS, "HIGH_PRIORITY", "0x00000080 Process that performs time-critical tasks that must be executed immediately for it to run correctly. The threads of a high-priority class process preempt the threads of normal or idle priority class processes. An example is the Task List, which must respond quickly when called by the user, regardless of the load on the operating system. Use extreme care when using the high-priority class, because a high-priority class CPU-bound application can use nearly all available cycles." },
    { IDLE_PRIORITY_CLASS, "IDLE_PRIORITY", "0x00000040 Process whose threads run only when the system is idle and are preempted by the threads of any process running in a higher priority class. An example is a screen saver. The idle priority class is inherited by child processes." },
    { NORMAL_PRIORITY_CLASS, "NORMAL_PRIORITY", "0x00000020 Process with no special scheduling needs." },
    { REALTIME_PRIORITY_CLASS, "REALTIME_PRIORITY", "0x00000100 Process that has the highest possible priority. The threads of a real-time priority class process preempt the threads of all other processes, including operating system processes performing important tasks. For example, a real-time process that executes for more than a very brief interval can cause disk caches not to flush or cause the mouse to be unresponsive." },
    { 0, NULL, NULL }
};
     
//  Forward declarations:
BOOL GetProcessList(void);
BOOL ListProcessModules( DWORD dwPID, PALLPROCESSLIST papl );
BOOL ListProcessThreads( DWORD dwOwnerPID, PALLPROCESSLIST papl );
void printError( TCHAR* msg );

PTSTR   Get_Priority_Class_Stg( DWORD pri )
{
    PBIT2STG pb = &sPriority_Classes[0];
    // 1. try absolute compare
    while ( pb->pdesc != NULL )
    {
        if ( pb->bit == pri )
            return pb->pdesc;
        pb++;
    }
    // 2. try bit
    pb = &sPriority_Classes[0];
    while ( pb->pdesc != NULL )
    {
        if ( pb->bit & pri )
            return pb->pdesc;
        pb++;
    }
    PTSTR pstg = GetNxtBuf();
    StringCchPrintf(pstg, MX_ONE_BUF, "%#X no in list!", pri);
    return pstg;
}

//void Enum_Process_List(void)
BOOL EnumProcessList (VOID) 
{
    BOOL bRet = GetProcessList();
    EP_PRINTF("\nDone process list - %s\n",
        (bRet ? "ok" : "FAILED") );
    return bRet;
}

BOOL GetProcessList(void)
{
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;
    DWORD dwAccessRights = PROCESS_ALL_ACCESS; // (0x1F0FFF) All possible access rights for a process object. 

    hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( hProcessSnap == INVALID_HANDLE_VALUE )
    {
        printError( "CreateToolhelp32Snapshot (of processes)" );
        return( FALSE );
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if( !Process32First( hProcessSnap, &pe32 ) )
    {
        printError( "Process32First" );  // Show cause of failure
        CloseHandle( hProcessSnap );     // Must clean up the snapshot object!
        return( FALSE );
    }

    dwAccessRights = PROCESS_QUERY_INFORMATION; // (0x0400) Required to retrieve 
    // certain information about a process, such as its token, exit code, 
    // and priority class (see OpenProcessToken, GetExitCodeProcess, GetPriorityClass,
    // and IsProcessInJob). 

    // Take a snapshot of all processes in the system.
    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do
    {
        PALLPROCESSLIST papl = new ALLPROCESSLIST;
        ZeroMemory(papl, sizeof(ALLPROCESSLIST));
        CopyMemory( &papl->pe32, &pe32, sizeof(PROCESSENTRY32) );
        
        EP_PRINTF( "\n\n=====================================================" );
        EP_PRINTF( "\nPROCESS NAME:  %s (PID=%08X)", pe32.szExeFile, pe32.th32ProcessID );
        EP_PRINTF( "\n-----------------------------------------------------" );
        // Retrieve the priority class.
        dwPriorityClass = 0;
        hProcess = OpenProcess( dwAccessRights, FALSE, pe32.th32ProcessID );
        if( hProcess == NULL )
            printError( "OpenProcess" );

        else
        {
            dwPriorityClass = GetPriorityClass( hProcess );
            if( dwPriorityClass )
                papl->bGotModule = TRUE;
            else
                printError( "GetPriorityClass" );
            CloseHandle( hProcess );
        }
        papl->dwPriorityClass = dwPriorityClass;

        //EP_PRINTF( "\n  process ID        = 0x%08X", pe32.th32ProcessID );
        EP_PRINTF( "\n  thread count      = %d",   pe32.cntThreads );
        EP_PRINTF( "\n  parent process ID = 0x%08X", pe32.th32ParentProcessID );
        EP_PRINTF( "\n  Priority Base     = %d", pe32.pcPriClassBase );
        if( dwPriorityClass )
            EP_PRINTF( "\n  Priority Class    = %d (%s)", dwPriorityClass, Get_Priority_Class_Stg(dwPriorityClass) );

        // List the modules and threads associated with this process

        EP_PRINTF( "\n  Process Modules..." );
        ListProcessModules( pe32.th32ProcessID, papl );
        EP_PRINTF( "\n  Process Threads..." );
        ListProcessThreads( pe32.th32ProcessID, papl );

        Add_to_ALL_Process_List( papl );    // ADD TO LIST
        
    } while( Process32Next( hProcessSnap, &pe32 ) );

    CloseHandle( hProcessSnap );

    return( TRUE );

}

BOOL ListProcessModules( DWORD dwPID, PALLPROCESSLIST papl )
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
    if(( hModuleSnap == NULL )||( hModuleSnap == INVALID_HANDLE_VALUE ))
    {
        printError( "CreateToolhelp32Snapshot (of modules)" );
        return( FALSE );
    }


    // Set the size of the structure before using it.
    me32.dwSize = sizeof( MODULEENTRY32 );
    // Retrieve information about the first module,
    // and exit if unsuccessful

    if( !Module32First( hModuleSnap, &me32 ) )
    {
        printError( "Module32First" );  // Show cause of failure
        CloseHandle( hModuleSnap );     // Must clean up the snapshot object!
        return( FALSE );
    }

    // Now walk the module list of the process,
    // and display information about each module
    do
    {
        PMODULELIST pml = new MODULELIST;
        ZeroMemory( pml, sizeof(MODULELIST) );
        CopyMemory( &pml->s_me32, &me32, sizeof(MODULEENTRY32) );
        Add_to_Module_List( papl, pml );
        EP_PRINTF( "\n\n     MODULE NAME:     %s",           me32.szModule );
        EP_PRINTF( "\n     executable     = %s",             me32.szExePath );
        EP_PRINTF( "\n     process ID     = 0x%08X",         me32.th32ProcessID );
        EP_PRINTF( "\n     ref count (g)  =     0x%04X",     me32.GlblcntUsage );
        EP_PRINTF( "\n     ref count (p)  =     0x%04X",     me32.ProccntUsage );
        EP_PRINTF( "\n     base address   = 0x%08X",         me32.modBaseAddr );
        EP_PRINTF( "\n     base size      = %d",             me32.modBaseSize );
        
    } while( Module32Next( hModuleSnap, &me32 ) );


    CloseHandle( hModuleSnap );

    return( TRUE );
}

BOOL ListProcessThreads( DWORD dwOwnerPID, PALLPROCESSLIST papl ) 
{ 
    HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
    THREADENTRY32 te32; 

    // Take a snapshot of all running threads
    hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 

    if(( hThreadSnap == NULL )||( hThreadSnap == INVALID_HANDLE_VALUE ))
    {
        printError( "CreateToolhelp32Snapshot (of threads)" );
        return( FALSE ); 
    }
 
    // Fill in the size of the structure before using it. 
    te32.dwSize = sizeof(THREADENTRY32 );

    // Retrieve information about the first thread,
    // and exit if unsuccessful
    if( !Thread32First( hThreadSnap, &te32 ) ) 
    {
        printError( "Thread32First" );  // Show cause of failure
        CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
        return( FALSE );
    }

    // Now walk the thread list of the system,
    // and display information about each thread
    // associated with the specified process
    do 
    {
        if( te32.th32OwnerProcessID == dwOwnerPID )
        {
            PTHREADLIST ptl = new THREADLIST;
            ZeroMemory( ptl, sizeof(THREADLIST) );
            CopyMemory( &ptl->s_te32, &te32, sizeof(THREADENTRY32) );
            Add_to_Thread_List( papl, ptl );

            EP_PRINTF( "\n\n     THREAD ID      = 0x%08X", te32.th32ThreadID );
            EP_PRINTF( "\n     base priority  = %d", te32.tpBasePri );
            EP_PRINTF( "\n     delta priority = %d", te32.tpDeltaPri );
        }
    } while( Thread32Next(hThreadSnap, &te32 ) ); 

    CloseHandle( hThreadSnap );

    return( TRUE );

}

void printError( TCHAR* msg )
{
    DWORD eNum;
    TCHAR sysMsg[512];
    TCHAR* p;

    eNum = GetLastError( );

    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, eNum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        sysMsg, 512, NULL );

    // Trim the end of the line and terminate it with a null
    p = sysMsg;
    while( ( *p > 31 ) || ( *p == 9 ) )
        ++p;    // get to end

    do
    {
        *p-- = 0;
    } while( ( p >= sysMsg ) && (( *p == '.' ) || ( *p < 33 )) );

    // Display the message
    EP_PRINTF( "\n  WARNING: %s failed with error %d (%s)", msg, eNum, sysMsg );

}

//  ======================================================

// OTHER CRUMBS NOT USED, and NOT all working (in XP at least)

///*********************
//   EnumProc.h
//   *********************/ 
//   #include <windows.h>

typedef BOOL (CALLBACK *PROCENUMPROC)( DWORD, LPVOID, LPSTR, LPARAM ) ;

//   BOOL WINAPI EnumProcs( PROCENUMPROC lpProc, LPARAM lParam ) ;
//

   /*********************
   EnumProc.c (or .cpp)
   *********************/ 
//   #include "EnumProc.h"
//   #include <tlhelp32.h>
//   #include <vdmdbg.h>

typedef struct tagEnumInfoStruct {
     DWORD          dwPID ;
     PROCENUMPROC   lpProc ;
     DWORD          lParam ;
     BOOL           bEnd ;
} EnumInfoStruct;


   BOOL WINAPI Enum16( DWORD dwThreadId, WORD hMod16, WORD hTask16,
      PSZ pszModName, PSZ pszFileName, LPARAM lpUserDefined ) ;

   // The EnumProcs function takes a pointer to a callback function
   // that will be called once per process in the system providing
   // process EXE filename and process ID.
   // Callback function definition:
   // BOOL CALLBACK Proc( DWORD dw, LPCSTR lpstr, LPARAM lParam ) ;
   // 
   // lpProc -- Address of callback routine.
   // 
   // lParam -- A user-defined LPARAM value to be passed to
   //           the callback routine.
BOOL WINAPI EnumProcs( PROCENUMPROC lpProc, LPARAM lParam )
{
   OSVERSIONINFO  osver ;
   HINSTANCE      hInstLib ;
   HINSTANCE      hInstLib2 ;
   HANDLE         hSnapShot ;
   static PROCESSENTRY32 procentry;
   static MODULEENTRY32  modentry;
   static THREADENTRY32  thdentry;

   BOOL           bFlag ;
   LPDWORD        lpdwPIDs ;
   DWORD          dwSize, dwSize2, dwIndex ;
   HMODULE        hMod ;
   HANDLE         hProcess ;
   char           szFileName[ MAX_PATH ] ;
   EnumInfoStruct sInfo ;

   // ToolHelp Function Pointers.
   HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD,DWORD);

   BOOL (WINAPI *lpfProcess32First)(HANDLE,LPPROCESSENTRY32);
   BOOL (WINAPI *lpfProcess32Next)(HANDLE,LPPROCESSENTRY32);

   BOOL (WINAPI *lpfModule32First)(HANDLE,LPMODULEENTRY32);
   BOOL (WINAPI *lpfModule32Next)(HANDLE,LPMODULEENTRY32);

   BOOL (WINAPI *lpfThread32First)(HANDLE,LPTHREADENTRY32);
   BOOL (WINAPI *lpfThread32Next)(HANDLE,LPTHREADENTRY32);

   // PSAPI Function Pointers.
   BOOL (WINAPI *lpfEnumProcesses)( DWORD *, DWORD cb, DWORD * );
   BOOL (WINAPI *lpfEnumProcessModules)( HANDLE, HMODULE *, DWORD, LPDWORD );
   DWORD (WINAPI *lpfGetModuleFileNameEx)( HANDLE, HMODULE, LPTSTR, DWORD );

   // VDMDBG Function Pointers.
   INT (WINAPI *lpfVDMEnumTaskWOWEx)( DWORD, TASKENUMPROCEX  fp, LPARAM );

   // Check to see if were running under Windows95 or
   // Windows NT.
   osver.dwOSVersionInfoSize = sizeof( osver ) ;
   if( !GetVersionEx( &osver ) ) {
         return FALSE ;
   }

   // look like enumerate will work fine ...
   // **********************************
      // If Windows NT:
      if( osver.dwPlatformId == VER_PLATFORM_WIN32_NT )
      {

         // Load library and get the procedures explicitly. We do
         // this so that we don't have to worry about modules using
         // this code failing to load under Windows 95, because
         // it can't resolve references to the PSAPI.DLL.
         hInstLib = LoadLibraryA( "PSAPI.DLL" ) ;
         if( hInstLib == NULL )
            return FALSE ;

         hInstLib2 = LoadLibraryA( "VDMDBG.DLL" ) ;
         if( hInstLib2 == NULL )
            return FALSE ;

         // Get procedure addresses.
         lpfEnumProcesses = (BOOL(WINAPI *)(DWORD *,DWORD,DWORD*))
            GetProcAddress( hInstLib, "EnumProcesses" ) ;
         lpfEnumProcessModules = (BOOL(WINAPI *)(HANDLE, HMODULE *,
            DWORD, LPDWORD)) GetProcAddress( hInstLib,
            "EnumProcessModules" ) ;
         lpfGetModuleFileNameEx =(DWORD (WINAPI *)(HANDLE, HMODULE,
            LPTSTR, DWORD )) GetProcAddress( hInstLib,
            "GetModuleFileNameExA" ) ;
         lpfVDMEnumTaskWOWEx =(INT(WINAPI *)( DWORD, TASKENUMPROCEX,
            LPARAM))GetProcAddress( hInstLib2, "VDMEnumTaskWOWEx" );
         if( lpfEnumProcesses == NULL ||
            lpfEnumProcessModules == NULL ||
            lpfGetModuleFileNameEx == NULL ||
            lpfVDMEnumTaskWOWEx == NULL)
            {
               FreeLibrary( hInstLib ) ;
               FreeLibrary( hInstLib2 ) ;
               return FALSE ;
            }

         // Call the PSAPI function EnumProcesses to get all of the
         // ProcID's currently in the system.
         // NOTE: In the documentation, the third parameter of
         // EnumProcesses is named cbNeeded, which implies that you
         // can call the function once to find out how much space to
         // allocate for a buffer and again to fill the buffer.
         // This is not the case. The cbNeeded parameter returns
         // the number of PIDs returned, so if your buffer size is
         // zero cbNeeded returns zero.
         // NOTE: The "HeapAlloc" loop here ensures that we
         // actually allocate a buffer large enough for all the
         // PIDs in the system.
         dwSize2 = 256 * sizeof( DWORD ) ;
         lpdwPIDs = NULL ;
         do
         {
            if( lpdwPIDs )
            {
               HeapFree( GetProcessHeap(), 0, lpdwPIDs ) ;
               dwSize2 *= 2 ;
            }
            lpdwPIDs = (LPDWORD)HeapAlloc( GetProcessHeap(), 0, dwSize2 );
            if( lpdwPIDs == NULL )
            {
               FreeLibrary( hInstLib ) ;
               FreeLibrary( hInstLib2 ) ;
               return FALSE ;
            }
            if( !lpfEnumProcesses( lpdwPIDs, dwSize2, &dwSize ) )
            {
               HeapFree( GetProcessHeap(), 0, lpdwPIDs ) ;
               FreeLibrary( hInstLib ) ;
               FreeLibrary( hInstLib2 ) ;
               return FALSE ;
            }
         }while( dwSize == dwSize2 ) ;

         // How many ProcID's did we get?
         dwSize /= sizeof( DWORD ) ;

         // Loop through each ProcID.
         for( dwIndex = 0 ; dwIndex < dwSize ; dwIndex++ )
         {
            szFileName[0] = 0 ;
            // Open the process (if we can... security does not
            // permit every process in the system).
            hProcess = OpenProcess(
               PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
               FALSE, lpdwPIDs[ dwIndex ] ) ;

            if( hProcess != NULL )
            {
               // Here we call EnumProcessModules to get only the
               // first module in the process this is important,
               // because this will be the .EXE module for which we
               // will retrieve the full path name in a second.
               if( lpfEnumProcessModules( hProcess, &hMod,
                  sizeof( hMod ), &dwSize2 ) )
               {
                  // Get Full pathname:
                  if( !lpfGetModuleFileNameEx( hProcess, hMod,
                     szFileName, sizeof( szFileName ) ) )
                  {
                     szFileName[0] = 0 ;
                    }
               }
               CloseHandle( hProcess ) ;
            }

            // Regardless of OpenProcess success or failure, we
            // still call the enum func with the ProcID.
            // *****************************************
            if(!lpProc( lpdwPIDs[dwIndex], 0, szFileName, lParam))
               break ;

            // Did we just bump into an NTVDM?
            if( _stricmp( szFileName+(strlen(szFileName)-9),
               "NTVDM.EXE")==0)
            {
               // Fill in some info for the 16-bit enum proc.
               sInfo.dwPID = lpdwPIDs[dwIndex] ;
               sInfo.lpProc = lpProc ;
               sInfo.lParam = (DWORD)lParam ;
               sInfo.bEnd = FALSE ;
               // Enum the 16-bit stuff.
               lpfVDMEnumTaskWOWEx( lpdwPIDs[dwIndex],
                  (TASKENUMPROCEX) Enum16,
                  (LPARAM) &sInfo);

               // Did our main enum func say quit?
               if(sInfo.bEnd)
                  break ;
            }
         }

         HeapFree( GetProcessHeap(), 0, lpdwPIDs ) ;
         FreeLibrary( hInstLib2 ) ;

      // If Windows 95:
      }else if( osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
      {
         DWORD    dwCnt = 0;
         hInstLib = LoadLibraryA( "Kernel32.DLL" ) ;
         if( hInstLib == NULL )
            return FALSE ;

         // Get procedure addresses.
         // We are linking to these functions of Kernel32
         // explicitly, because otherwise a module using
         // this code would fail to load under Windows NT,
         // which does not have the Toolhelp32
         // functions in the Kernel 32.
         lpfCreateToolhelp32Snapshot=
            (HANDLE(WINAPI *)(DWORD,DWORD))
            GetProcAddress( hInstLib,
            "CreateToolhelp32Snapshot" ) ;
         lpfProcess32First=
            (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
            GetProcAddress( hInstLib, "Process32First" ) ;
         lpfProcess32Next=
            (BOOL(WINAPI *)(HANDLE,LPPROCESSENTRY32))
            GetProcAddress( hInstLib, "Process32Next" ) ;

         // try the others seen ...
         // ***********************
         lpfModule32First=
            (BOOL(WINAPI *)(HANDLE,LPMODULEENTRY32))
            GetProcAddress( hInstLib, "Module32First" ) ;
         lpfModule32Next=
            (BOOL(WINAPI *)(HANDLE,LPMODULEENTRY32))
            GetProcAddress( hInstLib, "Module32Next" ) ;

         lpfThread32First=
            (BOOL(WINAPI *)(HANDLE,LPTHREADENTRY32))
            GetProcAddress( hInstLib, "Thread32First" ) ;
         lpfThread32Next=
            (BOOL(WINAPI *)(HANDLE,LPTHREADENTRY32))
            GetProcAddress( hInstLib, "Thread32Next" ) ;

         if( lpfProcess32Next == NULL ||
            lpfProcess32First == NULL ||
            lpfCreateToolhelp32Snapshot == NULL )
         {
            FreeLibrary( hInstLib ) ;
            return FALSE ;
         }

         // Get a handle to a Toolhelp snapshot of the systems
         // processes.
         hSnapShot = lpfCreateToolhelp32Snapshot(
            TH32CS_SNAPPROCESS, 0 ) ;
         if( hSnapShot == INVALID_HANDLE_VALUE )
         {
            FreeLibrary( hInstLib ) ;
            return FALSE ;
         }

         // Get the first process' information.
         procentry.dwSize = sizeof(PROCESSENTRY32) ;
         bFlag = lpfProcess32First( hSnapShot, &procentry ) ;

         // While there are processes, keep looping.
         while( bFlag )
         {
            dwCnt++; // bump the counter
            // Call the enum func with the filename and ProcID.
            if(lpProc( procentry.th32ProcessID, &procentry,
               procentry.szExeFile, lParam ))
            {
               procentry.dwSize = sizeof(PROCESSENTRY32) ;
               bFlag = lpfProcess32Next( hSnapShot, &procentry );
            }else
               bFlag = FALSE ;
         }

         CloseHandle( hSnapShot );

         hSnapShot = lpfCreateToolhelp32Snapshot( TH32CS_SNAPMODULE, 0 );

         if( lpfModule32First && lpfModule32Next && VFH(hSnapShot) )
         {

//typedef struct tagMODULEENTRY32 { 
//  DWORD   dwSize; 
//  DWORD   th32ModuleID; 
//  DWORD   th32ProcessID; 
//  DWORD   GlblcntUsage; 
//  DWORD   ProccntUsage; 
//  BYTE  * modBaseAddr; 
//  DWORD   modBaseSize; 
//  HMODULE hModule; 
//  TCHAR   szModule[MAX_MODULE_NAME32 + 1]; 
//  TCHAR   szExePath[MAX_PATH]; 
//} MODULEENTRY32; 
//typedef MODULEENTRY32 *PMODULEENTRY32; 
            modentry.dwSize = sizeof(MODULEENTRY32) ;
            bFlag = lpfModule32First( hSnapShot, &modentry ) ;
            while( bFlag )
            {
               if(lpProc( modentry.th32ProcessID, &modentry,
                  modentry.szExePath, lParam ))
               {
                  modentry.dwSize = sizeof(MODULEENTRY32) ;
                  bFlag = lpfModule32Next( hSnapShot, &modentry );
               }else
                  bFlag = FALSE ;
            }

            CloseHandle( hSnapShot );
//typedef struct tagTHREADENTRY32{ 
//  DWORD  dwSize; 
//  DWORD  cntUsage; 
//  DWORD  th32ThreadID; 
//  DWORD  th32OwnerProcessID; 
//  LONG   tpBasePri; 
//  LONG   tpDeltaPri; 
//  DWORD  dwFlags; 
//} THREADENTRY32; 
//typedef THREADENTRY32 *PTHREADENTRY32; 
            thdentry.dwSize = sizeof(THREADENTRY32);
            hSnapShot = lpfCreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
            if( lpfThread32First && lpfThread32Next &&
               VFH(hSnapShot) )
            {
               bFlag = lpfThread32First( hSnapShot, &thdentry ) ;
               while( bFlag )
               {
                  //if(lpProc( modentry.th32ProcessID, &modentry,
                  //modentry.szExePath, lParam ))
                  //if(lpProc( thdentry.th32ThreadID, &thdentry,
                  if(lpProc( thdentry.th32OwnerProcessID,   // SORT per OWNER THREAD ID
                        &thdentry,
                        0, lParam ))
                  {
                     thdentry.dwSize = sizeof(THREADENTRY32) ;
                     bFlag = lpfThread32Next( hSnapShot, &thdentry );
                  }else
                     bFlag = FALSE ;
               }
            }

            if(VFH(hSnapShot))
               CloseHandle( hSnapShot );
            hSnapShot = 0;

         }

      }else
         return FALSE ;

   // Free the library.
   FreeLibrary( hInstLib ) ;
   return TRUE ;
}

BOOL WINAPI Enum16( DWORD dwThreadId, WORD hMod16, WORD hTask16,
      PSZ pszModName, PSZ pszFileName, LPARAM lpUserDefined )
{
   BOOL bRet ;

      EnumInfoStruct *psInfo = (EnumInfoStruct *)lpUserDefined ;

      bRet = psInfo->lpProc( psInfo->dwPID, (PVOID)hTask16, pszFileName,
         psInfo->lParam ) ;

      if(!bRet)
      {
         psInfo->bEnd = TRUE ;
      }

   return !bRet;
} 

/* ======== IF FULL LIB avail
   ************************************

//#include <windows.h>
//#include <tlhelp32.h>
//#include <stdio.h>
   ************************************ */

BOOL GetProcessModule (DWORD dwPID, DWORD dwModuleID, 
     LPMODULEENTRY32 lpMe32, DWORD cbMe32) //this function is copy/paste from mswinsdk
{ 
    BOOL          bRet        = FALSE; 
    BOOL          bFound      = FALSE; 
    HANDLE        hModuleSnap = NULL; 
    MODULEENTRY32 me32        = {0}; 
 
    // Take a snapshot of all modules in the specified process. 
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if ((hModuleSnap == NULL) || (hModuleSnap == INVALID_HANDLE_VALUE))
        return FALSE; 
 
    // Fill the size of the structure before using it. 
    me32.dwSize = sizeof(MODULEENTRY32); 
 
    // Walk the module list of the process, and find the module of 
    // interest. Then copy the information to the buffer pointed 
    // to by lpMe32 so that it can be returned to the caller. 

    if ( Module32First(hModuleSnap, &me32) ) 
    { 
        do 
        { 
            if (me32.th32ModuleID == dwModuleID) { 
                CopyMemory (lpMe32, &me32, cbMe32); 
                bFound = TRUE; 
            } 
        } 
        while (!bFound && Module32Next(hModuleSnap, &me32)); 
 
        bRet = bFound;   // if this sets bRet to FALSE, dwModuleID 
                         // no longer exists in specified process 
    } 
    else 
        bRet = FALSE;           // could not walk module list 
 
    // Do not forget to clean up the snapshot object. 

    CloseHandle (hModuleSnap); 
 
    return (bRet); 
} 

BOOL EnumProcessList_NOT_USED (VOID) 
{
    HANDLE         hProcessSnap = NULL; 
    BOOL           bRet      = FALSE; 
    PROCESSENTRY32 pe32      = {0}; 
 
    //  Take a snapshot of all processes in the system. 

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 

    if (hProcessSnap == INVALID_HANDLE_VALUE) 
        return (FALSE); 
 
    //  Fill in the size of the structure before using it. 

    pe32.dwSize = sizeof(PROCESSENTRY32); 
 
    //  Walk the snapshot of the processes, and for each process, 
    //  display information. 

    if ( Process32First(hProcessSnap, &pe32) ) 
    { 
        //DWORD         dwPriorityClass; 
        //BOOL          bGotModule = FALSE; 
        //MODULEENTRY32 me32       = {0}; 
        do 
        { 
            PMYPROCESSLIST ppl = new MYPROCESSLIST;
            ZeroMemory(ppl, sizeof(MYPROCESSLIST));
            CopyMemory( &ppl->pe32, &pe32, sizeof(PROCESSENTRY32) );

            ppl->bGotModule = GetProcessModule(ppl->pe32.th32ProcessID, 
                pe32.th32ModuleID, &ppl->me32, sizeof(MODULEENTRY32)); 

            if (ppl->bGotModule) 
            { 
                HANDLE hProcess; 
                 // Get the actual priority class. 
                hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID); 
                ppl->dwPriorityClass = GetPriorityClass (hProcess); 
                CloseHandle (hProcess); 

#ifdef PRINTF_PROCESS_INFO
                // Print the process's information. 
                EP_PRINTF( "PID\t\t\t%d\n", pe32.th32ProcessID);
                EP_PRINTF( "Priority Class Base\t%d\n", pe32.pcPriClassBase); 
                EP_PRINTF( "Thread Count\t\t%d\n", pe32.cntThreads);
                EP_PRINTF( "Module Name\t\t%s\n", me32.szModule);
                EP_PRINTF( "Full Path\t\t%s\n\n", me32.szExePath);
#endif // PRINTF_PROCESS_INFO
            }
            Add_to_Process_List( ppl );
        } 
        while ( Process32Next(hProcessSnap, &pe32) ); 
        bRet = TRUE; 
    } 
    else 
        bRet = FALSE;    // could not walk the list of processes 
 
    // Do not forget to clean up the snapshot object. 
    CloseHandle (hProcessSnap); 

    return bRet; 
} 

/*  ****************************************  */

// eof - EnumProc.c

