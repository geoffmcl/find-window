
#pragma once

extern int ParseArgs( int argc, TCHAR * argv[]);

// find items
extern size_t get_find_count(void);
extern PTSTR get_first_find(void);
extern PTSTR get_next_find(void);

// not find items
extern PTSTR get_first_not_find(void);
extern PTSTR get_next_not_find(void);
extern size_t get_not_find_count(void);

extern BOOL gb_Propagate;

// eof - find_CMD.h
