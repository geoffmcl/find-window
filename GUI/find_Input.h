
#pragma once

#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
extern VOID Do_WM_MOUSEWHEEL(HWND hWnd, WPARAM wParam, LPARAM lParam);
#endif

extern VOID Do_WM_KEYDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam );
extern VOID Do_WM_KEYUP( HWND hWnd, WPARAM wParam, LPARAM lParam );

// eof - find_Input.cpp
