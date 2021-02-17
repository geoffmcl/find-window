// find_Scroll.cpp

#include "find_window.h"

LRESULT Do_WM_VSCROLL(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    WORD type = LOWORD(wParam);
    WORD wPos = HIWORD(wParam);
    RECT rect = g_ClientRect;   // Client area.
    UINT nHorzOrVert = SB_VERT;
    int yBar = GetScrollPos( hWnd, nHorzOrVert );
    int nMin, nMax;
    int dy = 0;
    int doupdate = 0;
    int diff = 0;
    if (gi_ScreenLines < gi_LineCount)
        diff = gi_LineCount - gi_ScreenLines;
	GetScrollRange( hWnd, nHorzOrVert, &nMin, &nMax );
    switch(type)
    {
    case SB_LINEUP: // = SB_LINELEFT 0
        // Scrolls one line up.
        dy = -1;
        if (gi_StartLine)
            doupdate = 1;
#ifdef USE_PREV_CODE
        if (gi_StartLine) {
            gi_StartLine += dy;
        } else {
            doupdate = 0;
        }
#endif
        break;
    case SB_LINEDOWN: // = SB_LINERIGHT 1
        // Scrolls one line down.
        dy = 1;
        doupdate = 1;
        if (gi_LineCount > gi_ScreenLines) {
            // we have MORE lines, than one screen, so
#ifdef USE_PREV_CODE
            if (gi_StartLine < diff) {
                gi_StartLine += dy; // bump start line, but
            } else {
                doupdate = 0;
            }
#endif
        } else {
            dy = 0; // we have NO NEED to SCROLL
            doupdate = 0;
        }
        break;
    case SB_PAGEUP: // = SB_PAGELEFT 2
        // Scrolls one page up.
        dy = -gi_ScreenLines;
        doupdate = 1;
#ifdef USE_PREV_CODE
        gi_StartLine += dy;
        if (gi_StartLine < 0)
            gi_StartLine = 0;
#endif
        break;
    case SB_PAGEDOWN: // SB_PAGERIGHT 3
        // Scrolls one page down.
        dy = gi_ScreenLines;
        doupdate = 1;
#ifdef USE_PREV_CODE
        gi_StartLine += dy;
        if (gi_StartLine > diff)
            gi_StartLine = diff;
#endif
        break;
    case SB_THUMBPOSITION: // 4
   		dy = wPos - yBar;
        doupdate = 1;
        break;
    case SB_THUMBTRACK:     // 5
   		dy = wPos - yBar;
        doupdate = 1;
        break;
    }

	if( doupdate ) {
        yBar += dy;
#ifndef USE_PREV_CODE
        gi_StartLine += dy;
        if (gi_StartLine > diff)
            gi_StartLine = diff;
        if (gi_StartLine < 0)
            gi_StartLine = 0;
#endif
        if( yBar < nMin )
            yBar = nMin;
        if( yBar > nMax )
            yBar = nMax;
		SetScrollPos( hWnd, nHorzOrVert, yBar, TRUE );
        //UpdateWindow( hWnd );
        InvalidateRect(hWnd,NULL,TRUE); // and repaint
	}

    return lRes;
}

VOID Go_To_Bottom( HWND hWnd )
{
    int     diff = 0;
	UINT    nHorzOrVert = SB_VERT;
    int     yBar, nMin, nMax;
    if (gi_ScreenLines < gi_LineCount)
        diff = gi_LineCount - gi_ScreenLines;
	yBar = GetScrollPos( hWnd, nHorzOrVert );
	GetScrollRange( hWnd, nHorzOrVert, &nMin, &nMax );
    yBar = nMax;
    SetScrollPos( hWnd, nHorzOrVert, yBar, TRUE );
    gi_StartLine = diff;
    if (diff) 
        InvalidateRect(hWnd,NULL,TRUE); // and repaint
}

VOID Go_To_Top( HWND hWnd )
{
	UINT    nHorzOrVert = SB_VERT;
    int     yBar, nMin, nMax;
	yBar = GetScrollPos( hWnd, nHorzOrVert );
	GetScrollRange( hWnd, nHorzOrVert, &nMin, &nMax );
    yBar = nMin;
    SetScrollPos( hWnd, nHorzOrVert, yBar, TRUE );
    if (gi_StartLine) {
        gi_StartLine = 0;
        InvalidateRect(hWnd,NULL,TRUE); // and repaint
    }
}

// case WM_HSCROLL:
// what to base this ON - pixels, or per character width
// Have variables - int  gi_Height, gi_Width;
// gi_OneCharWidth, gi_ScreenChars; // using CN8 font, number of char across screen
// gi_MaxLineLength
// maxchars = gi_MaxLineLength, and if this is greater than
// the maximum char that can be painted, then set the scroll
// range to the difference, then one scroll left or right
// is the width of ONE character...
//  if (gi_MaxLineLength > gi_ScreenChars)
//		cxRange = gi_MaxLineLength - gi_ScreenChars; // as scroll character width
//  SetScrollRange( hWnd, SB_HORZ, 0, cxRange, TRUE );
// int gi_LeftPosn = 0 is in PIXELS, when painting
// Added gi_MaxThisScreen = (int)len; // keep MAXIMUM line for THIS screen
LRESULT Do_WM_HSCROLL(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    WORD type = LOWORD(wParam);
    WORD wPos = HIWORD(wParam);
    RECT    rect;   // Client area.
	int		nHorzOrVert; // Doing the horizontal or vertical?
    int     cClient;
    int     nMin, nMax, xBar;
	int		dx;			// How much to move.
    SIZE    sz;
    int     doupdate = 0;
    int     charcntdiff = 0;

    rect = g_ClientRect;
    sz   = g_TxtSize;
    // GetClientRect( hWnd, &rect );
    nHorzOrVert = SB_HORZ;// Doing the horizontal or vertical?
    cClient    = rect.right - rect.left;	// Get WIDTH

    // if possible screen char LT max this screen
    if (gi_ScreenChars < gi_MaxChThisScreen)
        charcntdiff = (int)(gi_MaxChThisScreen - gi_ScreenChars);
	xBar = GetScrollPos( hWnd, nHorzOrVert );   // = 0 to max_line_chars
	GetScrollRange( hWnd, nHorzOrVert, &nMin, &nMax );
    dx = 0;
    switch(type)
    {
    // Scrolls left by one unit.
    case SB_LINELEFT:   // = SB_LINEUP =  0
        // scroll to LEFT one char
        if (gi_LeftPosn) {
            dx = -1;
#ifdef USE_PREV_CODE
            if ( gi_LeftPosn )
                gi_LeftPosn += dx;
            if ( gi_LeftPosn < 0 )
                gi_LeftPosn = 0;
#endif
            doupdate = 1;
        }
        break;
    // Scrolls right by one unit.
    case SB_LINERIGHT: // = SB_LINEDOWN =  1
        // scroll to RIGHT one char
        // only IF there is a line on this screen 
        // greater than supportable line length
        if (charcntdiff) {
            dx = 1;
            doupdate = 1;
            // gi_MaxThisScreen = keep MAXIMUM line length for THIS screen
            // check have not moved ALL off the screen
            if ((gi_LeftPosn + dx - 1) > charcntdiff)
                doupdate = 0;
#ifdef USE_PREV_CODE
            if ((gi_LeftPosn + dx - 1) > charcntdiff)
                doupdate = 0;
            else
                gi_LeftPosn += dx;
#endif
            // slider is 0 to max_line_chars
            // but we want to move it as if 0 to max_this_screen
            // so left_pos / max_this == xBar + dx / charcntdiff(max_max)
            //dx = (int)( ((double)charcntdiff * 
            //    ((double)gi_LeftPosn / (double)gi_MaxThisScreen )) - (double)xBar );
        }
        break;
    case SB_PAGELEFT: // = SB_PAGEUP = 2
        // Scrolls left by the width of the window.
        dx = -(int)gi_ScreenChars;
        if (gi_LeftPosn) {
#ifdef USE_PREV_CODE
            gi_LeftPosn += dx;
            if (gi_LeftPosn < 0)
                gi_LeftPosn = 0;
#endif
            doupdate = 1;
        }
        break;
    case SB_PAGERIGHT: // = SB_PAGEDOWN = 3
        // Scrolls right by the width of the window.
        if (charcntdiff) {
            dx = (int)gi_ScreenChars;
            doupdate = 1;
#ifdef USE_PREV_CODE
            if ((gi_LeftPosn + dx - 1) > charcntdiff)
                doupdate = 0;
            else
                gi_LeftPosn += dx;
#endif
        }
        break;
    case SB_THUMBPOSITION: // 4
        // The user has dragged the scroll box (thumb) and released the mouse button.
        // The high-order word indicates the position of the scroll box at 
        // the end of the drag operation.
   		dx = wPos - xBar;
        doupdate = 1;
        break;
    case SB_THUMBTRACK:     // 5
        // The user is dragging the scroll box. This message is sent
        // repeatedly until the user releases the mouse button. 
        // The high-order word indicates the position that the scroll box
        // has been dragged to.
   		dx = wPos - xBar;
        doupdate = 1;
        break;
    case SB_TOP:            // = SB_LEFT 6
        break;
    case SB_BOTTOM:         // = SB_RIGHT 7
        break;
    case SB_ENDSCROLL:   // 8
        break;
    }
    if(doupdate) {
#ifndef USE_PREV_CODE
        gi_LeftPosn += dx;
        if ((gi_LeftPosn - 1) > charcntdiff)
            gi_LeftPosn = charcntdiff + 1;
        if ( gi_LeftPosn < 0 )
            gi_LeftPosn = 0;
#endif
		xBar += dx;
		if( xBar < nMin ) {
			dx  -= xBar - nMin;
			xBar = nMin;
		}
		if( xBar > nMax ) {
			dx  -= xBar - nMax;
			xBar = nMax;
		}
		SetScrollPos( hWnd, nHorzOrVert, xBar, TRUE );
        //UpdateWindow( hWnd );
        InvalidateRect(hWnd,NULL,TRUE); // and repaint
    }
    return lRes;
}

/* ===============================
cbSize - Specifies the size, in bytes, of this structure. The caller 
         must set this to sizeof(SCROLLINFO). 
fMask - Specifies the scroll bar parameters to set or retrieve. This member 
        can be a combination of the following values: 
  SIF_ALL - Combination of SIF_PAGE, SIF_POS, SIF_RANGE, and SIF_TRACKPOS.
  SIF_DISABLENOSCROLL - This value is used only when setting a scroll bar's
      parameters. If the scroll bar's new parameters make the scroll bar 
      unnecessary, disable the scroll bar instead of removing it. 
  SIF_PAGE - The nPage member contains the page size for a proportional scroll bar.
  SIF_POS - The nPos member contains the scroll box position, which is 
            not updated while the user drags the scroll box.
  SIF_RANGE - The nMin and nMax members contain the minimum and maximum 
              values for the scrolling range.
  SIF_TRACKPOS - The nTrackPos member contains the current position of the
                 scroll box while the user is dragging it.
nMin - Specifies the minimum scrolling position. 
nMax - Specifies the maximum scrolling position. 
nPage - Specifies the page size. A scroll bar uses this value to 
        determine the appropriate size of the proportional scroll box. 
nPos - Specifies the position of the scroll box. 
nTrackPos - Specifies the immediate position of a scroll box that the 
            user is dragging. An application can retrieve this value while 
            processing the SB_THUMBTRACK request code.
            An application cannot set the immediate scroll position; 
            the SetScrollInfo function ignores this member. 
   =============================== */
#undef USE_OLD_SCROLL_RANGE
BOOL m_SetScrollRange( HWND hWnd, int nBar, int nMinPos, int nMaxPos, BOOL bRedraw )
{
    BOOL bRet = TRUE;
#ifdef USE_OLD_SCROLL_RANGE
    bRet = SetScrollRange( hWnd, nBar, nMinPos, nMaxPos, bRedraw );
#else
    SCROLLINFO si; 
    si.cbSize = sizeof(si);
    GetScrollInfo( hWnd, nBar, &si );
    si.fMask  = SIF_RANGE;
    si.nMin   = nMinPos; 
    si.nMax   = nMaxPos;
    if (nMinPos == nMaxPos) {
        si.nMax += 1;
        si.fMask |= SIF_DISABLENOSCROLL;
    }
    //si.nPage  = xClient / xChar; 
    SetScrollInfo(hWnd, nBar, &si, bRedraw); 
#endif
    sprtf("Set %s scroll, min %d, max %d\n",
        ((nBar == SB_HORZ) ? "SB_HORZ" : "SB_VERT"),
        nMinPos, nMaxPos );
    return bRet;
}

// eof - find_Scroll.cpp
