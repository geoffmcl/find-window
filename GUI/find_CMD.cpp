// find_CMD.cpp

#include "find_window.h"
#include <vector>
#include <string>
#include <sys/stat.h>   // for _fstat

using namespace std;

// forward ref
int Process_Commands( int argc, TCHAR * argv[]);

typedef vector<string>  VSTRING;

BOOL gb_Propagate = TRUE;
static BOOL _s_b_DnMessage = FALSE;

VSTRING vFind;
VSTRING vNotFind;

PTSTR pHelp =
"Find Window HELP\n"
"Usage: find_window [Options] window_to_find\n"
"Options:\n"
" -f --find=name - Alternative input of window to find\n"
" -n --not=name  - Skip windows containing this word.\n"
" -p --propagate[=no|yes] - Mark all of the same process. (Def=On)\n"
" -h --help - Put up this help dialog, and exit.\n"
" -v --version - Put up the About dialog, and exit.\n"
"All information is output to a log file.\n";

#define GOTO_EOL   for ( ; i < len; i++) {      \
                    c = buf[i];                 \
                    if ((c < ' ')&&(c != '\t')) \
                        break;                  \
                }

int process_buffer(char * buf, size_t len)
{
    size_t i, count, off;
    char c, d;
    count = 0;
    off = 0;
    d = 0;
    for (i = 0; i < len; i++) {
        c = buf[i];
        if (c > ' ') {
            if ((c == '#')||(c == ';')) {
                i++;    // discard line
                GOTO_EOL;
            } else {
                if (c == '"') {
                    d = c;
                    i++;
                    c = buf[i]; // get NEXT char
                }
                buf[off++] = c;
                count++;
                i++;    // get to end of parameter
                for ( ; i < len; i++) {
                    c = buf[i];
                    if ( d ) {  // wait until next inverted comma
                        if ( c == d ) { // is it
                            i++;    // bump past it
                            break;  // and end command
                        }
                    } else if ( c == '"' ) {
                        d = c;
                        i++;
                        c = buf[i]; // get NEXT char
                    } else if (c <= ' ')
                        break;
                    // else add/continue the command
                    buf[off++] = c;
                }
                buf[off++] = 0; // zero terminate parameter
                d = 0;  // kill any inverted comma
            }
        }
    }
    if (count)
    {
        // got a set of parameters
        count++;
        int bytes = (int)((count+1) * sizeof(char *));
        char * *cp = (char**)malloc(bytes);
        if (!cp) {
            if ( !_s_b_DnMessage ) {
                PTSTR ptmp = GetNxtBuf();
                sprintf(ptmp,"ERROR: Memory FAILED on %d bytes!\n"
                    "Will abort application on [OK]", bytes );
                MessageBox(g_hWnd,ptmp,"MEMORY ALLOCATION FAILED",
                    MB_OK | MB_ICONEXCLAMATION );
                sprtf("%s\n",ptmp);
                _s_b_DnMessage = TRUE;
            }
            return 1;
        }
        buf[off] = 0;
        count = 0;
        cp[count++] = "Dummy";
        for (i = 0; i < off; i++) {
            if (buf[i]) {
                cp[count++] = &buf[i];
                i++;
                for ( ; i < off; i++) {
                    if (buf[i] == 0)
                        break;
                }
            }
        }
        int iret = Process_Commands( (int)count, cp );
        free(cp);
        return iret;
    }
    return 0;
}

int Load_Input_File( PTSTR pFile )
{
    int iret = 1;
    struct stat buf;
    PTSTR ptmp;
    if ( stat(pFile, &buf) ) {
        if ( !_s_b_DnMessage ) {
            ptmp = GetNxtBuf();
            sprintf(ptmp,"ERROR: Unable to stat [%s]!\n"
                "Will abort application on [OK]", pFile );
            MessageBox(g_hWnd,ptmp,"INVALID FILE NAME",
                MB_OK | MB_ICONEXCLAMATION );
            sprtf("%s\n",ptmp);
            _s_b_DnMessage = TRUE;
        }
        return 1;
    }
    if (buf.st_size == 0)
        return 0;   // quietly FORGET an empty file

    FILE * fp = fopen(pFile,"rb");
    if (fp) {
        char * pfbuf = (char *)malloc( buf.st_size + 2 );
        if (!pfbuf) {
            fclose(fp);
            if ( !_s_b_DnMessage ) {
                ptmp = GetNxtBuf();
                sprintf(ptmp,"ERROR: Memory FAILED on %d bytes!\n"
                    "Will abort application on [OK]", buf.st_size + 2 );
                MessageBox(g_hWnd,ptmp,"MEMORY ALLOCATION FAILED",
                    MB_OK | MB_ICONEXCLAMATION );
                sprtf("%s\n",ptmp);
                _s_b_DnMessage = TRUE;
            }
            return 1;
        }
        // read file into allocated buffer
        if ( fread(pfbuf, 1, buf.st_size, fp) != buf.st_size ) {
            fclose(fp);
            if ( !_s_b_DnMessage ) {
                ptmp = GetNxtBuf();
                sprintf(ptmp,"ERROR: fread of %s\nFAILED on %d bytes!\n"
                    "Will abort application on [OK]", pFile, buf.st_size );
                MessageBox(g_hWnd,ptmp,"INVALID FILE",
                    MB_OK | MB_ICONEXCLAMATION );
                sprtf("%s\n",ptmp);
                _s_b_DnMessage = TRUE;
            }
            return 1;
        }
        iret = process_buffer(pfbuf,buf.st_size);
        free(pfbuf);
        fclose(fp);
        return iret;
    }
    if ( !_s_b_DnMessage ) {
        ptmp = GetNxtBuf();
        sprintf(ptmp,"ERROR: Unable to open file [%s]!\n"
            "Will abort application on [OK]", pFile );
        MessageBox(g_hWnd,ptmp,"INVALID FILE",
            MB_OK | MB_ICONEXCLAMATION );
        sprtf("%s\n",ptmp);
        _s_b_DnMessage = TRUE;
    }
    return 1;
}

int Process_Commands( int argc, TCHAR * argv[])
{
    PTSTR ptmp;
    int i;
    TCHAR opt;
    TCHAR * parg, *p;
    BOOL    btmp;
    for (i = 1; i < argc; i++) {
        parg = argv[i];
        p = parg;
        if (*parg == '@') {
            parg++;
            if (Load_Input_File(parg))
                goto Bad_Arg;
        } else if (*parg == '-') {
            parg++;
            if (*parg == '-')
            {   // --Options
                parg++;
                opt = *parg;
                if (strncmp(parg,"find", 4) == 0) {
                    parg += 3;
                } else if (strncmp(parg,"not", 3) == 0) {
                    parg += 2;
                } else if (strncmp(parg, "propagate", 9) == 0) {
                    parg += 8;
                } else if (strncmp(parg, "help", 4) == 0) {
                    parg += 3;
                } else if (strncmp(parg, "version", 7) == 0) {
                    parg += 6;
                } else {
                    goto Bad_Arg;
                }
            } else { // -Options
                opt = *parg;
            }
            if (opt == 'h') {
                if ( !_s_b_DnMessage ) {
                    ptmp = GetNxtBuf();
                    sprintf(ptmp,"HELP: %s\n"
                        "Will abort application on [OK]", pHelp );
                    MessageBox(g_hWnd,ptmp,"HELP DIALOG",
                        MB_OK | MB_ICONINFORMATION );
                    sprtf("%s\n",ptmp);
                    _s_b_DnMessage = TRUE;
                }
                return 1;
            } else if (opt == 'v') {
                if ( !_s_b_DnMessage ) {
                    Do_IDM_ABOUT( g_hInst, g_hWnd );
                    _s_b_DnMessage = TRUE;
                }
                return 1;
            } else if (opt == 'f') {
                // add to find set
                parg++;
                opt = *parg;
                if (opt == 0) {
                    // assume is next parameter
                    if ((i + 1) < argc) {
                        i++;
                        parg = argv[i];
                    } else
                        goto Bad_Arg;

                } else if ((opt == '=')||(opt == ':')) {
                    parg++;
                } else
                    goto Bad_Arg;
                // aadd to find list
                vFind.push_back(parg);
            } else if (opt == 'n') {
                // add to NOT find set
                parg++;
                opt = *parg;
                if (opt == 0) {
                    // assume is next parameter
                    if ((i + 1) < argc) {
                        i++;
                        parg = argv[i];
                    } else
                        goto Bad_Arg;

                } else if ((opt == '=')||(opt == ':')) {
                    parg++;
                } else
                    goto Bad_Arg;
                // aadd to find list
                vNotFind.push_back(parg);
            } else if (opt == 'p') {
                parg++;
                opt = *parg;
                btmp = gb_Propagate;
                if (opt == 0)
                    gb_Propagate = TRUE;
                else if ((opt == '=')||(opt == ':')) {
                    parg++;
                    if (IsYes(parg)||IsNo(parg)) {
                        if (IsYes(parg))
                            gb_Propagate = TRUE;
                        else
                            gb_Propagate = FALSE;
                    } else
                        goto Bad_Arg;
                } else
                    goto Bad_Arg;
                if ( btmp != gb_Propagate )
                    bChgPG = TRUE;
            } else {
Bad_Arg:
                if ( !_s_b_DnMessage ) {
                    ptmp = GetNxtBuf();
                    sprintf(ptmp,"ERROR: Unknown argument [%s]!\n"
                        "Will abort application on [OK]", p );
                    MessageBox(g_hWnd,ptmp,"COMMAND LINE ERROR",
                        MB_OK | MB_ICONEXCLAMATION );
                    sprtf("%s\n",ptmp);
                    _s_b_DnMessage = TRUE;
                }
                return 1;
            }
        } else {
            // assume it is a find
            vFind.push_back(parg);
        }
    }

    return 0;
}

// parse command line arguments, if any
int ParseArgs( int argc, TCHAR * argv[])
{
    vFind.clear();
    vNotFind.clear();
    return (Process_Commands(argc,argv));
}

size_t get_find_count(void)
{
    return vFind.size();
}

static size_t _s_find_offset = 0;
PTSTR get_first_find(void)
{
    _s_find_offset = 0;
    if (get_find_count()) {
        PTSTR pstg = (PTSTR)vFind[_s_find_offset].c_str();
        _s_find_offset++;
        return pstg;
    }
    return NULL;
}

PTSTR get_next_find(void)
{
    size_t cnt = get_find_count();
    if (cnt && (_s_find_offset < cnt)) {
        PTSTR pstg = (PTSTR)vFind[_s_find_offset].c_str();
        _s_find_offset++;
        return pstg;
    }
    return NULL;
}

size_t get_not_find_count(void)
{
    return vNotFind.size();
}

static size_t _s_not_find_offset = 0;
PTSTR get_first_not_find(void)
{
    _s_not_find_offset = 0;
    if (get_not_find_count()) {
        PTSTR pstg = (PTSTR)vNotFind[_s_not_find_offset].c_str();
        _s_not_find_offset++;
        return pstg;
    }
    return NULL;
}

PTSTR get_next_not_find(void)
{
    size_t cnt = get_not_find_count();
    if (cnt && (_s_not_find_offset < cnt)) {
        PTSTR pstg = (PTSTR)vNotFind[_s_not_find_offset].c_str();
        _s_not_find_offset++;
        return pstg;
    }
    return NULL;
}

// eof - find_CMD.cpp