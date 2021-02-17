
#pragma once

#include "find_EnumP.h"

#define MMX_WIN_TITLE   1024
#define MMX_WIN_CLASS   260

#define flg_INFIND  0x00000001
#define flg_SELF    0x00000002
#define flg_DONEIT  0x80000000

typedef struct tagWINLIST {
    void * next;
    UINT   flag;
    UINT   index;
    HWND   hwnd;
    DWORD  dwThreadID; // GetWindowThreadProcessId(hWnd,&dwProcID)
    DWORD  dwProcID;   // FindProcID(hWnd, dwProcID, dwThreadID )
    PALLPROCESSLIST pProcInfo;
    int    dntsize;
    size_t tlen;
    SIZE   tsz;
    TCHAR  winclass[MMX_WIN_CLASS];
    TCHAR  wintitle[MMX_WIN_TITLE];
}WINLIST, * PWINLIST;

extern PWINLIST pWinList;

extern int Add_Stg_to_List( PTSTR pstr );
extern int Add_Window_to_List( HWND hwnd );
extern int Get_List_Count(void);
extern void Delete_Line_List(void);
extern VOID Get_Window_List( HWND hWnd );
extern VOID Reset_Win_Finds(VOID);
extern VOID Set_Win_Finds(VOID);

extern int gi_StartLine;
extern int gi_LeftPosn;
extern SIZE g_TxtSize;
extern RECT g_ClientRect;
extern int  gi_Height, gi_Width;
extern int  gi_OneCharWidth;
extern int  gi_ScreenLines;
extern size_t  gi_ScreenChars; // using CN8 font, number of char across screen
extern int  gi_LineCount;
extern size_t gi_MaxLineLength;

extern size_t gi_MaxPxThisScreen;   // maximum PIXEL width, this screen
extern size_t gi_MaxChThisScreen;   // maximum CHAR COUNT, this screen

extern DWORD gdw_MaxIndex;

extern int mark_Find(PTSTR pfind);

// eof - find_list.h

