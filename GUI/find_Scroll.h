#pragma once

extern LRESULT Do_WM_HSCROLL(HWND hWnd, WPARAM wParam, LPARAM lParam);
extern LRESULT Do_WM_VSCROLL(HWND hWnd, WPARAM wParam, LPARAM lParam);
extern VOID Go_To_Top( HWND hWnd );
extern VOID Go_To_Bottom( HWND hWnd );
extern BOOL m_SetScrollRange( HWND hWnd, int nBar, int nMinPos, int nMaxPos, BOOL bRedraw );

// eof - find_Scroll.h


