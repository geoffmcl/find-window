#########################################################################
# 20140820 - Hand crafed CMakeLists.txt for find-window project - geoff #
#########################################################################
cmake_minimum_required (VERSION 2.8.5)

project(find_window)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(name find-window)
    message(STATUS "*** 64-bit build of ${name}")
else ()
    set(name find-window32)
    message(STATUS "*** 32-bit build of ${name}")
endif ()
set(dir src)
add_executable( ${name} 
    ${dir}/find-window.cpp ${dir}/find-window.h 
    ${dir}/find-sprtf.cpp ${dir}/find-sprtf.h 
    ${dir}/find-utils.cpp ${dir}/find-utils.h
    ${dir}/targetver.h
    )
set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
# deal with install - *** FIXME: NOTE SPECIAL PERSONAL INSTALL LOCATION ***
install( TARGETS ${name} DESTINATION C:/MDOS )

# ADD the GUI
# Now excluded ${dir}/stdafx.cpp ${dir}/stdafx.h
set(name find_window)
set(dir GUI)
set(GUI_srcs
    ${dir}/find_ABT.cpp   ${dir}/find_ABT.h
    ${dir}/find_CMD.cpp   ${dir}/find_CMD.h
    ${dir}/find_EnumP.cpp ${dir}/find_EnumP.h
    ${dir}/find_EnumW.cpp ${dir}/find_EnumW.h
    ${dir}/find_INI.cpp   ${dir}/find_INI.h
    ${dir}/find_Input.cpp ${dir}/find_Input.h
    ${dir}/find_Menu.cpp  ${dir}/find_Menu.h
    ${dir}/find_Paint.cpp ${dir}/find_Paint.h
    ${dir}/find_Scroll.cpp ${dir}/find_Scroll.h
    ${dir}/find_Sprtf.cpp ${dir}/find_Sprtf.h
    ${dir}/find_Timer.cpp ${dir}/find_Timer.h
    ${dir}/find_Utils.cpp ${dir}/find_Utils.h
    ${dir}/find_window.cpp ${dir}/find_window.h
    ${dir}/find_VER.h
    ${dir}/resource.h
    ${dir}/targetver.h
    ${dir}/find_window.rc
    )
add_executable( ${name} WIN32 ${GUI_srcs} )
if(MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif()
# deal with install - *** FIXME: NOTE SPECIAL PERSONAL INSTALL LOCATION ***
install( TARGETS ${name} DESTINATION C:/MDOS )

# eof
