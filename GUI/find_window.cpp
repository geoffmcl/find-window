// find_Window.cpp : Defines the entry point for the application.
//
#include "find_window.h"
#define MMAX_STRING 260

// Global Variables:
HWND        g_hWnd = NULL;
HINSTANCE   g_hInst;								// current instance
TCHAR szTitle[MMAX_STRING] = { _T("Find Window") };	// The title bar text
TCHAR szWindowClass[MMAX_STRING] = { _T("FIND_WINDOW") };	// the main window class name
UINT_PTR  g_uiTimerID = 0;  // SetTimer( hWnd, TIMER_ID, TIMER_TIC, NULL );
BOOL      g_AddVersion = FALSE;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

    // Initialize global strings
    find_ReadINI();

    if( ParseArgs(__argc, __argv) )
        return FALSE;

    MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CURSOR);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    find_WriteINI();
    Delete_Line_List();
    Kill_Process_List();
    Kill_ALL_Process_List();
    close_log_file(); // shut the output

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_CURSOR);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_CURSOR;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    UINT uStyle = WS_OVERLAPPEDWINDOW | WS_HSCROLL  | WS_VSCROLL;

    g_hInst = hInstance; // Store instance handle in our global variable

    g_hWnd = CreateWindow(szWindowClass, szTitle, uStyle,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
      return FALSE;

   g_uiTimerID = SetTimer( g_hWnd, TIMER_ID, TIMER_TIC, NULL );
   if( !g_uiTimerID ) {
      MessageBox(g_hWnd, 
         "ERROR: Failed to get TIMER ...",
         "CRITCAL ERROR",
         MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL );
      DestroyWindow( g_hWnd );
      return FALSE;
   }

   PTSTR pstr = GetNxtBuf();
   StringCchPrintf(pstr, MX_ONE_BUF, "%p: in %s", g_hWnd, Get_OS_Version_string() );
   if (g_AddVersion)
       Add_Stg_to_List( pstr );
   else
       sprtf("%s\n",pstr);

   if( gbGotOut )
      SetWindowPlacement(g_hWnd,&g_sWP);
   else
      ShowWindow(g_hWnd, nCmdShow);

   UpdateWindow(g_hWnd);

   return TRUE;
}

//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//  PURPOSE:  Processes messages for the main window, like
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
    LRESULT lRes = 0;

	switch (message) 
	{
    case WM_INITMENUPOPUP:
        if ( Do_WM_INITMENUPOPUP( hWnd, wParam, lParam ) )
    		lRes = DefWindowProc(hWnd, message, wParam, lParam);
        break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
            Do_IDM_ABOUT( g_hInst, hWnd );
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
        case IDM_SORTTOGGLE:
            if (gb_PaintPerIndex)
                gb_PaintPerIndex = FALSE;
            else
                gb_PaintPerIndex = TRUE;
            InvalidateRect(hWnd,NULL,TRUE);
            bChgPI = TRUE;
            break;
        case IDM_PROPTOGGLE:
            if (gb_Propagate)
                gb_Propagate = FALSE;
            else
                gb_Propagate = TRUE;
            Reset_Win_Finds();
            Set_Win_Finds();
            InvalidateRect(hWnd,NULL,TRUE);
            bChgPG = TRUE;
            break;
        case ID_OPTIONS_FIND:
            Do_ID_OPTIONS_FIND( g_hInst, hWnd );
            break;

		default:
			lRes = DefWindowProc(hWnd, message, wParam, lParam);
            break;
		}
		break;
	case WM_PAINT:
        Do_WM_PAINT( hWnd );
		break;
	case WM_DESTROY:
        UpdateWP( hWnd );
		PostQuitMessage(0);
		break;
   case WM_TIMER:
       Do_WM_TIMER( hWnd );
       break;
    case WM_MOVE:
        UpdateWP( hWnd );
        break;
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
    case WM_MOUSEWHEEL:
        Do_WM_MOUSEWHEEL( hWnd, wParam, lParam );
        break;
#endif   // #if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
   case WM_HSCROLL:
       lRes = Do_WM_HSCROLL(hWnd, wParam, lParam);
       break;
   case WM_VSCROLL:
       lRes = Do_WM_VSCROLL(hWnd, wParam, lParam);
       break;
    case WM_KEYDOWN:
        Do_WM_KEYDOWN( hWnd, wParam, lParam );
        break;
    case WM_KEYUP:
        Do_WM_KEYUP( hWnd, wParam, lParam );
        break;
	case WM_RBUTTONDOWN:
        Do_WM_RBUTTONDOWN( hWnd, wParam, lParam );
        break;
	default:
        lRes = DefWindowProc(hWnd, message, wParam, lParam);
        break;
	}
	return lRes;
}


// eof - find_window.cpp

