// cursorINI.h
#ifndef  _cursorINI_h_
#define  _cursorINI_h_

extern VOID find_WriteINI( VOID );
extern VOID find_ReadINI( VOID );
extern PTSTR Get_INI_File(VOID);

extern VOID UpdateWP( HWND hwnd );
// restore LAST size
extern BOOL  gbGotOut;
extern WINDOWPLACEMENT g_sWP;

// Title=
extern TCHAR ini_Title[];  // CCH_RGBUF
extern BOOL  bChgTit;
// Class=
extern TCHAR ini_Class[];
extern BOOL  bChgCls;

extern BOOL  IsYes( LPTSTR lpb );
extern BOOL  IsNo( LPTSTR lpb );

//TCHAR szPI[] = "SortPerIndex";
// extern BOOL gb_PaintPerIndex;
extern BOOL  bChgPI;

//TCHAR szPG[] = "Propagate";
// extern BOOL gb_Propagate;
extern BOOL  bChgPG;


extern TCHAR g_szFind[]; // last find
extern BOOL bChgFND;

// static TCHAR szCASE[] = "Ignore_cAsE";
extern BOOL bIgnCase;
extern BOOL bChgCase;

extern VOID  GetModulePath(LPTSTR);


#endif // #ifndef  _cursorINI_h_
// eof = cursorINI.h

