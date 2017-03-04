/*\
 * find-window.cpp
 *
 * Copyright (c) 2012-2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
// find-window.cpp : Defines the entry point for the console application.
//

#include "find-window.h"

#include <vector>
#include <string>

using namespace std;

typedef vector<HWND>    vHWND;
typedef vHWND::iterator vHi;

typedef vector<string> vSTRING;
typedef vSTRING::iterator vSi;

vHWND vhwnds;
vHWND vFinds;
vSTRING vsSkip;
static HWND conWin = 0;

#define ISNUM(a)    ((a >= '0') && (a <= '9'))

char *title_to_find = 0;
int case_compare = 0;
int whole_compare = 0;
int verbosity = 0;
int iret = 0;
int skip_found = 0;
int do_set_window_focus = 0;

BOOL skipped_console = FALSE;


static const char *def_log = "tempfind.txt";

int VERB1() { return (verbosity >= 1) ? 1 : 0; }
int VERB2() { return (verbosity >= 2) ? 1 : 0; }
int VERB5() { return (verbosity >= 5) ? 1 : 0; }
int VERB9() { return (verbosity >= 9) ? 1 : 0; }

char *get_basename(char *name)
{
    char *title = name;
    size_t len = strlen(name);
    size_t i;
    int c;
    for (i = 0; i < len; i++) {
        c = name[i];
        if ((c == '/')||(c == '\\'))
            title = &name[i+1];
    }
    return title;
}

char *get_basepath(char *name)
{
    char *cp = GetNxtBuf();
    strcpy(cp,name);
    size_t len = strlen(cp);
    size_t i;
    int c;
    size_t last = 0;
    for (i = 0; i < len; i++) {
        c = cp[i];
        if ((c == '/')||(c == '\\'))
            last = i + 1;
        if ( c == '/' )
            cp[i] = '\\';
    }
    cp[last] = 0;
    return cp;
}


//UINT    g_uWinCount = 0;
#define MX_WIN_TITLE 264
char window_title[MX_WIN_TITLE+2];
/* =================================================
    20130519 - Another idea about 'whole_compare'
   ================================================= */
int my_strncmpi(char *cp1, char *cp2, size_t len)
{
    size_t len1 = strlen(cp1);
    size_t len2 = strlen(cp2);
    size_t i = 0;
    int a, b;
    while (len) {
        a = tolower(cp1[i]);
        b = tolower(cp2[i]);
        if (a != b)
            return (a - b);
        i++;    // up to next char
        len--;  // and reduce length to seek
        if ((a == 0)||(b == 0)) {
            return -1;
        }
    }
    return 0;
}

BOOL compare_cp_2_title(char *cp, char *title)
{
    size_t lent, len;
    if (whole_compare) {
        if (case_compare) {
            if (strcmp(cp,title) == 0)
                return TRUE;
        } else {
            if (strcmpi(cp,title) == 0)
                return TRUE;
        }
        // 20130519 - just compare the beginning...
        lent = strlen(title);   // title to find
        len  = strlen(cp);      // title of this window
        if (len > lent) {
            if (case_compare) {
                if (strncmp(cp,title,lent) == 0)
                    return TRUE;
            } else {
                if (my_strncmpi(cp,title,lent) == 0)
                    return TRUE;
            }
        }
    } else {
        if (case_compare) {
            if (InStr(cp,title))
                return TRUE;
        } else {
            if (InStri(cp,title))
                return TRUE;
        }
    }
    return FALSE;
}

BOOL title_matches(char *cp, int len) 
{
    char *title = title_to_find;
    if (!title)
        return FALSE;
    if (len == 0)
        return FALSE;

    return compare_cp_2_title(cp,title);

}

void set_window_focus( HWND hwnd )
{
    if (VERB5()) {
        sprtf("Attempting to bring [%p] window to top...\n", hwnd);
    }
    if (BringWindowToTop(hwnd) == 0) {
        DWORD dwi = GetLastError();
        char *cp = GetNxtBuf();
        sprintf(cp, "BringWindowToTop(%p)",hwnd);
        printError( cp, dwi );
    }
    
    if (SetForegroundWindow(hwnd) == 0) {
        DWORD dwi = GetLastError();
        char *cp = GetNxtBuf();
        sprintf(cp, "SetForegroundWindow(%p)",hwnd);
        printError( cp, dwi );
    }
}

/* =========================================================================================
* 20130519 - UGH Since Windows 7 changes the title of the console to match the
* command, this means a window is ALWAYS found
* Either need to get MORE information about the window, and somehow eliminate
* the console the command was run in, like -
BOOL WINAPI GetWindowInfo( HWND hwnd, PWINDOWINFO pwi );
typedef struct tagWINDOWINFO {
DWORD cbSize;
RECT  rcWindow;
RECT  rcClient;
DWORD dwStyle;
DWORD dwExStyle;
DWORD dwWindowStatus;
UINT  cxWindowBorders;
UINT  cyWindowBorders;
ATOM  atomWindowType;
WORD  wCreatorVersion;
} WINDOWINFO, *PWINDOWINFO, *LPWINDOWINFO;
Where -
dwWindowStatus Type: DWORD
The window status. If this member is WS_ACTIVECAPTION (0x0001), the window
is active. Otherwise, this member is zero.
So could IGNORE the 'ACTIVE' window since that is obvious NOT the window sought
OR
Using the title search, only if the title begins with the title sought

Or maybe skip the associated console window
HWND WINAPI GetConsoleWindow(void);
* ========================================================================================= */

// type
// 1 = title matches
// 2 = reshow at end if VERB9
// 9 = only VERB9
void show_window(HWND hwnd, int type)
{
    DWORD dwProcID;
    DWORD dwThreadID = GetWindowThreadProcessId( hwnd, &dwProcID );
    // Find in process list, this process ID (PID)
    // PALLPROCESSLIST pProcInfo = FindProcID( hwnd, dwProcID, dwThreadID );
    char *cp = window_title;
    int len = GetWindowText( hwnd, cp, MX_WIN_TITLE );
    printf("%p [%08X:%08X] [%s]%d \n", hwnd, dwProcID, dwThreadID, cp, len);
    if (type != 9) {
        // show MORE information about window
        WINDOWPLACEMENT wp;
        WINDOWINFO wi;
        BOOL gwp = GetWindowPlacement(hwnd, &wp);
        wi.cbSize = sizeof(WINDOWINFO);
        BOOL gwi = GetWindowInfo(hwnd, &wi);
        if (gwi) {
            int width = wi.rcClient.right - wi.rcClient.left;
            int height = wi.rcClient.bottom - wi.rcClient.top;
            printf("Style: %s\n", GetWS(wi.dwStyle));
            printf("Size: %s, Width: %d, Height: %d, Pos: %s\n", GetRECTStg(&wi.rcClient), width, height, GetRECTStg(&wi.rcWindow));
        }

    }
}

void log_window( HWND hwnd, char *cp, int len )
{
    DWORD dwProcID;
    DWORD dwThreadID = GetWindowThreadProcessId( hwnd, &dwProcID );
    sprtf("%p [%08X:%08X] [%s]%d \n", hwnd, dwProcID, dwThreadID, cp, len );
}

void out_to_log( HWND hwnd )
{
    char *cp = window_title;
    int len = GetWindowText( hwnd, cp, MX_WIN_TITLE );
    log_window( hwnd, cp, len );
}

void save_window(HWND hwnd, char *cp, int len)
{
    iret++;
    vFinds.push_back(hwnd);
}

BOOL is_in_skip( HWND hwnd, char *cp, int len )
{
    BOOL bRet = FALSE;
    size_t cnt = vsSkip.size();
    if (cnt) {
        vSi ii = vsSkip.begin();
        char *title;
        for ( ; ii != vsSkip.end(); ii++ ) {
            title = (char *)(*ii).c_str();
            if (compare_cp_2_title( cp, (char *)title )) {
                bRet = TRUE;
                break;
            }
        }
    }
    if (!bRet) {
        if (conWin && (conWin == hwnd)) {
            bRet = TRUE;
            skipped_console = TRUE;
        }
    }
    return bRet;
}

BOOL process_hwnd( HWND hwnd )
{
    BOOL bRet = FALSE;
    char *cp = window_title;
    int len = GetWindowText( hwnd, cp, MX_WIN_TITLE );
    log_window( hwnd, cp, len );
    if ( len && title_matches(cp,len) ) {
        BOOL b = is_in_skip(hwnd,cp,len);
        if (b) {
            skip_found++;
        } else {
            save_window(hwnd,cp,len);
            bRet = TRUE;
        }
    }
    if (bRet && VERB1()) {
        show_window(hwnd, 1);
    } else if ( VERB9() ) {
        show_window(hwnd, 9);
    }
    return bRet;
}

BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
    vhwnds.push_back(hwnd);
    return TRUE;
}

void give_help(char *name);

void do_log_file(char *name)
{
    char *cp = GetNxtBuf();
    DWORD dwd = GetModuleFileName( NULL, cp, 256 );
    if (dwd == 256)
        strcpy(cp,name);
    size_t len = strlen(cp);
    char c;
    size_t i;
    size_t last;
    last = 0;
    for (i = 0; i < len; i++) {
        c = cp[i];
        if (( c == '/' ) || ( c == '\\' ))
            last = i + 1;
        if ( c == '/' )
            cp[i] = '\\';
    }
    cp[last] = 0;
    strcat(cp,def_log);
    // printf("Setting log file to '%s'\n",cp);
    set_log_file(cp,false);
}

int _tmain(int argc, _TCHAR* argv[])
{
    int i;
    char *arg;
    char *sarg;
    string s;

    do_log_file(argv[0]);
    conWin = GetConsoleWindow();

    if (argc < 2) {
        // give_help(argv[0]);
        printf("ERROR: No window title to find found in command!\n");
        return 1;
    }
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        if (arg[0] == '-') {
            sarg = &arg[1];
            while (*sarg == '-') sarg++;

            if ((strcmp(arg,"--help") == 0) ||
                (strcmp(arg,"-h") == 0) ||
                (strcmp(arg,"-?") == 0)) {
                give_help(argv[0]);
                return 2;
            } else if ((strcmp(arg,"--case") == 0)||
                       (strcmp(arg,"-c") == 0)||
                       (*sarg == 'c')) {
                case_compare = 1;
            } else if ((strcmp(arg,"--whole") == 0)||
                       (strcmp(arg,"-w") == 0)||
                       (*sarg == 'w')) {
                whole_compare = 1;
            } else if (*sarg == 'v') {
                if (strncmp(arg,"--verb",6) == 0) {
                    sarg = &arg[6];
                } else if (strncmp(arg,"-v",2) == 0) {
                    sarg = &arg[2];
                } else {
                    goto Bad_Arg;
                }
                if (*sarg) {
                    if (*sarg == '=') {
                        sarg++;
                        if (ISNUM(*sarg)) {
                            verbosity = atoi(sarg);
                        } else 
                            goto Bad_Arg;
                    } else
                        goto Bad_Arg;
                } else {
                    verbosity++;
                }
            } else if (*sarg == 's') {
                if (strncmp(arg,"--skip=",7) == 0) {
                    sarg = &arg[7];
                } else if (strncmp(arg,"-s=",3) == 0) {
                    sarg = &arg[3];
                } else {
                    goto Bad_Arg;
                }
                s = sarg;
                if (s.length()) {
                    vsSkip.push_back(s);
                } else
                    goto Bad_Arg;
            } else if (*sarg == 't') {
                do_set_window_focus = 1;    // attempt to make 'found' top of z order
            } else {
Bad_Arg:
                printf("ERROR: Unknown command [%s]! Try -? for help. Aborting (3)\n", arg);
                return 3;
            }
        } else {
            // bare title to find
            title_to_find = arg;
        }
    }
    if (!title_to_find) {
        printf("ERROR: No 'title' to find found in command! Aborting (3)\n");
        return 3;
    }
    vhwnds.clear();
    vFinds.clear();
    EnumWindows( EnumWindowsProc, (LPARAM)0 );
    if (VERB1()) {
        printf("EnumWindows returned %d handles...\n", (int)vhwnds.size());
    }

    sprtf("EnumWindows returned %d handles...\n", (int)vhwnds.size());

    vHi ii = vhwnds.begin();
    int found = 0;
    HWND hwnd;
    for ( ; ii != vhwnds.end(); ii++ ) {
        hwnd = *ii;
        if (process_hwnd(hwnd) )
            found++;
    }
    sprtf("Found %d, seeking [%s], case %s, %s, at verbosity %d ",
        found,
        title_to_find,
        (case_compare ? "sensitive" : "insenstive"),
        (whole_compare ? "whole" : "partial"),
        verbosity );
    if (skip_found) {
        if (skipped_console) {
            sprtf("but skipped console");
            if (skip_found > 1)
                sprtf(", and %d due to -s", (skip_found - 1));
        }
        else {
            sprtf("but skipped %d due to -s", skip_found);
        }
    }
    sprtf("\n");
    if (VERB1()) {
        printf("Found %d, seeking [%s], case %s, %s, at verbosity %d ",
            found,
            title_to_find,
            (case_compare ? "sensitive" : "insenstive"),
            (whole_compare ? "whole" : "partial"),
            verbosity );
        if (skip_found) {
            if (skipped_console) {
                printf("but skipped console");
                if (skip_found > 1)
                    printf(", and %d due to -s", (skip_found - 1));
            }
            else {
                printf("but skipped %d due to -s", skip_found);
            }
        }
        printf("\n");
    }

    if (VERB9()) {
        // reshow the match found
        printf("List of %d windows matching...\n", found);
        ii = vFinds.begin();
        for ( ; ii != vFinds.end(); ii++ ) {
            hwnd = *ii;
            show_window(hwnd,2);
        }
    }
    sprtf("List of %d windows matching...\n", found);
    ii = vFinds.begin();
    for ( ; ii != vFinds.end(); ii++ ) {
        hwnd = *ii;
        out_to_log(hwnd);
    }
    if (do_set_window_focus) {
        ii = vFinds.begin();
        for ( ; ii != vFinds.end(); ii++ ) {
            hwnd = *ii;
            set_window_focus(hwnd);
        }
    }
    vhwnds.clear();
    vFinds.clear();
    if (VERB1()) {
        sprtf("Exit ERRORLEVEL %d\n",iret);
    }
    sprtf("End of application...\n");
    close_log_file();

	return iret;    // relects the number of finds
}

void give_help(char *name)
{
    char *title = get_basename(name);
    char *bpath = get_basepath(name);
    strcat(bpath,def_log);
    printf("%s: version %s, circa %s\n",
        title, VERSION, VERDATE );
    printf("Usage: %s [options] title-to-find\n", title );
    printf("Options:\n");
    printf(" --help  (-h or -?) = Show this help and exit(2)\n");
    printf(" --find=title  (-f) = Alternative way to set title to find.\n");
    printf(" --verb[=N]    (-v) = Bump or set verbosity\n");
    printf(" --skip=title  (-s) = Skip windows containing this in the title.\n");
    printf(" --whole       (-w) = Only matching this whole title.\n");
    printf(" --case        (-c) = Set case sensitive when searching.\n");
    printf(" --top         (-t) = Attempt to bring found window to top of z order.\n");
    printf(" Will enumerate ALL windows, and exit(1) if title 'found',\n");
    printf(" and show found window details if verbosity set to at least 1,\n");
    printf(" Else exit(0) = none found matching. Errors will exit(2)\n");
    printf(" Verbosity 9 will show ALL windows enumerated.\n");
    printf(" List of all windows found, together with some other information,\n");
    printf(" is written to '%s', the runtime of the exe.\n", bpath);
}

// eof - find-window.cpp

