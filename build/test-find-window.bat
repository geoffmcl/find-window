@setlocal
@set TMPEXE=Release\find-window.exe
@if NOT EXIST %TMPEXE% goto ERR1

@%TMPEXE% setup.txt -v

@if ERRORLEVEL 1 goto FOUND

@echo A 'setup.txt' window title NOT found

@goto END

:FOUND
@echo Found a 'setup.txt' window...
@goto END

:END
@endlocal
@exit /b 0
