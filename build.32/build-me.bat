@setlocal

@set DOINST=0
@set TMPPRJ=find-window
@set TMPLOG=bldlog-1.txt
@set CONTONERR=0
@set BUILD_RELDBG=0

@REM ###########################################
@REM NOTE: Specific install location
@REM ###########################################
@set TMPINST=C:\MDOS

@REM ############################################
@REM NOTE: MSVC 16 2019
@REM Adjust to suit your environment
@REM ##########################################
@set GENERATOR=Visual Studio 16 2019

@set TMPOPTS=
@set TMPOPTS=%TMPOPTS% -DCMAKE_INSTALL_PREFIX:PATH=%TMPINST%
@set TMPOPTS=%TMPOPTS% -G "%GENERATOR%" -A Win32
@set TMPOPTS=%TMPOPTS% -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON

@call chkmsvc %TMPPRJ%

@echo Built project %TMPORJ%... all ouput to %TMPLOG%

@set CMOPTS=%TMPOPTS%
@echo Commence build %DATE% at %TIME% > %TMPLOG%

@echo Doing: 'cmake .. %CMOPTS%
@echo Doing: 'cmake .. %CMOPTS% >> %TMPLOG%
@cmake .. %CMOPTS% >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR1

@echo Doing: 'cmake --build . --config Debug'
@echo Doing: 'cmake --build . --config Debug' >> %TMPLOG%
@cmake --build . --config Debug >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR2

:DNDEBBLD

@if "%BUILD_RELDBG%x" == "0x" goto DNRELDBG
@echo Doing: 'cmake --build . --config RelWithDebInfo'
@echo Doing: 'cmake --build . --config RelWithDebInfo' >> %TMPLOG%
@cmake --build . --config RelWithDebInfo >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR3

:DNRELDBG
@echo Doing: 'cmake --build . --config Release'
@echo Doing: 'cmake --build . --config Release' >> %TMPLOG%
@cmake --build . --config Release >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR4

:DNRELBLD

@echo.
@echo Appears successful...
@echo.
@if NOT "%DOINST%x" == "1x" (
@echo Install NOT configued. Set DOINST=1
@goto END
)
@echo Continue with Release install to %TMPINST%?
@echo.
@pause
@echo Doing: 'cmake --build . --config Release --target INSTALL'
@echo Doing: 'cmake --build . --config Release --target INSTALL' >> %TMPLOG%
@cmake --build . --config Release --target INSTALL >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR5

@fa4 " -- " %TMPLOG%

@echo Done build and install of %TMPPRJ% to %TMPINST%

@goto END

:ERR1
@echo FATAL ERROR: cmake configuration/generation FAILED
@echo FATAL ERROR: cmake configuration/generation FAILED >> %TMPLOG%
@goto ISERR

:ERR2
@echo ERROR: cmake build Debug
@echo ERROR: cmake build Debug >> %TMPLOG%
@if %CONTONERR% EQU 1 goto DNDEBBLD
@goto ISERR

:ERR3
@echo ERROR: cmake build RelWithDebInfo
@echo ERROR: cmake build RelWithDebInfo >> %TMPLOG%
@if %CONTONERR% EQU 1 goto DNRELBLD
@goto ISERR

:ERR4
@echo ERROR: cmake build Release
@echo ERROR: cmake build Release >> %TMPLOG%
@goto ISERR

:ERR5
@echo ERROR: cmake install Release
@echo ERROR: cmake install Release >> %TMPLOG%
@goto ISERR

:ISERR
@endlocal
@exit /b 1

:END
@endlocal
@exit /b 0

@REM eof
