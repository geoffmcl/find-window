// find_Paint.cpp

#include "find_window.h"

BOOL gb_PaintPerIndex = TRUE;

COLORREF gcr_Blue = RGB(0xe0,0xe0,0xff);
COLORREF gcr_Self = RGB(0x80,0xff,0x80);
COLORREF gcr_Gray = RGB(0xF0,0xF0,0xF0);
COLORREF gcr_HighLight = RGB(0xf0,0xf0,0xff);

HFONT g_hfCN8 = NULL;
// TEXTMETRIC g_sFF8;
LOGFONT  g_slfCN = {
   -11, 0, 0, 0, 0,
      0, 0, 0,
      ANSI_CHARSET,
      0, 0,
      DEFAULT_QUALITY,
      FIXED_PITCH | FF_MODERN,
      _T("Courier New")
};

HFONT  Get_CN8Font( HWND hWnd )
{
    if (g_hfCN8)
        return g_hfCN8;
    HDC hdc = GetDC(hWnd);
    if(hdc) {
        PLOGFONT plf = &g_slfCN;   // get pointer to LOGFONT structure
        int iy = GetDeviceCaps(hdc, LOGPIXELSY);
        plf->lfHeight = -MulDiv(8, iy, 72);
        g_hfCN8 = CreateFontIndirect(plf);
        ReleaseDC(hWnd,hdc); // release in WM_CREATE
    }
    return g_hfCN8;
}

VOID    DeleteObjects(VOID)
{
    if (g_hfCN8)
        DeleteObject(g_hfCN8);
    g_hfCN8 = NULL;
}

#define  MAX_BGN_COLORS 8

COLORREF sColors[MAX_BGN_COLORS] = {
   RGB(255,255,255),    // white
   RGB(255,255,0  ),    // yellow
   RGB(255,0,255  ),
   RGB(0,255,255  ),
   RGB(128,128,128),    // white
   RGB(128,128,0  ),    // yellow
   RGB(128,0,128  ),
   RGB(0,128,128  )
};

COLORREF gcr_CT = RGB(255,192,255);

#define  CX_MARGIN   2
#define  CY_MARGIN   2

PTSTR GetTmStg( SYSTEMTIME * pst )
{
   PTSTR ps = GetNxtBuf();
   sprintf(ps, "%02d:%02d", pst->wHour, pst->wMinute);
   return ps;
}


VOID  Minutes_2_HDC( HWND hwnd, HDC hdc )
{
   RECT  rc;
   if( GetClientRect( hwnd, &rc ) ) {
      POINT pt;
      SYSTEMTIME st;
      char tb[256];
      INT   i;
      SIZE  sz;
      pt.x = rc.right - 40;
      pt.y = rc.bottom - 15;
      GetLocalTime( &st );
      sprintf(tb, "%s", GetTmStg(&st));
      sprintf( EndBuf(tb), " %d mins, %d wins",
          g_minsElapsed,
          Get_List_Count() );
      i = (INT)strlen(tb);
      if ( GetTextExtentPoint(
         hdc,        // handle to DC
         tb,         // text string
         i,          // number of characters in string
         &sz ) )     // string size
      {
         pt.x = rc.right - sz.cx - CX_MARGIN;
         pt.y = rc.bottom - sz.cy - CY_MARGIN;
      }
      TextOut( hdc, pt.x, pt.y, tb, i );
      g_secCount = st.wSecond;
   }
}

VOID  Paint_Minutes( HWND hwnd )
{
   HDC hdc = GetDC(hwnd);
   if(hdc) {
      Minutes_2_HDC( hwnd, hdc );
      ReleaseDC( hwnd, hdc );
   }
}

PWINLIST pNextWL = NULL;
PWINLIST Get_First_Per_Index( UINT index )
{
    PWINLIST pn = pWinList;
    while (pn) {
        if (pn->index == index) {
            pNextWL = (PWINLIST)pn->next;
            return pn;
        }
        pn = (PWINLIST)pn->next;
    }
    pNextWL = NULL;
    return NULL;
}

PWINLIST Get_Next_Per_Index( UINT index )
{
    PWINLIST pn = pNextWL;
    while (pn) {
        if (pn->index == index) {
            pNextWL = (PWINLIST)pn->next;
            return pn;
        }
        pn = (PWINLIST)pn->next;
    }
    pNextWL = NULL;
    return NULL;
}

DWORD GetThreadID(PTHREADLIST ptl, DWORD thread)
{
    DWORD count = 0;
    while (ptl)
    {
        if (count == thread)
            return ptl->s_te32.th32ThreadID;
        ptl = (PTHREADLIST)ptl->next;
        count++;
    }
    return -1;
}

VOID Do_Interation_Check(VOID)
{
    DWORD index = 0;
    PWINLIST pn = NULL;
    UINT count = 0;
    DWORD PID = 0;
    DWORD prevPID = -1;
    DWORD thread = 0;
    DWORD ThreadID = 0;
    DWORD ThreadCnt = 0;
    PTHREADLIST ptl = NULL;
    PALLPROCESSLIST papl = NULL;
    HWND    hwnd;
    PTSTR pName = NULL;
    gi_LineCount = Get_List_Count();
    if (gi_LineCount) {
        sprtf("WindowID:ProcessI:ThreadID: Info\n");
        while (( pn = Get_First_Per_Index(index) ) == NULL ) {
            index++;
            if (index > gdw_MaxIndex)
                break;
        }
        ThreadCnt = 0;
        while (pn) {
            hwnd = pn->hwnd;
            PID = pn->dwProcID;
            papl = pn->pProcInfo;
            ptl = NULL;
            pName = "<Unknown>";
            if (papl) {
                ptl = (PTHREADLIST)papl->pth32;
                //ThreadCnt = papl->pe32.cntThreads;
                pName = papl->pe32.szExeFile;
            }
            if (PID != prevPID) {
                prevPID = PID;
                thread = 0;
                ThreadCnt = 0;
                if (papl)
                    ThreadCnt = papl->pe32.cntThreads;
                sprtf("%p: Index %d (%s)\n", hwnd, index, pName);
            } else {
                thread++;
            }
            ThreadID = GetThreadID(ptl, thread);
            count++;
            sprtf("%p:%08X:%08X: %d of %d Threads (%d)\n",
                hwnd,
                PID,
                ThreadID,
                thread + 1,
                ThreadCnt,
                count );
            pn = Get_Next_Per_Index(index);    // get NEXT or NULL
            if ( !pn && ( count < (UINT)gi_LineCount) ) {
                index++;
                if (index > gdw_MaxIndex)
                    break;
                pn = Get_First_Per_Index(index);
            }
        }
    }
}

VOID Paint_Win_List_per_Index( HWND hWnd, HDC hdc )
{
    PWINLIST pn = NULL;
    SIZE    sz;
    int       count = 0;
    int paint_count = 0;
    long max_width = 0;
    UINT    index = 0;
    COLORREF crold = 0;
    int line_count = 0;

    gi_LineCount = Get_List_Count();
    if (gi_LineCount) {
        RECT rc;
        HFONT hf = Get_CN8Font( hWnd );
        HFONT hfold = NULL;

        if (hf)
            hfold = (HFONT)SelectObject(hdc,hf);

        Get_Text_Size( hWnd, hdc );
        rc = g_ClientRect;
        rc.top = 0;
        sz = g_TxtSize;
        gi_MaxPxThisScreen = 0;
        gi_MaxChThisScreen = 0;
        while (( pn = Get_First_Per_Index(index) ) == NULL ) {
            index++;
            if (index > gdw_MaxIndex)
                break;
        }

        while (pn) {
            count++;
            if (count > gi_StartLine) {
                if (!pn->dntsize) {
                    GetTextExtentPoint32(hdc, pn->wintitle, (int)pn->tlen, &pn->tsz);
                    pn->dntsize = 1;
                }

                if (pn->flag & flg_INFIND)
                    crold = SetBkColor( hdc, gcr_Blue );
                else if (pn->flag & flg_SELF)
                    crold = SetBkColor( hdc, gcr_Self );
                else if (line_count & 1)
                    crold = SetBkColor( hdc, gcr_Gray );

                if (gi_LeftPosn > 0) {
                    // must indent on this/these lines
                    if ((int)pn->tlen > gi_LeftPosn) {
                        TextOut( hdc, 0, rc.top, &pn->wintitle[gi_LeftPosn], (int)(pn->tlen - gi_LeftPosn));
                    } else {
                        // nothing to paint on this line
                    }
                } else {
                    // paint from beginning of line
                    TextOut(hdc, 0, rc.top, pn->wintitle, (int)pn->tlen);
                }

                if (crold) 
                    SetBkColor( hdc, crold );
                crold = 0;

                paint_count++;
                rc.top += sz.cy;
                if ( pn->tsz.cy > (int)gi_MaxPxThisScreen )
                    gi_MaxPxThisScreen = pn->tsz.cy;
                if ( pn->tlen > gi_MaxChThisScreen )
                    gi_MaxChThisScreen = pn->tlen;

                if (rc.top > rc.bottom)
                    break;
                line_count++;
            }
            pn = Get_Next_Per_Index(index);    // get NEXT or NULL
            if ( !pn && ( count < gi_LineCount) ) {
                index++;
                if (index > gdw_MaxIndex)
                    break;
                pn = Get_First_Per_Index(index);
            }
        }

        if (hfold)
            hf = (HFONT)SelectObject(hdc,hfold);
    }

    UINT cxRange = 0;
    // maxchars = gi_MaxLineLength, and if this is greater than
    // the maximum char that can be painted, then set the scroll
    // range to the difference, then one scroll left or right
    // is the width of ONE character...
    if (gi_MaxChThisScreen > gi_ScreenChars) {
		cxRange = (int)(gi_MaxChThisScreen - gi_ScreenChars); // as scroll character width
    }
    m_SetScrollRange( hWnd, SB_HORZ, 0, cxRange, TRUE );

    UINT cyRange = 0;
    if ( gi_ScreenLines < gi_LineCount ) {
		//cyRange = maxheight - cyWindow - 1 + GetSystemMetrics( SM_CYHSCROLL );
        cyRange = gi_LineCount - gi_ScreenLines;
    }
    // if there are 100 line available, and only 20 per screen, this this
    // range is set to 0 - 80
    m_SetScrollRange( hWnd, SB_VERT, 0, cyRange, TRUE );

}

VOID Paint_Win_List( HWND hWnd, HDC hdc )
{
    PWINLIST pn = pWinList;
    SIZE    sz;
    int       count = 0;
    int paint_count = 0;
    long max_width = 0;
    int line_count = 0;
    COLORREF crold = 0;
    //size_t maxchars = 0;
    gi_LineCount = Get_List_Count();
    if (pn && gi_LineCount) {
        RECT rc;
        HFONT hf = Get_CN8Font( hWnd );
        HFONT hfold = NULL;

        if (hf)
            hfold = (HFONT)SelectObject(hdc,hf);

        Get_Text_Size( hWnd, hdc );
        rc = g_ClientRect;
        rc.top = 0;
        sz = g_TxtSize;
        gi_MaxPxThisScreen = 0;
        gi_MaxChThisScreen = 0;
        while (pn) {
            count++;
            if (count > gi_StartLine) {
                if (!pn->dntsize) {
                    GetTextExtentPoint32(hdc, pn->wintitle, (int)pn->tlen, &pn->tsz);
                    pn->dntsize = 1;
                }

                if (pn->flag & flg_INFIND)
                    crold = SetBkColor( hdc, gcr_Blue );
                else if (pn->flag & flg_SELF)
                    crold = SetBkColor( hdc, gcr_Self );
                else if (line_count & 1)
                    crold = SetBkColor( hdc, gcr_Gray );

                if (gi_LeftPosn > 0) {
                    // must indent on this/these lines
                    if ((int)pn->tlen > gi_LeftPosn) {
                        TextOut( hdc, 0, rc.top, &pn->wintitle[gi_LeftPosn], (int)(pn->tlen - gi_LeftPosn));
                    } else {
                        // nothing to paint on this line
                    }
                } else {
                    // paint from beginning of line
                    TextOut(hdc, 0, rc.top, pn->wintitle, (int)pn->tlen);
                }

                paint_count++;
                rc.top += sz.cy;
                if ( pn->tsz.cy > (int)gi_MaxPxThisScreen )
                    gi_MaxPxThisScreen = pn->tsz.cy;
                if ( pn->tlen > gi_MaxChThisScreen )
                    gi_MaxChThisScreen = pn->tlen;

                if (rc.top > rc.bottom)
                    break;
                if (crold) 
                    SetBkColor( hdc, crold );
                crold = 0;
                line_count++;
            }
            pn = (PWINLIST)pn->next;    // get NEXT or NULL
        }

        if (hfold)
            hf = (HFONT)SelectObject(hdc,hfold);
    }

    UINT cxRange = 0;
    // maxchars = gi_MaxLineLength, and if this is greater than
    // the maximum char that can be painted, then set the scroll
    // range to the difference, then one scroll left or right
    // is the width of ONE character...
    if (gi_MaxChThisScreen > gi_ScreenChars) {
		cxRange = (int)(gi_MaxChThisScreen - gi_ScreenChars); // as scroll character width
    }
    m_SetScrollRange( hWnd, SB_HORZ, 0, cxRange, TRUE );

    UINT cyRange = 0;
    if ( gi_ScreenLines < gi_LineCount ) {
		//cyRange = maxheight - cyWindow - 1 + GetSystemMetrics( SM_CYHSCROLL );
        cyRange = gi_LineCount - gi_ScreenLines;
    }
    // if there are 100 line available, and only 20 per screen, this this
    // range is set to 0 - 80
    m_SetScrollRange( hWnd, SB_VERT, 0, cyRange, TRUE );

}

VOID  Do_WM_PAINT( HWND hWnd )
{
    PAINTSTRUCT ps;
	HDC hdc;

    hdc = BeginPaint(hWnd, &ps);

    if (gb_PaintPerIndex)
        Paint_Win_List_per_Index( hWnd, hdc );
    else
        Paint_Win_List( hWnd, hdc );

    Minutes_2_HDC( hWnd, hdc );

    EndPaint(hWnd, &ps);
}

#define MMX_CENTER_MSG  512
TCHAR   szCenterMessage[MMX_CENTER_MSG] = { _T("\0") };
VOID Paint_Center_Message_HDC( HDC hdc, HWND hWnd )
{
    if (hdc && szCenterMessage[0]) {
        PTSTR pstr = szCenterMessage;
        HFONT hf,hfold;
        //SIZE sz;
        RECT    rc;
        COLORREF crold;
        UINT form = DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOCLIP | DT_NOPREFIX;
        int len = lstrlen(pstr);
        hf = NULL; // Get_CN8Font( hWnd );
        if (hf)
            hfold = (HFONT)SelectObject(hdc,hf);
        else
            hfold = NULL;
        crold = SetBkColor(hdc, gcr_CT);
        GetClientRect(hWnd,&rc);
        //GetTextExtentPoint32(hdc,pstr,len,&sz);
        // no let Windows do this...
        DrawText(hdc, pstr, len, &rc, form );
        SetBkColor(hdc, crold);
        if (hfold)
            SelectObject(hdc,hfold);
    }
}

VOID Paint_Center_Message( HWND hWnd, PTSTR pstr )
{
    HDC hdc = GetDC(hWnd);
    if (hdc) {
        StringCchCopy(szCenterMessage,MMX_CENTER_MSG,pstr);
        Paint_Center_Message_HDC( hdc, hWnd );
        ReleaseDC(hWnd,hdc);
    }
}
static TEXTMETRIC _s_tm; 
static int xChar;       // horizontal scrolling unit 
static int yChar;       // vertical scrolling unit 
static int xUpper;      // average width of uppercase letters 

VOID Get_Text_Size(HWND hWnd, HDC hdc)
{
    TEXTMETRIC * ptm = &_s_tm;
    PTSTR pstr = _T("ABCdef");
    int len = (int)lstrlen(pstr);
    GetTextExtentPoint32(hdc,pstr,(int)len,&g_TxtSize);
    GetClientRect(hWnd,&g_ClientRect);
    // Extract font dimensions from the text metrics. 
    GetTextMetrics (hdc, ptm); 
    // set some values
    xChar = ptm->tmAveCharWidth; 
    xUpper = (ptm->tmPitchAndFamily & 1 ? 3 : 2) * xChar/2; 
    yChar = ptm->tmHeight + ptm->tmExternalLeading;

    gi_Height = g_ClientRect.bottom - g_ClientRect.top;
    gi_Width  = g_ClientRect.right  - g_ClientRect.left;
    gi_ScreenLines = gi_Height / g_TxtSize.cy;  // paintable lines on screen
    //gi_ScreenChars = (int) ((double)gi_Width  / (double)g_TxtSize.cx / (double)len);
    gi_OneCharWidth = (g_TxtSize.cx / len);
    gi_ScreenChars = ( gi_Width  / gi_OneCharWidth);
}

// eof - find_Paint.cpp
