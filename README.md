find-window Project Overview
============================

Given a window title, enumerate all existing window searching for a match 
to that title. Exit setting errorlevel to the count of windows matched.

20120531 - MSVC10 AppWizard created this initial find-window application

Naturally this is a MS Windows ONLY application, since it uses the Window OS API
to enumerate and get titles of all windows, visible and hidden, and these functions
are not portable to other platforms.

Files:
------

CMakeLists.txt - new 20140820  
    cmake configuration and generation of the build file

find-window.cpp/h  
    This is the main application source file.

find-sprtf.cpp/h  
    Log output

find-utils.cpp/h  
    Some simple utility functions
    
README.md  
    You are reading it now...
    
Directories:
------------

build -  
    Where the building of the 32-bit application takes place.  
    Files:  
        build-me.bat - a helpful batch fiel to do the building and installing.

build.x64  
    Where the building of the 64-bit application takes place
    Files:  
        build-me.bat - a helpful batch file to do the building and installing.

NOTE: The very SPECIFIC install location in the above batch files, and in the 
CMakeLists.txt file. I put all utility apps I build into a C:\MDOS directory, 
which I then add to my PATH environment variable.

Enjoy...

Geoff.  
20140820
        
; eof
