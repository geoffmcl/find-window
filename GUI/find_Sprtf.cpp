// find_Sprtf.cpp

#include "find_window.h"

// sprtf.cxx
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif // #ifndef _CRT_SECURE_NO_DEPRECATE
#pragma warning( disable:4996 )

#define  MXIO     256
char def_log_name[] = "find_window.txt";
char logfile[264] = "\0";

FILE * outfile = NULL;
static int addsystime = 0;
static int addstdout = 0;
static int addflush = 1;

int   add_std_out( int val )
{
   int i = addstdout;
   addstdout = val;
   return i;
}

int   add_sys_time( int val )
{
   int   i = addsystime;
   addsystime = val;
   return i;
}

char* get_log_file(void)
{
    if (logfile[0] == 0)
    {
        GetAppData(logfile);
        strcat(logfile, def_log_name);
    }
    return logfile;
}


int   open_log_file( void )
{
    char* log = get_log_file();
    outfile = fopen(log, "wb");
   if( outfile == 0 ) {
      outfile = (FILE *)-1;
      printf("WARNING: Failed to open log file [%s] ...\n", log);
      return 0;   /* failed */
   }
   return 1; /* success */
}

void close_log_file( void )
{
   if( VFP(outfile) ) {
      fclose(outfile);
   }
   outfile = NULL;
}

void   set_log_file( char * nf )
{
   char * log = get_log_file();
   if ( nf && *nf && strcmpi(nf,log) ) {
      close_log_file(); // remove any previous
      strcpy(log,nf); // set new name
      open_log_file();  // and open it ... anything previous written is 'lost'
   }
}

static void oi( char * psin )
{
   char * ps = psin;
   int len = (int)strlen(ps);
   if(len) {
      if( outfile == 0 ) {
         open_log_file();
      }
      if( addsystime ) {
         SYSTEMTIME st;
         GetLocalTime( &st );
         ps = (char *)GetNxtBuf();
         len = sprintf( ps, "%2d:%02d:%02d - %s", st.wHour & 0xffff,
            st.wMinute & 0xffff, st.wSecond & 0xffff, psin );
      }
      if( VFP(outfile) ) {
         int w;
         w = (int)fwrite( ps, 1, len, outfile );
         if( w != len ) {
            fclose(outfile);
            outfile = (FILE *)-1;
            printf("WARNING: Failed write to log file [%s] ...\n", logfile);
         } else {
            fflush( outfile );
         }
      }

      if( addstdout ) {
         fwrite( ps, 1, len, stdout );
      }
   }
}

static void	prt( char * ps )
{
   static char buf[MXIO + 4];
	char * pb = buf;
	size_t i, j, k;
	char   c, d;
   i = strlen(ps);
	if(i) {
		k = 0;
		d = 0;
		for( j = 0; j < i; j++ )
		{
			c = ps[j];
			if( c == 0x0d ) {
				if( (j+1) < i ) {
					if( ps[j+1] != 0x0a ) {
						pb[k++] = c;
						c = 0x0a;
					}
            } else {
					pb[k++] = c;
					c = 0x0a;
				}
			} else if( c == 0x0a ) {
				if( d != 0x0d ) {
					pb[k++] = 0x0d;
				}
			}
			pb[k++] = c;
			d = c;
			if( k >= MXIO ) {
				pb[k] = 0;
				oi( pb );
				k = 0;
			}
		}	// for length of string
		if( k ) {
			//if( ( gbCheckCrLf ) &&
			//	( d != 0x0a ) ) {
				// add Cr/Lf pair
				//pb[k++] = 0x0d;
				//pb[k++] = 0x0a;
				//pb[k] = 0;
			//}
			pb[k] = 0;
			oi( pb );
		}
	}
}

// STDAPI StringCchVPrintf( OUT LPTSTR  pszDest,
//   IN  size_t  cchDest, IN  LPCTSTR pszFormat, IN  va_list argList );
int _cdecl sprtf( char * pf, ... )
{
   static char _s_sprtfbuf[1024];
   char * pb = _s_sprtfbuf;
   int   i;
   va_list arglist;
   va_start(arglist, pf);
   i = vsprintf( pb, pf, arglist );
   va_end(arglist);
   prt(pb);
   return i;
}

#ifdef UNICODE
// WIDE VARIETY
static void wprt( PTSTR ps )
{
   static char _s_woibuf[1024];
   char * cp = _s_woibuf;
   int len = (int)lstrlen(ps);
   if(len) {
      int ret = WideCharToMultiByte( CP_ACP, // UINT CodePage,            // code page
         0, // DWORD dwFlags,            // performance and mapping flags
         ps,   // LPCWSTR lpWideCharStr,    // wide-character string
         len,     // int cchWideChar,          // number of chars in string.
         cp,      // LPSTR lpMultiByteStr,     // buffer for new string
         1024,    // int cbMultiByte,          // size of buffer
         NULL,    // LPCSTR lpDefaultChar,     // default for unmappable chars
         NULL );  // LPBOOL lpUsedDefaultChar  // set when default char used
      //oi(cp);
      prt(cp);
   }
}

int _cdecl wsprtf( PTSTR pf, ... )
{
   static WCHAR _s_sprtfwbuf[1024];
   PWSTR pb = _s_sprtfwbuf;
   int   i = 1;
   va_list arglist;
   va_start(arglist, pf);
   *pb = 0;
   StringCchVPrintf(pb,1024,pf,arglist);
   //i = vswprintf( pb, pf, arglist );
   va_end(arglist);
   wprt(pb);
   return i;
}

#endif // #ifdef UNICODE

// eof - sprtf.cxx
