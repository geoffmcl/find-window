
#pragma once

extern VOID  Do_WM_PAINT( HWND hWnd );
extern VOID  Paint_Minutes( HWND hwnd );
extern VOID Paint_Center_Message( HWND hWnd, PTSTR pstr );
extern HFONT  Get_CN8Font( HWND hWnd );
extern VOID Get_Text_Size(HWND hWnd, HDC hdc);

extern BOOL gb_PaintPerIndex;

// eof - find_Paint.h
