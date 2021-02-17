
#pragma once

#define  MX_ONE_BUF  1024
#define  MX_BUFFERS  256
#define BUFSIZE 80

extern PTSTR GetNxtBuf( void );
extern PTSTR GetRECTStg( PRECT pr );
extern INT InStr( PTSTR lpb, PTSTR lps );
extern INT InStri( PTSTR lpb, PTSTR lps );  // case insensitive

extern BOOL CenterWindow(HWND hwndChild, HWND hwndParent);

extern PTSTR Get_OS_Version_string(VOID);

#ifndef  EndBuf
#define  EndBuf(a)   ( a + strlen(a) )
#endif
#define EndBufW(a)  (a + lstrlen(a))

extern void GetAppData(PTSTR);

// eof - find_Utils.cpp
