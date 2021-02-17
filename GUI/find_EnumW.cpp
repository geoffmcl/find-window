// find_EnumW.cpp

#include "find_window.h"

PWINLIST pWinList = NULL;

int gi_StartLine = 0;
int gi_LeftPosn = 0;
SIZE    g_TxtSize = { 6, 6 };
RECT    g_ClientRect;
int  gi_Height, gi_Width;
int  gi_OneCharWidth;
int  gi_ScreenLines;
size_t gi_ScreenChars; // using CN8 font, number of char across screen
int  gi_LineCount;
size_t gi_MaxLineLength = 0;
size_t gi_MaxPxThisScreen = 0;  // maximum pixel size this screen
size_t gi_MaxChThisScreen = 0;  // max chars wide on this screen

DWORD   gdw_MaxIndex = 0;

PTSTR MakeWindowName( HWND hwnd )
{
    PTSTR ptmp1 = GetNxtBuf();
    PTSTR ptmp2 = GetNxtBuf();
    PTSTR ptmp3 = GetNxtBuf();
    GetWindowText( hwnd, ptmp1, MX_ONE_BUF );
    GetClassName(  hwnd, ptmp2, MX_ONE_BUF );
    sprintf(ptmp3,"[%s]:[%s]", ptmp1, ptmp2);
    return ptmp3;
}

PTSTR Get_Window_Details( HWND hwnd )
{
    HWND hParent = GetParent(hwnd);
    DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    RECT rc;
    PTSTR ppar = "No Parent";
    PTSTR ptmp1 = MakeWindowName(hwnd);
    if (hParent)
        ppar = MakeWindowName(hParent);
    GetWindowRect(hwnd, &rc);
    PTSTR ptmp = GetRECTStg(&rc);
    PTSTR ptmp2 = GetNxtBuf();
    if (dwStyle & WS_POPUP)
        sprintf (ptmp2, "%08lX: WS_POPUP", dwStyle);
    else if (dwStyle & WS_CHILD)
        sprintf (ptmp2, "%08lX: WS_CHILD, ID: %lX", dwStyle, GetWindowLong(hwnd, GWL_ID));
    else if (dwStyle & WS_ICONIC)
        sprintf (ptmp2, "%08lX: WS_ICONIC", dwStyle);
    else
        sprintf (ptmp2, "%08lX: WS_OVERLAPPED", dwStyle);

    PTSTR pinfo = GetNxtBuf();
    sprintf(pinfo,"%s:%s:%s:%s", ppar, ptmp1, ptmp2, ptmp);
    return pinfo;
}

int Add_Stg_to_List( PTSTR pstr )
{
    PWINLIST pwl = new WINLIST;
    PWINLIST pn = pWinList;

    ZeroMemory(pwl,sizeof(WINLIST));

    pwl->hwnd = NULL;
    StringCbCopy(pwl->wintitle, MMX_WIN_TITLE, pstr);
    StringCchLength( pstr, MMX_WIN_TITLE, &pwl->tlen );
    sprtf("%s\n",pwl->wintitle);

    if (pwl->tlen > gi_MaxLineLength)
        gi_MaxLineLength = pwl->tlen;   // keep MAXIMUM length

    if (pn == NULL)
        pWinList = pwl;
    else {
        void *vp = pn->next;
        while(vp) {
            pn = (PWINLIST)vp;
            vp = pn->next;
        }
        pn->next = pwl;
    }
    return 1;
}
void add_Header_Line()
{
    static BOOL done_header = FALSE;
    if (!done_header) {
        PTSTR head = "  HWND     Proc    Thread   Information";
        Add_Stg_to_List(head);
        done_header = TRUE;
    }
}

int Add_Window_to_List( HWND hwnd )
{
    PTSTR pform = "%p:%08X:%08X: %s";
    PWINLIST pwl = new WINLIST;
    PWINLIST pn = pWinList;

    ZeroMemory(pwl,sizeof(WINLIST));

    pwl->hwnd = hwnd;   // store the WINDOW HANDLE
    // System get window thread process, and process
    pwl->dwThreadID = GetWindowThreadProcessId( hwnd, &pwl->dwProcID );
    // Find in process list, this process ID (PID)
    pwl->pProcInfo = FindProcID( hwnd, pwl->dwProcID, pwl->dwThreadID );

    PTSTR pinfo = Get_Window_Details(hwnd);

    if ( hwnd == g_hWnd ) {
        pwl->flag |= flg_SELF;
        pform = "%p:%08X:%08X:SELF: %s";
    }

    pwl->tlen = sprintf(pwl->wintitle,pform, hwnd,
        pwl->dwProcID, pwl->dwThreadID,
        pinfo);
#ifdef DO_SET_FIND_IN_ENUMERATION
    if ( get_find_count() ) {
        PTSTR pfind;
        if ((pfind = get_first_find()) != NULL) {
            do
            {
                if (InStri(pwl->wintitle,pfind)) {
                    pwl->flag |= flg_INFIND;
                    break;
                }
            } while ((pfind = get_next_find()) != NULL);
        }
    }
#endif // #ifdef DO_SET_FIND_IN_ENUMERATION

    sprtf("%s\n",pwl->wintitle);

    if (pwl->tlen > gi_MaxLineLength)
        gi_MaxLineLength = pwl->tlen;   // keep MAXIMUM length

    if (pn == NULL)
        pWinList = pwl;
    else {
        if (pn->hwnd == hwnd)
            return 0;

        void *vp = pn->next;
        while(vp) {
            pn = (PWINLIST)vp;
            if (pn->hwnd == hwnd)
                return 0;
            vp = pn->next;
        }
        pn->next = pwl;
    }
    return 1;
}

int Get_List_Count(void)
{
    int count = 0;
    PWINLIST pn = pWinList;
    while (pn) {
        count++;
        pn = (PWINLIST)pn->next;
    }
    return count;
}

void Delete_Line_List(void)
{
    int count = 0;
    PWINLIST pn = pWinList;
    while (pn) {
        void * vp = pn->next;
        delete pn;
        pn = (PWINLIST)vp;
    }
    pWinList = NULL;
}

extern VOID Do_Interation_Check(VOID);
VOID Index_By_ProcID(VOID)
{
    UINT    index = 1;
    PWINLIST pn1 = pWinList;
    PWINLIST pn2;
    UINT    count = 0;
    while (pn1) {
        count++;
        pn2 = (PWINLIST)pn1->next;
        if (pn1->index == 0) {
            pn1->index = index;
            while (pn2)
            {
                if (pn2->index == 0) {
                    if (pn1->dwProcID == pn2->dwProcID) {
                        pn2->index = index;
                    }
                }
                pn2 = (PWINLIST)pn2->next;
            }
            index++;    // bump the index
        }
        pn1 = (PWINLIST)pn1->next;
    }
    sprtf("Done %d indexes, for %d in list...\n", index, count);
    gdw_MaxIndex = index;
    Do_Interation_Check();

}

UINT    g_uWinCount = 0;
BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
   g_uWinCount += Add_Window_to_List( hwnd );
   return TRUE;
}

VOID Reset_Win_Finds(VOID)
{
    PWINLIST pwl = pWinList;
    while(pwl)
    {
        pwl->flag &= ~(flg_DONEIT | flg_INFIND);    // remove these flags
        pwl = (PWINLIST)pwl->next;
    }
}

int mark_Find(PTSTR pfind)
{
    int count = 0;
    PWINLIST pwl = pWinList;
    while(pwl)
    {
        pwl->flag &= ~(flg_DONEIT | flg_INFIND);    // remove these flags
        if (pfind) {
            if (bIgnCase) {
                if (InStri( pwl->wintitle, pfind )) {
                    pwl->flag |= flg_INFIND;
                    count++;
                }
            } else {
                if (InStr( pwl->wintitle, pfind )) {
                    pwl->flag |= flg_INFIND;
                    count++;
                }
            }
        }
        pwl = (PWINLIST)pwl->next;
    }
    return count;
}

VOID Set_Win_Finds(VOID)
{
    PWINLIST pwl = pWinList;
    if ( !pwl )
        return;
    if ( !get_find_count() )
        return;
    int count = 0;
    while (pwl)
    {
        PTSTR pfind, pNot;
        DWORD   flag;
        pwl->flag &= ~flg_DONEIT;   // clear the DONE flag
        if ((pfind = get_first_find()) != NULL) {
            do
            {
                if (InStri(pwl->wintitle,pfind)) {
                    flag = flg_INFIND;
                    if ((pNot = get_first_not_find()) != NULL) {
                        // maybe skip it, if also has a NOT
                        do
                        {
                            if (InStri(pwl->wintitle,pNot)) {
                                flag = 0;
                                break;
                            }
                        } while ((pNot = get_next_not_find()) != NULL);
                    }
                    if (flag) {
                        pwl->flag |= flag;
                        count++;
                    }
                    break;
                }
            } while ((pfind = get_next_find()) != NULL);
        }

        pwl = (PWINLIST)pwl->next;
    }
    if ( !count || !gb_Propagate )
        return; // all DONE

    // ok, have marked item to propogate
    pwl = pWinList;
    DWORD dwProcID;
    while ( pwl )
    {
        if ( !(pwl->flag & flg_DONEIT) ) {   // ok, no DONE flag
            if (pwl->flag & flg_INFIND) {
                // found flag in NOT done item
                dwProcID = pwl->dwProcID;
                PWINLIST pwl2 = pWinList;
                while(pwl2)
                {
                    if ( !(pwl2->flag & flg_DONEIT) ) {   // ok, no DONE flag
                        if (pwl2->dwProcID == dwProcID) {
                            pwl2->flag |= flg_INFIND | flg_DONEIT;
                        }
                    }
                    pwl2 = (PWINLIST)pwl2->next;
                }
            }
        }
        pwl = (PWINLIST)pwl->next;
    }
}

VOID Get_Window_List( HWND hWnd )
{
   // HWND FindWindow( LPCTSTR lpClassName,
   // LPCTSTR lpWindowName );
    int prev_count = Get_List_Count();
    if (prev_count < 2)
        add_Header_Line();
    EnumWindows( EnumWindowsProc, (LPARAM)0 );
    Index_By_ProcID();
    if (prev_count != Get_List_Count())
        InvalidateRect( hWnd, NULL, TRUE );
    Set_Win_Finds();
}

// eof - find_EnumW.cpp
