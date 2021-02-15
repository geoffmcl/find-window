/*\
 * find_sprtf.h
 *
 * Copyright (c) 2012-2014 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
#ifndef _FIND_SPRTF_H_
#define _FIND_SPRTF_H_


extern int add_std_out( int val );
extern int add_sys_time( int val );
extern char* get_log_file(void);
extern int open_log_file( void );
extern void close_log_file( void );
extern void set_log_file( char * nf, bool open = true );
extern int _cdecl sprtf( char * pf, ... );



#endif // #ifndef _FIND_SPRTF_H_
// oef - find-sprtf.h
