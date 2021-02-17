
// cursorABT.cpp : About BOX
#include "find_window.h"
#include <direct.h> // for _getcwd()

VOID abt_WM_INITDIALOG( HWND hDlg )
{
    char buf[264];
    char* cp = buf;
    char* ep1, * ep2;
	// DWORD dwData = GetVersion();
    OSVERSIONINFO  vi;

    vi.szCSDVersion[0] = 0;
    vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx( &vi );

    sprintf(cp, "Find Window - Version %s %s", APP_VERS, VER_DATE );
    SetDlgItemText( hDlg, IDC_INFO3, cp );

    sprintf(cp, "Copyright (C) %s Geoff R. McLane", COPY_YEAR);
    SetDlgItemText( hDlg, IDC_INFO4, cp );

    sprintf(cp, "Compiled on %s, at %s",
      __DATE__,
      __TIME__ );
    SetDlgItemText( hDlg, IDC_INFO1, cp );

    ep1 = getenv( "COMPUTERNAME" );
    ep2 = getenv( "USERNAME" );
    sprintf(cp, "Run in %s, by %s",
      ep1 ? ep1 : "<unknown>",
      ep2 ? ep2 : "<unknown>" );
    SetDlgItemText( hDlg, IDC_INFO2, cp );
   
    sprintf( cp, "OS Version %d.%d.%d ",
      vi.dwMajorVersion,
      vi.dwMinorVersion,
      vi.dwBuildNumber );
    if( vi.szCSDVersion[0] )
      strcat( cp, &vi.szCSDVersion[0]);
    SetDlgItemText( hDlg, IDC_INFO5, cp );

     PTSTR ptmp = GetNxtBuf();
     if ( _fullpath(ptmp, Get_INI_File(), MX_ONE_BUF) == NULL )
        StringCbCopy(ptmp, MX_ONE_BUF, Get_INI_File());
     sprintf( cp, "INI: %s", ptmp);
     SetDlgItemText( hDlg, IDC_INFO6, cp );

     if ( _fullpath(ptmp, get_log_file(), MX_ONE_BUF) == NULL )
        StringCbCopy(ptmp, MX_ONE_BUF, get_log_file());
     sprintf( cp, "LOG: %s", ptmp);
     SetDlgItemText( hDlg, IDC_INFO7, cp );

     _getcwd(ptmp,MX_ONE_BUF);
     sprintf( cp, "CWD: %s", ptmp);
     SetDlgItemText( hDlg, IDC_INFO8, cp );

     if (g_hWnd) // only if we have a global handle
        CenterWindow( hDlg, g_hWnd );
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
      abt_WM_INITDIALOG( hDlg );
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

VOID  Do_IDM_ABOUT( HINSTANCE hInst, HWND hWnd )
{
   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
}

#define  CDB(a,b) CheckDlgButton( hDlg, a, ( b ? BST_CHECKED : BST_UNCHECKED ) )
#define ICDB(a) ((IsDlgButtonChecked( hDlg, a ) == BST_CHECKED) ? TRUE : FALSE)
VOID find_WM_INITDIALOG( HWND hDlg )
{
    if (strlen(g_szFind))
        SetDlgItemText( hDlg, IDC_EDIT1, g_szFind );
    CDB( IDC_CHECK1, bIgnCase );
    CenterWindow( hDlg, g_hWnd );
}

static TCHAR last_find[264] = "\0";
UINT find_GetFind( HWND hDlg )
{
    PTSTR pstr = last_find;
    *pstr = 0;
    UINT res = GetDlgItemText( hDlg, IDC_EDIT1, pstr, 256 );
    if (res && strlen(pstr)) {
        if ( strcmp(g_szFind,pstr) ) {
            strcpy(g_szFind,pstr);
             bChgFND = TRUE;
        }
    }
    BOOL ichk = ICDB( IDC_CHECK1 );
    if (ichk) {
        if (!bIgnCase ) {
            bIgnCase = TRUE;
            bChgCase = TRUE;
        }
    } else {
        if (bIgnCase ) {
            bIgnCase = FALSE;
            bChgCase = TRUE;
        }

    }
    return res;

}

// Message handler for Find box.
LRESULT CALLBACK Find(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
      find_WM_INITDIALOG( hDlg );
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
            find_GetFind(hDlg);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		} else if (LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
        }
		break;
	}
	return FALSE;
}

VOID Do_ID_OPTIONS_FIND( HINSTANCE hInst, HWND hWnd )
{
    PTSTR pstr = last_find;
    INT_PTR res = DialogBox(hInst, (LPCSTR)IDD_DIALOG1, hWnd, (DLGPROC)Find);
    size_t len = strlen(pstr);
    if ((res == IDOK) && len) {
        mark_Find(pstr);
    } else {
        mark_Find(0);
    }
    InvalidateRect( hWnd, NULL, TRUE );
}

// eof - cursorABT.cpp
