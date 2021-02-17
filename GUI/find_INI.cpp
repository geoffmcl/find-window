
// cursorINI.c
// this is public domain software - praise me, if ok, just don't blame me!
#include "find_window.h"

//#define  sprtf printf

#define  it_None        0     // end of list
#define  it_Version     1
#define  it_String      2
#define  it_Int         3
#define  it_Bool        4
#define  it_WinSize     5     // special WINDOWPLACEMENT
#define  it_Rect        6
#define  it_Color       7
#define  it_SList       8     // simple list
#define  it_DWord       it_Int   // virtually the same in WIN32

typedef struct	tagINILST {	/* i */
	LPTSTR	i_Sect;
	LPTSTR	i_Item;
	DWORD	   i_Type;
	LPTSTR	i_Deft;
	PBOOL	   i_Chg;
	PVOID	   i_Void;
	DWORD	   i_Res1;
} INILST, * PINILST;

static	TCHAR	g_szDefIni[] = "find_window.ini";
TCHAR	g_szIni[264];   // the INI file to read/write

TCHAR szBlk[] = "\0";

// [Sections]
TCHAR szVer[] = "Version";
TCHAR szOpt[] = "Options";
TCHAR szOut[] = "OutLine";
TCHAR szExp[] = "Expanded";
TCHAR szWin[] = "Window";

// section [Version]
TCHAR szDt[] = "Date";
TCHAR szCVer[] = VER_DATE;    // control re-setting of INI file
BOOL  bChgAll = FALSE;        // if SET, whole file is cleaned and redone

// section [Options]

// section [OutLine]
TCHAR szGOut[] = "OutSaved";
BOOL  gbGotOut = FALSE;
WINDOWPLACEMENT   g_sWP;
BOOL  bChgOut = FALSE;

TCHAR szShow[] = "ShowCmd";
TCHAR szMaxX[] = "MaxX";
TCHAR szMaxY[] = "MaxY";
TCHAR szMinX[] = "MinX";
TCHAR szMinY[] = "MinY";
TCHAR szLeft[] = "NormLeft";
TCHAR szTop[]  = "NormTop";
TCHAR szRite[] = "NormRight";
TCHAR szBot[]  = "NormBottom";

// section [Window]
TCHAR szZm[] = "Zoomed";
TCHAR szIc[] = "Iconic";
TCHAR szSz[] = "Size";
TCHAR szWT[] = "Title";
TCHAR szWC[] = "Class";

// Title=
TCHAR ini_Title[CCH_RGBUF];
BOOL  bChgTit = FALSE;
// Class=
TCHAR ini_Class[CCH_RGBUF];
BOOL  bChgCls = FALSE;

TCHAR szHL[] = "HighLightRGB";
extern COLORREF gcr_HighLight;
BOOL  bChgHL = FALSE;

TCHAR szPI[] = "SortPerIndex";
// extern BOOL gb_PaintPerIndex;
BOOL  bChgPI = FALSE;

TCHAR szPG[] = "Propagate";
// extern BOOL gb_Propagate;
BOOL  bChgPG = FALSE;

TCHAR szFND[] = "Last_Find";
TCHAR g_szFind[1024] = {0};
BOOL bChgFND = FALSE;

static TCHAR szCASE[] = "Ignore_cAsE";
BOOL bIgnCase = FALSE;
BOOL bChgCase = FALSE;


TCHAR gszTmpBuf[1024];

VOID  ReadINI( PTSTR lpini );
VOID  WriteINI( LPTSTR lpini, BOOL bNoReset );

BOOL  ChangedWP( WINDOWPLACEMENT * pw1, WINDOWPLACEMENT * pw2 )
{
   BOOL  bChg = FALSE;
   if( ( pw1->length != sizeof(WINDOWPLACEMENT) ) ||
       ( pw2->length != sizeof(WINDOWPLACEMENT) ) ||
       ( pw1->showCmd != pw2->showCmd ) ||
       ( pw1->ptMaxPosition.x != pw2->ptMaxPosition.x ) ||
       ( pw1->ptMaxPosition.y != pw2->ptMaxPosition.y ) ||
       ( !EqualRect( &pw1->rcNormalPosition, &pw2->rcNormalPosition ) ) )
   {
      bChg = TRUE;
   }
   return bChg;
}

VOID UpdateWP( HWND hwnd )
{
   WINDOWPLACEMENT wp;
   wp.length = sizeof(WINDOWPLACEMENT);
   if( GetWindowPlacement(hwnd,&wp) )
   {
      if( ChangedWP( &wp, &g_sWP ) ) {
         memcpy( &g_sWP, &wp, sizeof(WINDOWPLACEMENT) );
         g_sWP.length = sizeof(WINDOWPLACEMENT);
         bChgOut = TRUE;
      }
   }
}


BOOL  Chk4Debug( LPTSTR lpd )
{
   BOOL     bret = FALSE;
   LPTSTR ptmp = &gszTmpBuf[0];
   LPTSTR   p;
   DWORD  dwi;

   strcpy(ptmp, lpd);
   dwi = (DWORD)strlen(ptmp);
   if(dwi)
   {
      dwi--;
      if(ptmp[dwi] == '\\')
      {
         ptmp[dwi] = 0;
         p = strrchr(ptmp, '\\');
         if(p)
         {
            p++;
            if( strcmpi(p, "DEBUG") == 0 )
            {
               *p = 0;
               strcpy(lpd,ptmp);    // use this
               bret = TRUE;
            }
         }
      }
   }
   return bret;
}

VOID  GetModulePath( LPTSTR lpb )
{
   LPTSTR   p;
   GetModuleFileName( NULL, lpb, 256 );
   p = strrchr( lpb, '\\' );
   if( p )
      p++;
   else
      p = lpb;
   *p = 0;
#ifndef  NDEBUG
   Chk4Debug( lpb );
#endif   // !NDEBUG

}


INILST  sIniLst[] = {
   { szVer, szDt,   it_Version,          szCVer,        &bChgAll, 0, 0 },  // CHANGE ALL!
   { szOut, szGOut, it_WinSize,   (PTSTR)&g_sWP,        &bChgOut, (PVOID)&gbGotOut, 0 },

   // section [Window]
   { szWin, szWT,   it_String,        ini_Title,        &bChgTit, 0, 0 },
   { szWin, szWC,   it_String,        ini_Class,        &bChgCls, 0, 0 },

   // section [Options]
   { szOpt, szHL,   it_Color,     (PTSTR)&gcr_HighLight,&bChgHL,  0, 0 },
   { szOpt, szPI,   it_Bool,   (PTSTR)&gb_PaintPerIndex,&bChgPI,  0, 0 },
   { szOpt, szPG,   it_Bool,   (PTSTR)&gb_Propagate,    &bChgPG,  0, 0 },
   { szOpt, szFND,  it_String,             g_szFind,    &bChgFND, 0, 0 },
   { szOpt, szCASE, it_Bool,        (PTSTR)&bIgnCase,   &bChgCase,0, 0 },


   // ===================================================================
   { 0,     0,      it_None,    0,     0,        0, 0 }  // END OF TABLE
   // ===================================================================
};

BOOL  SetChgAll( BOOL bChg )
{
   BOOL  bRet = bChgAll;
   bChgAll = bChg;
   return bRet;
}
///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : SetINIFile
// Return type: VOID 
// Argument   : LPTSTR lpini
// Description: Establish the INI file name. In current work directory
//              when DEBUG, else placed in the RUNTIME directory.
// FIX20010522 - ALWAYS place it in the RUNTIME folder!!!
// FIX20210216 - ALWAYS place it in the %APPDATA%/find_file folder!
///////////////////////////////////////////////////////////////////////////////
VOID  SetINIFile( LPTSTR lpini )
{
   *lpini = 0;
   // GetModulePath( lpini );    // does   GetModuleFileName( NULL, lpini, 256 );
   GetAppData(lpini);
   strcat(lpini, g_szDefIni);
}

#define	GetStg( a, b )	\
	GetPrivateProfileString( a, b, &szBlk[0], lpb, 256, lpini )


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsYes
// Return type: BOOL 
// Argument   : LPTSTR lpb
// Description: Return TRUE if the string given is "Yes" OR "On"
///////////////////////////////////////////////////////////////////////////////
BOOL  IsYes( LPTSTR lpb )
{
   BOOL  bRet = FALSE;
   if( ( strcmpi(lpb, "YES") == 0 ) ||
       ( strcmpi(lpb, "ON" ) == 0 ) )
       bRet = TRUE;
   return bRet;
}
BOOL  IsNo( LPTSTR lpb )
{
   BOOL  bRet = FALSE;
   if( ( strcmpi(lpb, "NO" ) == 0 ) ||
       ( strcmpi(lpb, "OFF") == 0 ) )
       bRet = TRUE;
   return bRet;
}

BOOL  ValidShowCmd( UINT ui )
{
   BOOL  bRet = FALSE;
   if( ( ui == SW_HIDE ) ||   //Hides the window and activates another window. 
       ( ui == SW_MAXIMIZE ) ||  //Maximizes the specified window. 
       ( ui == SW_MINIMIZE ) ||  //Minimizes the specified window and activates the next top-level window in the Z order. 
       ( ui == SW_RESTORE ) ||   //Activates and displays the window. If the window is minimized or maximized, the system restores it to its original size and position. An application should specify this flag when restoring a minimized window. 
       ( ui == SW_SHOW ) || //Activates the window and displays it in its current size and position.  
       ( ui == SW_SHOWMAXIMIZED ) || //Activates the window and displays it as a maximized window. 
       ( ui == SW_SHOWMINIMIZED ) || //Activates the window and displays it as a minimized window. 
       ( ui == SW_SHOWMINNOACTIVE ) ||  //Displays the window as a minimized window. 
       ( ui == SW_SHOWNA ) || //Displays the window in its current size and position. 
       ( ui == SW_SHOWNOACTIVATE ) ||  //Displays a window in its most recent size and position. 
       ( ui == SW_SHOWNORMAL ) )
       bRet = TRUE;
   return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : GotWP
// Return type: BOOL 
// Arguments  : LPTSTR pSect
//            : LPTSTR pDef
//            : LPTSTR lpb
//            : LPTSTR lpini
// Description: Read in a special BLOCK of window palcement item from
//              the INI file. This is in its own [section].
///////////////////////////////////////////////////////////////////////////////
BOOL  GotWP( LPTSTR pSect, LPTSTR pDef, LPTSTR lpb, LPTSTR lpini )
{
   BOOL  bRet = FALSE;
   WINDOWPLACEMENT   wp;
   WINDOWPLACEMENT * pwp = (WINDOWPLACEMENT *)pDef;
   if( !pwp )
      return FALSE;

   *lpb = 0;
   GetStg( pSect, szShow ); // = "ShowCmd";
   if( *lpb == 0 )
      return FALSE;
   wp.showCmd = atoi(lpb);
   if( !ValidShowCmd( wp.showCmd ) )
      return FALSE;

   *lpb = 0;
   GetStg( pSect, szMaxX );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMaxPosition.x = atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szMaxY );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMaxPosition.y = atoi(lpb);

   *lpb = 0;
   GetStg( pSect, szMinX );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMinPosition.x = atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szMinY );
   if( *lpb == 0 )
      return FALSE;
   wp.ptMinPosition.y = atoi(lpb);

   *lpb = 0;
   GetStg( pSect, szLeft );   // = "NormLeft";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.left = atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szTop ); // = "NormTop";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.top = atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szRite );   // = "NormRight";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.right = atoi(lpb);
   *lpb = 0;
   GetStg( pSect, szBot ); //  = "NormBottom";
   if( *lpb == 0 )
      return FALSE;
   wp.rcNormalPosition.bottom = atoi(lpb);

   wp.flags = 0;
   wp.length = sizeof(WINDOWPLACEMENT);

   memcpy( pwp, &wp, sizeof(WINDOWPLACEMENT) );
   bRet = TRUE;
   return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : IsRectOk
// Return type: BOOL 
// Argument   : PRECT pr
// Description: A rough verification that a RECTANGLE 'looks' OK
//              NOTE: Thre must be SOME value, and they must all
// be positive (or zero, but not all).
///////////////////////////////////////////////////////////////////////////////
BOOL  IsRectOk( PRECT pr )
{
   BOOL  bRet = FALSE;
   if( pr->left || pr->top || pr->right || pr->bottom )
   {
      // good start - some value is NOT zero
      // here I am ONLY accepting POSITIVE values
      if( ( pr->left >= 0 ) &&
          ( pr->top >= 0 ) &&
          ( pr->right >= 0 ) &&
          ( pr->bottom >= 0 ) )
      {
         bRet = TRUE;
      }
      else if(( pr->right  > 0 ) &&
              ( pr->bottom > 0 ) )
      {
         // this is a POSSIBLE candidate,
         // but limit neg x,y to say 5 pixels
         if(( pr->left > -5 ) &&
            ( pr->top  > -5 ) )
         {
            bRet = TRUE;
         }
      }
   }
   return bRet;
}

PTSTR Get_INI_File(VOID)
{
   PTSTR lpini = &g_szIni[0];
   SetINIFile(lpini);
   return lpini;
}

VOID  find_ReadINI( VOID )
{
   LPTSTR lpini = &g_szIni[0];
   SetINIFile( lpini );
   ReadINI( lpini );
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : ReadINI
// Return type: VOID 
// Argument   : LPTSTR lpini - name of file to use
// Description: Read the "preferences" from an INI file
///////////////////////////////////////////////////////////////////////////////
VOID  ReadINI( LPTSTR lpini )
{
   PINILST  plst = &sIniLst[0];
   LPTSTR   lpb = &gszTmpBuf[0];
   DWORD    dwt;
   LPTSTR   pSect, pItem, pDef;
   PBOOL    pb, pbd;
   PINT     pi;
   INT      i, icnt;
   PRECT    pr, pr1;
   icnt = 0;
   while( ( dwt = plst->i_Type ) != it_None )
   {
      pSect = plst->i_Sect;   // pointer to [section] name
      pItem = plst->i_Item;   // pointer to itme in section
      pDef  = plst->i_Deft;   // pointer to destination
      pb    = plst->i_Chg;    // pointer to CHANGE flag
      *lpb = 0;
      if( dwt == it_SList ) {
          sprtf("\nERROR: 'it_SList' NOT SUPPORTED!\n");
          printf( "\nERROR: 'it_SList' NOT SUPPORTED!\n" );
         exit(1);
      } else {
         GetStg( pSect, pItem );
      }
      if( *lpb )
      {
         switch(dwt)
         {
         case it_Version:
            if( strcmp( pDef, lpb ) )
               *pb = TRUE;
            break;
         case it_String:
            if( strcmp( pDef, lpb ) )
               strcpy( pDef, lpb );
            break;
         case it_Int:   // also doubles as a DWORD in WIN32
            pi = (PINT)pDef;
            i = atoi(lpb);
            *pi = i;
            break;
         case it_Bool:
            pbd = (PBOOL)pDef;
            if( IsYes(lpb) )
               *pbd = TRUE;
            else if( IsNo(lpb) )
               *pbd = FALSE;
            else
               *pb = TRUE;
            break;
         case it_WinSize:     // special WINDOWPLACEMENT
            if( ( IsYes(lpb) ) &&
                ( GotWP( pSect, pDef, lpb, lpini ) ) )
            {
               // only if SAVED is yes, AND then success
               pb = (PBOOL) plst->i_Void;
               *pb = TRUE; // set that we have a (valid!) WINDOWPLACEMENT
            }
            else
               *pb = TRUE;
            break;
         case it_Rect:
            pr = (PRECT) &lpb[ strlen(lpb) + 2 ];
            pr->left = 0;
            pr->top = 0;
            pr->right = 0;
            pr->bottom = 0;
            if( ( sscanf(lpb, "%d,%d,%d,%d", &pr->left, &pr->top, &pr->right, &pr->bottom ) == 4 ) &&
                ( IsRectOk( pr ) ) )
            {
               pr1 = (PRECT)pDef;
               pr1->left = pr->left;
               pr1->top  = pr->top;
               pr1->right = pr->right;
               pr1->bottom = pr->bottom;
               pb = (PBOOL) plst->i_Void;
               if(pb)
                  *pb = TRUE;
            }
            break;
         case it_Color:
            pr = (PRECT) &lpb[ strlen(lpb) + 2 ];
            pr->left = 0;
            pr->top = 0;
            pr->right = 0;
            pr->bottom = 0;
            if( sscanf(lpb, "%d,%d,%d", &pr->left, &pr->top, &pr->right ) == 3 )
            {
               COLORREF * pcr = (COLORREF *)pDef;
               *pcr = RGB( pr->left, pr->top, pr->right );
            }
            break;
         case it_SList:
            // it has all been done
            break;
         }
      }
      else
      {
         *pb = TRUE;    // can only SET change
         if( icnt == 0 )
            sprtf( "ADVICE: bChgAll has been set!" MEOR );

      }

      plst++;
      icnt++;
   }

}  // end ReadINI( lpini )




#define  WI( a, b )\
   {  sprintf(lpb, "%d", b );\
      WritePrivateProfileString(pSect, a, lpb, lpini ); }


VOID  find_WriteINI( VOID )
{
   LPTSTR   lpini = &g_szIni[0];
   SetINIFile( lpini );
   WriteINI( lpini, FALSE );  // ensure FULL reset of 'changed' flags
}


///////////////////////////////////////////////////////////////////////////////
// FUNCTION   : WriteINI
// Return type: VOID 
// Argument   : LPTSTR   lpini
//            : BOOL bNoReset
// Description: Save the "preferences" to an INI file
//              
///////////////////////////////////////////////////////////////////////////////
VOID  WriteINI( LPTSTR lpini, BOOL bNoReset )
{
   LPTSTR   lpb   = &gszTmpBuf[0];
   PINILST  plst  = &sIniLst[0];
   DWORD    dwt;
   LPTSTR   pSect, pItem, pDef;
   BOOL     ball = *(plst->i_Chg);
   BOOL     bchg;
   PINT     pint;
   PBOOL    pb;
   WINDOWPLACEMENT * pwp;
   PRECT    pr;

   if( ball )
   {
      // clear down the current INI file
      while( ( dwt = plst->i_Type ) != it_None )
      {
         pSect = plst->i_Sect;
         WritePrivateProfileString( pSect,		// Section
            NULL,    // Res.Word
            NULL,    // String to write
            lpini );	// File Name
         plst++;
      }
   }

   plst = &sIniLst[0];  // start of LIST

   while( ( dwt = plst->i_Type ) != it_None )
   {
      pSect = plst->i_Sect;
      pItem = plst->i_Item;
      pDef  = plst->i_Deft;
      bchg  = *(plst->i_Chg);
      if( ball || bchg )
      {
         *lpb = 0;
         switch(dwt)
         {
         case it_Version:
            strcpy(lpb, pDef);
            break;
         case it_String:
            strcpy(lpb, pDef);
            break;
         case it_Int:
            pint = (PINT)pDef;
            sprintf(lpb, "%d", *pint );
            break;
         case it_Bool:
            pb = (PBOOL)pDef;
            if( *pb )
               strcpy(lpb, "Yes");
            else
               strcpy(lpb, "No");
            break;
         case it_WinSize:     // special WINDOWPLACEMENT
            pb = (PBOOL)plst->i_Void;
            pwp = (WINDOWPLACEMENT *)pDef;
            if( ( pwp->length == sizeof(WINDOWPLACEMENT) ) &&
                ( ValidShowCmd( pwp->showCmd ) ) )
            {
               WI( szShow, pwp->showCmd );
               WI( szMaxX, pwp->ptMaxPosition.x );
               WI( szMaxY, pwp->ptMaxPosition.y );
               WI( szMinX, pwp->ptMinPosition.x );
               WI( szMinY, pwp->ptMinPosition.y );
               WI( szLeft, pwp->rcNormalPosition.left );
               WI( szTop,  pwp->rcNormalPosition.top  );
               WI( szRite, pwp->rcNormalPosition.right);
               WI( szBot,  pwp->rcNormalPosition.bottom);
               strcpy(lpb, "Yes");
            }
            else
               strcpy(lpb, "No");
            break;
         case it_Rect:
            pr = (PRECT)pDef;

            sprintf(lpb, "%d,%d,%d,%d", pr->left, pr->top, pr->right, pr->bottom );
            break;
         case it_Color:
            {
               COLORREF * pcr = (COLORREF *)pDef;
               sprintf(lpb, "%d,%d,%d",
                  GetRValue(*pcr), GetGValue(*pcr), GetBValue(*pcr) );
            }
            break;
         case it_SList:
            {
               printf( "ERROR: NO SUPPORTED!" MEOR );
               exit(1);
            }
            break;
         }

         if( !bNoReset )
            *(plst->i_Chg) = FALSE;

         if( *lpb )
         {
            WritePrivateProfileString(
						pSect,		// Section
						pItem,		// Res.Word
						lpb,		// String to write
						lpini );	// File Name
         }
      }
      plst++;
   }

}


// eof - cursorINI.c
