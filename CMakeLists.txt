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

# eof
