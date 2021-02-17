// find_Menu.cpp

#include "find_window.h"

#define MENU_FILE       0
#define MENU_OPTIONS    1
#define MENU_HELP       2

#define  mf_Enab     (MF_BYCOMMAND | MF_ENABLED)
#define  mf_Disab    (MF_BYCOMMAND | MF_DISABLED | MF_GRAYED)
#define  EMI(a,b) EnableMenuItem(hMenu, a, ((b) ? mf_Enab : mf_Disab))
#define  CMI(a,b) CheckMenuItem(hMenu,  a, ((b) ? MF_CHECKED : MF_UNCHECKED) )

LRESULT Do_WM_INITMENUPOPUP( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
   LRESULT  lRet = 0;
   HMENU    hMenu = (HMENU)wParam;
   DWORD    dwPos = LOWORD(lParam);
   BOOL     bSys  = HIWORD(lParam);
   if( bSys )
       return 1;
   switch( dwPos )
   {
   case MENU_FILE:
       break;
   case MENU_OPTIONS:
       CMI( IDM_SORTTOGGLE, gb_PaintPerIndex );
       CMI( IDM_PROPTOGGLE, gb_Propagate     );
       break;
   }
   return lRet;
}

// FIX20080121 - Remove DUPLICATES in POPUP context menu
#define  MAX_MENU_ITEMS 32
UINT  uMenuCount = 0;
UINT  uMenuItems[MAX_MENU_ITEMS];

//#define  AM(a,b)  AppendMenu(hMenu,uFlags,a,b)
#define  AM(a,b)  Append_2_Menu(hMenu,uFlags,a,b)

BOOL Append_2_Menu( HMENU hMenu, UINT uFlags, UINT uCmd, PTSTR lpb )
{
   BOOL  bRet = FALSE;
   //if( Not_Yet_Added( uCmd ) )
      bRet = AppendMenu( hMenu, uFlags, uCmd, lpb );
   return bRet;
}

VOID  Do_WM_RBUTTONDOWN( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    POINT pnt;
    HMENU hMenu;
    UINT  uFlags = MF_STRING | MF_BYCOMMAND;
    UINT  uOpts = TPM_LEFTALIGN | TPM_TOPALIGN;

    pnt.x = LOWORD(lParam);
    pnt.y = HIWORD(lParam);
    ClientToScreen(hWnd, &pnt);

    hMenu = CreatePopupMenu(); // create a POPUP Menu
    uMenuCount = 0;   // clear ADDED commands
    if(hMenu) {
        AM( IDM_SORTTOGGLE, (gb_PaintPerIndex ? "Sort Off" : "Sort On") );
        AM( IDM_PROPTOGGLE, (gb_Propagate ? "Prop Off" : "Prop On")     );
        AppendMenu(hMenu, MF_SEPARATOR, 0, 0    );
        AM( IDM_EXIT,  "Exit!"                  );

        // TrackPopupMenu()
        TrackPopupMenu( hMenu,         // handle to shortcut menu
            uOpts,   // TPM_CENTERALIGN | TPM_VCENTERALIGN, // options
            pnt.x,   // horizontal position
            pnt.y,   // vertical position
            0,       // reserved, must be zero
            hWnd,    // handle to owner window
            0 );     // ignored

        DestroyMenu(hMenu);
    }
}

// eof - find_Menu.cpp
