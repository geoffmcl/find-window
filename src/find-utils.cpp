/*\
 * find_utils.cpp
 *
 * Copyright (c) 2012-2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include "find-window.h"

static char _s_strbufs[MX_ONE_BUF * MX_BUFFERS];
static int iNextBuf = 0;

PTSTR GetNxtBuf( void )
{
   iNextBuf++;
   if(iNextBuf >= MX_BUFFERS)
      iNextBuf = 0;
   return &_s_strbufs[MX_ONE_BUF * iNextBuf];
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : InStr
// Return type: INT 
// Arguments  : LPTSTR lpb
//            : LPTSTR lps
// Description: Return the position of the FIRST instance of the string in lps
//              Emulates the Visual Basic function.
///////////////////////////////////////////////////////////////////////////////
INT   InStr( PTSTR lpb, PTSTR lps )
{
   INT   iRet = 0;
   size_t  i, j, k, l, m;
   TCHAR    c;
   i = strlen(lpb);
   j = strlen(lps);
   if( i && j && ( i >= j ) )
   {
      c = *lps;   // get the first we are looking for
      l = i - ( j - 1 );   // get the maximum length to search
      for( k = 0; k < l; k++ )
      {
         if( lpb[k] == c )
         {
            // found the FIRST char so check until end of compare string
            for( m = 1; m < j; m++ )
            {
               if( lpb[k+m] != lps[m] )   // on first NOT equal
                  break;   // out of here
            }
            if( m == j )   // if we reached the end of the search string
            {
               iRet = (INT)(k + 1);  // return NUMERIC position (that is LOGICAL + 1)
               break;   // and out of the outer search loop
            }
         }
      }  // for the search length
   }
   return iRet;
}

INT   InStri( PTSTR lpb, PTSTR lps )
{
   INT   iRet = 0;
   size_t  i, j, k, l, m;
   TCHAR    c;
   i = strlen(lpb);
   j = strlen(lps);
   if( i && j && ( i >= j ) )
   {
      c = toupper(*lps);   // get the first we are looking for
      l = i - ( j - 1 );   // get the maximum length to search
      for( k = 0; k < l; k++ )
      {
         if( toupper(lpb[k]) == c )
         {
            // found the FIRST char so check until end of compare string
            for( m = 1; m < j; m++ )
            {
               if( toupper(lpb[k+m]) != toupper(lps[m]) )   // on first NOT equal
                  break;   // out of here
            }
            if( m == j )   // if we reached the end of the search string
            {
               iRet = (INT)(k + 1);  // return NUMERIC position (that is LOGICAL + 1)
               break;   // and out of the outer search loop
            }
         }
      }  // for the search length
   }
   return iRet;
}

void printError( TCHAR* msg, DWORD eNum )
{
    // DWORD eNum;
    TCHAR sysMsg[512];
    TCHAR* p;

    //eNum = GetLastError( );

    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, eNum,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        sysMsg, 512, NULL );

    // Trim the end of the line and terminate it with a null
    p = sysMsg;
    while( ( *p > 31 ) || ( *p == 9 ) )
        ++p;    // get to end

    do {
        *p-- = 0;
    } while( ( p >= sysMsg ) && (( *p == '.' ) || ( *p < 33 )) );

    // Display the message
    printf( "\n  WARNING: %s failed with error %d (%s)", msg, eNum, sysMsg );

}

typedef struct tagWINSTYLE {
    DWORD   style;
    const char *name;
    const char *value;
    const char *desc;
    DWORD flag;
}WINSTYLE, *PWINSTYLE;

static WINSTYLE winStyles[] = {
    { WS_BORDER, "WS_BORDER", "0x00800000L", "The window has a thin - line border.", 1 },
    { WS_CAPTION, "WS_CAPTION", "0x00C00000L", "The window has a title bar(includes the WS_BORDER style).", 0 },
    { WS_CHILD, "WS_CHILD", "0x40000000L", "The window is a child window.A window with this style cannot have a menu bar.This style cannot be used with the WS_POPUP style.", 1 },
    //{ WS_CHILDWINDOW, "WS_CHILDWINDOW", "0x40000000L", "Same as the WS_CHILD style.", 0 },
    { WS_CLIPCHILDREN, "WS_CLIPCHILDREN", "0x02000000L", "Excludes the area occupied by child windows when drawing occurs within the parent window.This style is used when creating the parent window.", 1 },
    { WS_CLIPSIBLINGS, "WS_CLIPSIBLINGS", "0x04000000L", "Clips child windows relative to each other; that is, when a particular child window receives a WM_PAINT message, the WS_CLIPSIBLINGS style clips all other overlapping child windows out of the region of the child window to be updated.If WS_CLIPSIBLINGS is not specified and child windows overlap, it is possible, when drawing within the client area of a child window, to draw within the client area of a neighboring child window.", 1 },
    { WS_DISABLED, "WS_DISABLED", "0x08000000L", "The window is initially disabled.A disabled window cannot receive input from the user.To change this after a window has been created, use the EnableWindow function.", 1 },
    { WS_DLGFRAME, "WS_DLGFRAME", "0x00400000L", "The window has a border of a style typically used with dialog boxes.A window with this style cannot have a title bar.", 1 },
    { WS_GROUP, "WS_GROUP", "0x00020000L", "The window is the first control of a group of controls.The group consists of this first control and all controls defined after it, up to the next control with the WS_GROUP style.The first control in each group usually has the WS_TABSTOP style so that the user can move from group to group.The user can subsequently change the keyboard focus from one control in the group to the next control in the group by using the direction keys.", 1 },
    // You can turn this style on and off to change dialog box navigation.To change this style after a window has been created, use the SetWindowLong function.
    { WS_HSCROLL, "WS_HSCROLL", "0x00100000L", "The window has a horizontal scroll bar.", 1 },
    // { WS_ICONIC, "WS_ICONIC", "0x20000000L", "The window is initially minimized.Same as the WS_MINIMIZE style.", 1 },
    { WS_MAXIMIZE, "WS_MAXIMIZE", "0x01000000L", "The window is initially maximized.", 1 },
    { WS_MAXIMIZEBOX, "WS_MAXIMIZEBOX", "0x00010000L", "The window has a maximize button.Cannot be combined with the WS_EX_CONTEXTHELP style.The WS_SYSMENU style must also be specified.", 1 },
    { WS_MINIMIZE, "WS_MINIMIZE", "0x20000000L", "The window is initially minimized.Same as the WS_ICONIC style.", 1 },
    { WS_MINIMIZEBOX, "WS_MINIMIZEBOX", "0x00020000L", "The window has a minimize button.Cannot be combined with the WS_EX_CONTEXTHELP style.The WS_SYSMENU style must also be specified.", 1 },
    //{ WS_OVERLAPPED, "WS_OVERLAPPED", "0x00000000L", "The window is an overlapped window.An overlapped window has a title bar and a border.Same as the WS_TILED style.", 0 },
    { WS_OVERLAPPEDWINDOW, "WS_OVERLAPPEDWINDOW", "(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)", "The window is an overlapped window.Same as the WS_TILEDWINDOW style.", 0 },
    { WS_POPUP, "WS_POPUP", "0x80000000L", "The windows is a pop - up window.This style cannot be used with the WS_CHILD style.", 1 },
    { WS_POPUPWINDOW, "WS_POPUPWINDOW", "(WS_POPUP | WS_BORDER | WS_SYSMENU)", "The window is a pop - up window.The WS_CAPTION and WS_POPUPWINDOW styles must be combined to make the window menu visible.", 0 },
    { WS_SIZEBOX, "WS_SIZEBOX", "0x00040000L", "The window has a sizing border.Same as the WS_THICKFRAME style.", 1 },
    { WS_SYSMENU, "WS_SYSMENU", "0x00080000L", "The window has a window menu on its title bar.The WS_CAPTION style must also be specified.", 1 },
    { WS_TABSTOP, "WS_TABSTOP", "0x00010000L", "The window is a control that can receive the keyboard focus when the user presses the TAB key.Pressing the TAB key changes the keyboard focus to the next control with the WS_TABSTOP style.", 1 },
    // You can turn this style on and off to change dialog box navigation.To change this style after a window has been created, use the SetWindowLong function.For user - created windows and modeless dialogs to work with tab stops, alter the message loop to call the IsDialogMessage function.
    // { WS_THICKFRAME, "WS_THICKFRAME", "0x00040000L", "The window has a sizing border.Same as the WS_SIZEBOX style.", 1 },
    //{ WS_TILED, "WS_TILED", "0x00000000L", "The window is an overlapped window.An overlapped window has a title bar and a border.Same as the WS_OVERLAPPED style.", 0 },
    { WS_TILEDWINDOW, "WS_TILEDWINDOW", "(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)", "The window is an overlapped window.Same as the WS_OVERLAPPEDWINDOW style.", 0 },
    { WS_VISIBLE, "WS_VISIBLE", "0x10000000L", "The window is initially visible.", 1 },
    // This style can be turned on and off by using the ShowWindow or SetWindowPos function.
    { WS_VSCROLL, "WS_VSCROLL", "0x00200000L", "The window has a vertical scroll bar.", 1 },
    // terminator
    { 0,          0,      0,             0,       0 }
};

PTSTR GetWS(DWORD style)
{
    PWINSTYLE pws = winStyles;
    PTSTR ps = GetNxtBuf();
    *ps = 0;
    DWORD dw = style;

    while (dw && pws->desc) {
        if (pws->flag & 0x01) {
            if (dw & pws->style) {
                if (*ps)
                    strcat(ps, "|");
                strcat(ps, pws->name);
                dw &= ~(pws->style);
            }
        }
        pws++;
    }
    return ps;
}

/*
typedef struct _RECT {
LONG left;
LONG top;
LONG right;
LONG bottom;
} RECT, *PRECT;

*/

PTSTR GetRECTStg(PRECT pr)
{
    PTSTR ps = GetNxtBuf();
    sprintf(ps, "%ld,%ld,%ld,%ld", pr->left, pr->top, pr->right, pr->bottom);
    return ps;
}


// eof - find-utils.cpp
