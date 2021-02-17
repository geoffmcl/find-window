// find_Utils.cpp

#include <sys/types.h>
#include <sys/stat.h>
#include "find_window.h"

static char gszAppData[264] = "\0";
#ifndef PATH_SEP
#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif
#endif // PATH_SEP

static char _s_strbufs[MX_ONE_BUF * MX_BUFFERS];
static int iNextBuf = 0;

PTSTR GetNxtBuf( void )
{
   iNextBuf++;
   if(iNextBuf >= MX_BUFFERS)
      iNextBuf = 0;
   return &_s_strbufs[MX_ONE_BUF * iNextBuf];
}

PTSTR GetRECTStg( PRECT pr )
{
   PTSTR ps = GetNxtBuf();
   sprintf(ps,"(%d,%d,%d,%d)", pr->left, pr->top, pr->right, pr->bottom );
   return ps;
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

/* -
Operating system Version number 
Windows Server "Longhorn" 6.0 
Windows Vista             6.0 
Windows Server 2003 R2    5.2 
Windows Server 2003       5.2 
Windows XP                5.1 
Windows 2000              5.0 
   - */

// To compile an application that uses this function, 
// define WIN32_WINNT as 0x0501 or later. For more information, 
// see Using the SDK Headers.
BOOL add_os_value(PTSTR pstr)
{
   OSVERSIONINFOEX osvi;
   SYSTEM_INFO si;
   BOOL bOsVersionInfoEx;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) {
          StringCbCat(EndBufW(pstr), MX_ONE_BUF, "ERROR: GetVersionEx FAILED!");
         return FALSE;
      }
   }


   switch (osvi.dwPlatformId)
   {
      // Test for the Windows NT product family.

      case VER_PLATFORM_WIN32_NT:

      // Test for the specific product.

      if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
      {
         if( osvi.wProductType == VER_NT_WORKSTATION )
             StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows Vista ");
         else StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Windows Server \"Longhorn\" " );
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
      {
         GetNativeSystemInfo(&si);

         if( GetSystemMetrics(SM_SERVERR2) )
            StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows Server 2003 \"R2\" ");
         else if( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 &&
                  osvi.wProductType == VER_NT_WORKSTATION )
         {
            StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows XP Professional x64 Edition ");
         }
         else StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows Server 2003, ");
      }

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
         StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows XP ");

      if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
         StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows 2000 ");

      if ( osvi.dwMajorVersion <= 4 )
         StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows NT ");

      // Test for specific product on Windows NT 4.0 SP6 and later.
      if( bOsVersionInfoEx )
      {
         // Test for the workstation type.
         if ( osvi.wProductType == VER_NT_WORKSTATION )
         {
            if( osvi.dwMajorVersion == 4 )
               StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Workstation 4.0 " );
            else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
               StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Home Edition " );
            else StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Professional " );
         }
            
         // Test for the server type.
         else if ( osvi.wProductType == VER_NT_SERVER || 
                   osvi.wProductType == VER_NT_DOMAIN_CONTROLLER )
         {
            if(osvi.dwMajorVersion==5 && osvi.dwMinorVersion==2)
            {
               if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
               {
                   if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                      StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Datacenter Edition for Itanium-based Systems" );
                   else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                      StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Enterprise Edition for Itanium-based Systems" );
               }

               else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
               {
                   if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                      StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Datacenter x64 Edition " );
                   else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                      StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Enterprise x64 Edition " );
                   else StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Standard x64 Edition " );
               }

               else
               {
                   if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                      StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Datacenter Edition " );
                   else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                      StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Enterprise Edition " );
                   else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
                      StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Web Edition " );
                   else StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Standard Edition " );
               }
            }
            else if(osvi.dwMajorVersion==5 && osvi.dwMinorVersion==0)
            {
               if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                  StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Datacenter Server " );
               else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                  StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Advanced Server " );
               else StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Server " );
            }
            else  // Windows NT 4.0 
            {
               if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                  StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Server 4.0, Enterprise Edition " );
               else StringCbCat(EndBufW(pstr), MX_ONE_BUF,  "Server 4.0 " );
            }
         }
      }
      // Test for specific product on Windows NT 4.0 SP5 and earlier
      else  
      {
         HKEY hKey;
         char szProductType[BUFSIZE];
         DWORD dwBufLen=BUFSIZE;
         LONG lRet;

         lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
            "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
            0, KEY_QUERY_VALUE, &hKey );
         if( lRet != ERROR_SUCCESS ) {
             StringCbCat(EndBufW(pstr), MX_ONE_BUF, "ERROR: RegOpenKeyEx( HKEY_LOCAL_MACHINE FAILED!");
            return FALSE;
         }

         lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
            (LPBYTE) szProductType, &dwBufLen);
         RegCloseKey( hKey );

         if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) ) {
            StringCbCat(EndBufW(pstr), MX_ONE_BUF, "ERROR: RegQueryValueEx( hKey, \"ProductType\" FAILED!");
            return FALSE;
         }

         if ( lstrcmpi( "WINNT", szProductType) == 0 )
            StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Workstation " );
         if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
            StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Server " );
         if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
            StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Advanced Server " );
         StringCchPrintf(EndBufW(pstr), MX_ONE_BUF, "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
      }

      // Display service pack (if any) and build number.

      if( osvi.dwMajorVersion == 4 && 
          lstrcmpi( osvi.szCSDVersion, "Service Pack 6" ) == 0 )
      { 
         HKEY hKey;
         LONG lRet;

         // Test for SP6 versus SP6a.
         lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
  "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009",
            0, KEY_QUERY_VALUE, &hKey );
         if( lRet == ERROR_SUCCESS )
            StringCchPrintf(EndBufW(pstr), MX_ONE_BUF, "Service Pack 6a (Build %d)", 
            osvi.dwBuildNumber & 0xFFFF );         
         else // Windows NT 4.0 prior to SP6a
         {
            StringCchPrintf(EndBufW(pstr), MX_ONE_BUF, "%s (Build %d)",
               osvi.szCSDVersion,
               osvi.dwBuildNumber & 0xFFFF);
         }

         RegCloseKey( hKey );
      }
      else // not Windows NT 4.0 
      {
         StringCchPrintf(EndBufW(pstr), MX_ONE_BUF, "%s (Build %d)",
            osvi.szCSDVersion,
            osvi.dwBuildNumber & 0xFFFF);
      }

      break;

      // Test for the Windows Me/98/95.
      case VER_PLATFORM_WIN32_WINDOWS:

      if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
      {
          StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows 95 ");
          if (osvi.szCSDVersion[1]=='C' || osvi.szCSDVersion[1]=='B')
             printf("OSR2 " );
      } 

      if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
      {
          StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows 98 ");
          if ( osvi.szCSDVersion[1]=='A' || osvi.szCSDVersion[1]=='B')
             printf("SE " );
      } 

      if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
      {
          StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Windows Millennium Edition");
      } 
      break;

      case VER_PLATFORM_WIN32s:

      StringCbCat(EndBufW(pstr), MX_ONE_BUF, "Microsoft Win32s");
      break;
   }
   return TRUE; 
}

PTSTR Get_OS_Version_string(VOID)
{
    PTSTR pstr = GetNxtBuf();

    *pstr = 0;

    if ( add_os_value(pstr) )
        return pstr;

    StringCbCat(pstr, MX_ONE_BUF, "WARNING: Get_os_Version FAILED!");
    return pstr;
}

BOOL CenterWindow(HWND hwndChild, HWND hwndParent)
{
    RECT    rcChild, rcParent;
    int     cxChild, cyChild, cxParent, cyParent;
    int     cxScreen, cyScreen, xNew, yNew;
    HDC     hdc;

    // Get the Height and Width of the child window
    GetWindowRect(hwndChild, &rcChild);
    cxChild = rcChild.right - rcChild.left;
    cyChild = rcChild.bottom - rcChild.top;

    // Get the Height and Width of the parent window
    GetWindowRect(hwndParent, &rcParent);
    cxParent = rcParent.right - rcParent.left;
    cyParent = rcParent.bottom - rcParent.top;

    // Get the display limits
    hdc = GetDC(hwndChild);
    cxScreen = GetDeviceCaps(hdc, HORZRES);
    cyScreen = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(hwndChild, hdc);

    // Calculate new X position, then adjust for screen
    xNew = rcParent.left + ((cxParent - cxChild) / 2);
    if (xNew < 0) {
         xNew = 0;
    } else if ((xNew + cxChild) > cxScreen) {
        xNew = cxScreen - cxChild;
    }

    // Calculate new Y position, then adjust for screen
    yNew = rcParent.top  + ((cyParent - cyChild) / 2);
    if (yNew < 0) {
        yNew = 0;
    } else if ((yNew + cyChild) > cyScreen) {
        yNew = cyScreen - cyChild;
    }

    // Set it, and return
    return SetWindowPos(hwndChild,
                        NULL,
                        xNew, yNew,
                        0, 0,
                        SWP_NOSIZE | SWP_NOZORDER);
}

// FIX20210216 - get %APPDATA%/find-window
int create_dir(const char* pd)
{
    int iret = 1;
    int res;
    if (is_file_or_directory(pd) != MDT_DIR) {
        size_t i, j, len = strlen(pd);
        char ps, ch, pc = 0;
        char tmp[260];
        j = 0;
        iret = 0;
        tmp[0] = 0;
#ifdef _WIN32
        ps = '\\';
#else
        ps = '/'
#endif

            for (i = 0; i < len; i++) {
                ch = pd[i];
                if ((ch == '\\') || (ch == '/')) {
                    ch = ps;
                    if ((pc == 0) || (pc == ':')) {
                        tmp[j++] = ch;
                        tmp[j] = 0;
                    }
                    else {
                        tmp[j] = 0;
                        if (is_file_or_directory(tmp) != MDT_DIR) {
                            res = mkdir(tmp);
                            if (res != 0) {
                                return 0; // FAILED
                            }
                            if (is_file_or_directory(tmp) != MDT_DIR) {
                                return 0; // FAILED
                            }
                        }
                        tmp[j++] = ch;
                        tmp[j] = 0;
                    }
                }
                else {
                    tmp[j++] = ch;
                    tmp[j] = 0;
                }
                pc = ch;
            } // for lenght of path
        if ((pc == '\\') || (pc == '/')) {
            iret = 1; // success
        }
        else {
            if (j && pc) {
                tmp[j] = 0;
                if (is_file_or_directory(tmp) == MDT_DIR) {
                    iret = 1; // success
                }
                else {
                    res = mkdir(tmp);
                    if (res != 0) {
                        return 0; // FAILED
                    }
                    if (is_file_or_directory(tmp) != MDT_DIR) {
                        return 0; // FAILED
                    }
                    iret = 1; // success
                }
            }
        }
    }
    return iret;
}

void GetAppData(PTSTR lpini)
{
    char* pd;
    if (!gszAppData[0]) {
        //pd = getenv("PROGRAMDATA"); // UGH - do not have permissions -  how to GET permissions
        //if (!pd) {
        //	pd = getenv("ALLUSERSPROFILE");
        //}
        pd = getenv("APPDATA");
        if (!pd) {
            pd = getenv("LOCALAPPDATA");
        }
        if (pd) {
            strcpy(gszAppData, pd);
            strcat(gszAppData, PATH_SEP);
            strcat(gszAppData, "find-window");
            if (!create_dir(gszAppData)) {
                gszAppData[0] = 0;
            }
            else {
                strcat(gszAppData, PATH_SEP);
            }
        }
    }

    if (gszAppData[0]) {
        strcpy(lpini, gszAppData);
    }
    else {
        GetModulePath(lpini);    // does GetModuleFileName( NULL, lpini, 256 );
    }
}

#ifdef _WIN32
#define M_IS_DIR _S_IFDIR
#else // !_MSC_VER
#define M_IS_DIR S_IFDIR
#endif

static struct stat buf;

int is_file_or_directory(const char* path)
{
    if (!path)
        return MDT_NONE;
    if (stat(path, &buf) == 0)
    {
        if (buf.st_mode & M_IS_DIR)
            return MDT_DIR;
        else
            return MDT_FILE;
    }
    return MDT_NONE;
}

size_t get_last_file_size() { return buf.st_size; }

// eof - find_Utils.cpp
