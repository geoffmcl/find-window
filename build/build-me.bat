@setlocal
@set TMPPRJ=find-window
@set TMPLOG=bldlog-1.txt
@set BUILD_RELDBG=0

@call chkmsvc %TMPPRJ%

@echo Built project %TMPORJ%... all ouput to %TMPLOG%

@set CMOPTS=
@REM ***************************************************
@REM NOTE WELL: FIXME: NOTE SPECIAL INSTALL PREFIX
@REM ***************************************************
@set CMOPTS=%CMOPTS% -DCMAKE_INSTALL_PREFIX=C:/MDOS
@REM ***************************************************

@echo Commence build %DATE% at %TIME% > %TMPLOG%

cmake .. %CMOPTS% >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR1

cmake --build . --config Debug >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR4

@if "%BUILD_RELDBG%x" == "0x" goto DNRELDBG
cmake --build . --config RelWithDebInfo >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR2
:DNRELDBG

cmake --build . --config Release >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR3

@echo Appears successful...
@echo.
@echo Contine with release install to C:/MDOS?
@pause
cmake --build . --config Release --target INSTALL >> %TMPLOG% 2>&1
@if ERRORLEVEL 1 goto ERR5

@fa4 " -- " %TMPLOG%

@echo Done build and install to C:/MDOS?

@goto END

:ERR1
@echo ERROR: cmake config/gen
@goto ISERR

:ERR2
@echo ERROR: cmake build RelWitDebInfo
@goto ISERR

:ERR3
@echo ERROR: cmake build Release
@goto ISERR

:ERR4
@echo ERROR: cmake build Debug
@goto ISERR

:ERR5
@echo ERROR: installing release
@goto ISERR

:NOXDIR
@echo.
@echo Oops, no X: directory found! Needed for simgear, etc
@echo Run setupx, or hdem3m, etc, to establish X: drive
@echo.
@goto ISERR

:ISERR
@endlocal
@exit /b 1

:END
@endlocal
@exit /b 0

@REM eof
