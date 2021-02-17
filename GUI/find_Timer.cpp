// find_Timer.cpp

#include "find_window.h"

UINT  g_minsElapsed = 0;
UINT  g_secCount = 0;

UINT  g_msCount = 0;
UINT  g_minCount = 0;
UINT  g_hourCount = 0;
UINT  g_minsWrap = 10;
UINT  g_Ticks = 0;

VOID Do_WM_TIMER( HWND hWnd )
{
    static int done_window_list = 0;
   if((g_Ticks == 0) && ( !done_window_list ))
   {
        done_window_list = 1;
        Paint_Center_Message( hWnd, "Moment... Enumerating processes..." );
        EnumProcessList ();
        Paint_Center_Message( hWnd, "Moment... Enumerating windows..." );
        Get_Window_List( hWnd );
   }
   g_Ticks++;
   g_msCount += TIMER_TIC;
   while( g_msCount >= 1000 ) {
      g_secCount++;
      g_msCount -= 1000;
   }
   while( g_secCount >= 60 ) {
      g_minCount++;
      g_secCount -= 60;
      g_minsElapsed++;
      Paint_Minutes( g_hWnd );
   }
   while( g_minCount >= 60 ) {
      g_hourCount++;
      g_minCount -= 60;
   }
}


// eof - find_Timer.cpp
