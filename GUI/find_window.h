#pragma once

#include "find_VER.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <psapi.h>
#include <tlhelp32.h>   // for PROCESSENTRY32, ...
#include <vdmdbg.h>
#include <direct.h> // for _mkdir, ...

// TODO: reference additional headers your program requires here
#include "find_window.h"
#include "find_INI.h"
#include "find_ABT.h"
#include "find_EnumW.h"
#include "find_Scroll.h"
#include "find_Utils.h"
#include "find_EnumP.h"
#include "find_Sprtf.h"
#include "find_Input.h"
#include "find_Timer.h"
#include "find_Paint.h"
#include "find_CMD.h"
#include "find_Menu.h"

#define  MEOR  "\r\n"
#define  CCH_RGBUF   256

extern HWND g_hWnd;         // main handle
extern HINSTANCE g_hInst;   // current instance

#include "resource.h"

// eof - find_window.h
