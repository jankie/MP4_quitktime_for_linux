/*

  Nullsoft WASABI Source File License

  Copyright 1999-2001 Nullsoft, Inc.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.


  Brennan Underwood
  brennan@nullsoft.com

*/

//PORTABLE
#ifndef _STD_H
#define _STD_H

#ifndef NULL
#define NULL 0
#endif

#include "platform/platform.h"
#include "common.h"
#include "../studio/assert.h"

#ifdef FORTIFY
#define FORTIFY_INFO                , char *file, int line
#define FORTIFY_PARAMS              , file, line
#define FORTIFY_HERE                , __FILE__, __LINE__
#define MALLOC(size)                DO_MALLOC(size, __FILE__, __LINE__)
#define CALLOC(records, recordsize) DO_CALLOC(records, recordsize, __FILE__, __LINE__)
#define REALLOC(ptr, size)          DO_REALLOC(ptr, size, __FILE__, __LINE__)
#define FREE(ptr)                   DO_FREE(ptr, __FILE__, __LINE__)
#define MEMDUP(src, n)              DO_MEMDUP(src, n, __FILE__, __LINE__)
#define STRDUP(ptr)                 DO_STRDUP(ptr, __FILE__, __LINE__)
#else
#define FORTIFY_INFO
#define FORTIFY_PARAMS
#define FORTIFY_HERE
#define MALLOC(size)                DO_MALLOC(size)
#define CALLOC(records, recordsize) DO_CALLOC(records, recordsize)
#define REALLOC(ptr, size)          DO_REALLOC(ptr, size)
#define FREE(ptr)                   DO_FREE(ptr)
#define MEMDUP(src, n)              DO_MEMDUP(src, n)
#define STRDUP(ptr)                 DO_STRDUP(ptr)
#endif

#if _MSC_VER >= 1100
#define NOVTABLE __declspec(novtable)
#else
#define NOVTABLE
#endif

COMEXP void *DO_MALLOC(int size FORTIFY_INFO);
COMEXP void *DO_CALLOC(int records, int recordsize FORTIFY_INFO);
COMEXP void *DO_REALLOC(void *ptr, int size FORTIFY_INFO);
COMEXP void DO_FREE(void *ptr FORTIFY_INFO);

COMEXP void MEMCPY(void *dest, const void *src, int n);
COMEXP void *DO_MEMDUP(const void *src, int n FORTIFY_INFO);
COMEXP void MEMZERO(void *dest, int nbytes);
COMEXP void MEMSET(void *dest, char c, int n);
COMEXP int MEMCMP( const void *buf1, const void *buf2, int count );

static __inline int TOUPPER(int c) { return toupper(c); }
static __inline int TOLOWER(int c) { return tolower(c); }
static __inline int ISDIGIT(int c) { return isdigit(c); }
static __inline int ISALPHA(int c) { return isalpha(c); }
static __inline int ISSPACE(int c) { return isspace(c); }
static __inline int ISPUNCT(int c) { return ispunct(c); }
static __inline int ATOI(const char *str) { if (!str) return 0; return atoi(str); }
static __inline double ATOF(const char *str) { if (!str) return 0.0; return atof(str); }
static __inline int STRTOL(const char *str, char **stopstr, int base) {
  return strtol(str, stopstr, base);
}
// hex to int
static __inline int XTOI(const char *str) {
  char *dummy;
  return STRTOL(str, &dummy, 16);
}

COMEXP char *DO_STRDUP(const char *ptr FORTIFY_INFO);
COMEXP int STRLEN(const char *str);
COMEXP int STRCMP(const char *str1, const char *str2);
COMEXP int STRICMP(const char *str1, const char *str2);
COMEXP int STRNCMP(const char *str1, const char *str2, int len);
COMEXP int STRNNCMP(const char *str1, const char *str2); // uses len of str2
COMEXP int STRNICMP(const char *str1, const char *str2, int len);
COMEXP int STRNINCMP(const char *str1, const char *str2); // uses len of str2
COMEXP int STREQL(const char *str1, const char *str2);
COMEXP int STRCASEEQL(const char *str1, const char *str2);
COMEXP char *STRCASESTR(const char *str1, const char *str2);
COMEXP char *STRSTR(const char *str1, const char *str2);
COMEXP int STRCASERPL(char *s, const char *w, const char *r, int maxchar);
COMEXP void STRCPY(char *dest, const char *src);
COMEXP void STRNCPY(char *dest, const char *src, int maxchar);
COMEXP char *STRCHR(const char *str, int c);
COMEXP void STRCAT(char *dest, const char *append);
COMEXP unsigned long STRTOUL(const char *str, char **p, int radix);
COMEXP int PATHEQL(const char *str1, const char *str2);
COMEXP void STRTOUPPER(char *str);
COMEXP void STRTOLOWER(char *str);

#ifndef NS_TRANSCENDENTALS
#define NS_TRANSCENDENTALS

#		ifndef NS_REAL_DEFINED
#		define NS_REAL_DEFINED
		typedef double Real;
#		endif//NS_REAL_DEFINED

COMEXP Real SIN( Real );
COMEXP Real COS( Real );

#endif//NS_TRANSCENDENTALS

#define RGBTOBGR(col) ((col & 0xFF000000) | ((col & 0xFF0000) >> 16) | (col & 0xFF00) | ((col & 0xFF) << 16))

#ifndef _NOSTUDIO
// EXTC is used here as some .c files will use these functions
EXTC COMEXP FILE *FOPEN(const char *filename,const char *mode);
EXTC COMEXP int FCLOSE(FILE *stream);
EXTC COMEXP int FSEEK(FILE *stream, long offset, int origin);
EXTC COMEXP long FTELL(FILE *stream);
EXTC COMEXP size_t FREAD(void *buffer, size_t size, size_t count, FILE *stream);
EXTC COMEXP size_t FWRITE(const void *buffer, size_t size, size_t count, FILE *stream);
EXTC COMEXP char *FGETS( char *string, int n, FILE *stream );
EXTC COMEXP int FPRINTF( FILE *stream, const char *format , ...);
EXTC COMEXP int FGETSIZE(FILE *stream);
EXTC COMEXP const char *TMPNAM(char *string);
EXTC COMEXP const char *TMPNAM2(char *string, int val);
EXTC COMEXP int FEXISTS(const char *filename); // return 1 if true, 0 if not, -1 if unknown
EXTC COMEXP int UNLINK(const char *filename); // return 1 on success, 0 on error

#ifndef REAL_STDIO
#define fopen FOPEN
#define fclose FCLOSE
#define fseek FSEEK
#define ftell FTELL
#define fread FREAD
#define fwrite FWRITE
#define fgets FGETS
#define fprintf FPRINTF
#define unlink UNLINK
#endif

#endif

#ifdef WIN32
#define SPRINTF wsprintf
#else
#define SPRINTF sprintf
#endif

#define SSCANF sscanf

#undef MAX_PATH
#define WA_MAX_PATH (8*1024)
#define MAX_PATH WA_MAX_PATH

// seconds since 1970
typedef unsigned int stdtimeval;
// milliseconds since...??
typedef double stdtimevalms;

typedef unsigned long THREADID;

#include "guid.h"

#ifdef __cplusplus

class COMEXP Std {
public:
  Std();

  static void getMousePos(POINT *p);
  static void getMousePos(int *x, int *y);
  static void getMousePos(long *x, long *y);
  static void setMousePos(POINT *p);
  static void setMousePos(int x, int y);

  static int keyDown(int code) { return !!(GetKeyState(code) & 0x8000); }
  
  static int pointInRect(RECT *r, POINT &p);
  static void setRect(RECT *r, int left, int top, int right, int bottom) {
    r->left = left;
    r->top = top;
    r->right = right;
    r->bottom = bottom;
  }
  static RECT makeRect(int left, int top, int right, int bottom) {
    RECT r;
    r.left = left;
    r.top = top;
    r.right = right;
    r.bottom = bottom;
    return r;
  }
  static POINT makePoint(int x, int y) {
    POINT p = { x, y };
    return p;
  }
  static void offsetRect(RECT *r, int x, int y) {
    r->left += x;
    r->right += x;
    r->top += y;
    r->bottom += y;
  }

  static int random();

  // time functions
  static void usleep(int ms);
  // get time in seconds since 1970
  static stdtimeval getTimeStamp();
  // get time in seconds as double, but not since any specific time
  // useful for relative timestamps only
  static stdtimevalms getTimeStampMS();
  // get milliseconds since system started. usefull for relative only
  static DWORD getTickCount();

  static void tolowerString(char *str);
  static void ensureVisible(RECT *r);
  static int getScreenWidth();
  static int getScreenHeight();

  static int messageBox(char *txt, char *title, int flags);

  static int getDoubleClickDelay();
  static int getDoubleClickX();
  static int getDoubleClickY();

  static void getViewport(RECT *r, POINT *p);
  static void getViewport(RECT *r, RECT *sr);
  static void getViewport(RECT *r, HWND wnd);
  static void getViewport(RECT *r, POINT *p, RECT *sr, HWND wnd);

  // returns the address of the last occurence of any of the characters of toscan in str string
  static const char *scanstr_back(const char *str, const char *toscan, const char *defval);

  // retrieves extension of a given filename
  static const char *extension(const char *fn);

  // retrieves filename from a given path+filename
  static const char *filename(const char *fn);

  static int getCurDir(char *str, int maxlen);
  static int setCurDir(const char *str);

  static void GUIDprint(GUID &guid, char *str);

  // regexp match functions

  // A match means the entire string TEXT is used up in matching.
  // In the pattern string:
  //      `*' matches any sequence of characters (zero or more)
  //      `?' matches any character
  //      [SET] matches any character in the specified set,
  //      [!SET] or [^SET] matches any character not in the specified set.

  // A set is composed of characters or ranges; a range looks like
  // character hyphen character (as in 0-9 or A-Z).  [0-9a-zA-Z_] is the
  // minimal set of characters allowed in the [..] pattern construct.
  // Other characters are allowed (ie. 8 bit characters) if your system
  // will support them.

  // To suppress the special syntactic significance of any of `[]*?!^-\',
  // and match the character exactly, precede it with a `\'.

  enum {
    MATCH_VALID=1,  /* valid match */
    MATCH_END,      /* premature end of pattern string */
    MATCH_ABORT,    /* premature end of text string */
    MATCH_RANGE,    /* match failure on [..] construct */
    MATCH_LITERAL,  /* match failure on literal match */
    MATCH_PATTERN,  /* bad pattern */
  };

  enum {
    PATTERN_VALID=0,  /* valid pattern */
    PATTERN_ESC=-1,   /* literal escape at end of pattern */
    PATTERN_RANGE=-2, /* malformed range in [..] construct */
    PATTERN_CLOSE=-3, /* no end bracket in [..] construct */
    PATTERN_EMPTY=-4, /* [..] contstruct is empty */
  };

  // return TRUE if PATTERN has any special wildcard characters
  static BOOL isMatchPattern(const char *p);

  // return TRUE if PATTERN has is a well formed regular expression
  static BOOL isValidMatchPattern(const char *p, int *error_type);

  // return MATCH_VALID if pattern matches, or an errorcode otherwise
  static int matche(register const char *p, register const char *t);
  static int matche_after_star(register const char *p, register const char *t);

  // return TRUE if pattern matches, FALSE otherwise.
  static BOOL match(const char *p, const char *t);

  // a unique # returned by the OS
  static THREADID getCurrentThreadId();
  
  // gets the system temporary path. the returned string will end with a backslash!
  static void getTempPath(int bufsize, char *dst);

  // creates a directory. returns 0 on error, nonzero on success
  static int createDirectory(const char *dirname);

  // gets informations about a given filename. returns 0 if file not found
  typedef struct {
    unsigned int fileSizeHigh;
    unsigned int fileSizeLow;
    unsigned int lastWriteTimeHigh;
    unsigned int lastWriteTimeLow;
  } fileInfoStruct;
  static int getFileInfos(const char *filename, fileInfoStruct *infos);

  // removes a directory
  static void removeDirectory(const char *dir, int recursive);
};

// neat trick from C++ book, p. 161
template<class T> inline T MAX(T a, T b) { return a > b ? a : b; }
template<class T> inline T MIN(T a, T b) { return a > b ? b : a; }
template<class T> inline T MINMAX(T a, T minval, T maxval) {
  return (a < minval) ? minval : ( (a > maxval) ? maxval : a );
}
template<class T> inline T ABS(T a) { return a < 0 ? -a : a; }
template<class T> inline T SQR(T a) { return a * a; }
template<class T> inline int CMP3(T a, T b) {
  if (a < b) return -1;
  if (a == b) return 0;
  return 1;
}

#endif

#endif
