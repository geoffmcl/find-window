
#pragma once

#define  TIMER_ID       0x1234
#define  TIMER_TIC      200      // each 200 ms

extern VOID Do_WM_TIMER( HWND hWnd );

extern UINT  g_secCount;
extern UINT  g_minsElapsed;

// eof - find_Timer.h
