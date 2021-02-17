// sprtf.h
#ifndef _sprtf_H_
#define _sprtf_H_
#include <stdio.h>

#ifdef  __cplusplus
extern  "C"
{
#endif  // __cplusplus

#define  VFP(a)      ( a != NULL )

extern int open_log_file( void );
extern int _cdecl sprtf( char * pf, ... );
extern void close_log_file( void );

#ifdef UNICODE
extern int _cdecl wsprtf( PTSTR pf, ... );
#endif

extern int add_std_out( int val );
extern int add_sys_time( int val );
extern void   set_log_file( char * nf );

extern char * get_log_file( void );

#define MDT_NONE 0
#define	MDT_FILE 1
#define	MDT_DIR 2

extern int is_file_or_directory(const char*);


#ifdef  __cplusplus
}
#endif  // __cplusplus

#endif // _sprtf_H_
// eof - sprtf.h
