/*\
 * find-window.h
 *
 * Copyright (c) 2012-2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
#ifndef _FIND_WINDOWS_H_
#define _FIND_WINDOWS_H_

#pragma warning ( disable : 4996 )

#include "targetver.h"

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "find-utils.h"
#include "find-sprtf.h"

#define VERDATE "20210215"  // in DELL03, use APPDATA, ...
#define VERSION "0.0.5"
#define APPNAME "find-window"

// VERDATE "20190915"  // add show window style
// VERSION "0.0.4"

// VERDATE "20170304"  // with -v show more information about found windows
// VERSION "0.0.3"

// VERDATE "20130519"  // with whole compare, also check the window title does not match the test beginning
// VERSION "0.0.2"
// original version
//#define VERSION "0.0.1"

#endif // #ifndef _FIND_WINDOWS_H_
// eof - find-window.h
