========================================================================
    WIN32 APPLICATION : find_window Project Overview
========================================================================

File List:

Pairs:
find_ABT.cpp     find_ABT.h
find_CMD.cpp     find_CMD.h
find_EnumP.cpp   find_EnumP.h
find_EnumW.cpp   find_EnumW.h
find_INI.cpp     find_INI.h
find_Input.cpp   find_Input.h
find_Menu.cpp    find_Menu.h
find_Paint.cpp   find_Paint.h
find_Scroll.cpp  find_Scroll.h
find_Sprtf.cpp   find_Sprtf.h
find_Timer.cpp   find_Timer.h
find_Utils.cpp   find_Utils.h
find_window.cpp  find_window.h

Others:
find_VER.h
resource.h
targetver.h

The action starts on the first timer tick -
   if((g_Ticks == 0) && ( !done_window_list )) {
        EnumProcessList ();
        Get_Window_List( hWnd );

First action is to enumerate the processes running. This list is kept in a structure ALLPROCESSLIST,
which can includes a Module list (PMODULELIST), and a Thread list (PTHREADLIST). This list is 
written to the log file. This function BOOL GetProcessList -> GetProcessList(void) uses -
 hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

Then the 2nd action is to get the Window list - Get_Window_List() - using :-
  EnumWindows( EnumWindowsProc, (LPARAM)0 ); 

BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam ) {
   g_uWinCount += Add_Window_to_List( hwnd );
   ...
The information is stored in a WINLIST structure. The GetWindowThreadProcessId function retrieves the identifier of 
the thread that created the specified window and, optionally, the identifier of the process that created the window.
Then the above process list is searched to match this window to the process list -
PALLPROCESSLIST FindProcID( HWND hwnd, DWORD dwProcID, DWORD dwThreadID )

This pointer, if found in stored in pwl->pProcInfo, matching a Window to a process.

The string to paint is already preformatted into pn->wintitle[1024]. Any string can be added to the painted 
list using int Add_Stg_to_List( PTSTR pstr );, which creates a 'dummy' WINLIST. But during the enumeration 
the above Add_Window_to_List() does the job.

There is also a small command line interface
"Find Window HELP\n"
"Usage: find_window [Options] window_to_find\n"
"Options:\n"
" -f --find=name - Alternative input of window to find\n"
" -n --not=name  - Skip windows containing this word.\n"
" -p --propagate[=no|yes] - Mark all of the same process. (Def=On)\n"
" -h --help - Put up this help dialog, and exit.\n"
" -v --version - Put up the About dialog, and exit.\n"
"All information is output to a log file.\n";

Previous options are kept in an INI file - stored in %APPDATA%/find-window

/////////////////////////////////////////////////////////////////////////////
