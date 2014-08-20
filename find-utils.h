/*\
 * find_utils.cpp
 *
 * Copyright (c) 2012-2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
#ifndef _FIND_UTILS_H_
#define _FIND_UTILS_H_

#define MX_ONE_BUF 264
#define MX_BUFFERS 64
extern PTSTR GetNxtBuf( void );

extern INT   InStri( PTSTR lpb, PTSTR lps );
extern INT   InStr( PTSTR lpb, PTSTR lps );

extern void printError( TCHAR* msg, DWORD eNum );


#endif // #ifndef _FIND_UTILS_H_
// eof - find-utils.h
