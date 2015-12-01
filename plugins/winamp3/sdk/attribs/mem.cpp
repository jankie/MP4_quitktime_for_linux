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

#include "../common/std.h"

// we reimplment these for the plugins

#undef GlobalAlloc
#undef GlobalFree

void *DO_MALLOC(int size) {
  if (size <= 0) return NULL;
  return GlobalAlloc(GPTR, size);
}

void DO_FREE(void *ptr) {
  if (ptr != NULL) GlobalFree(ptr);
}

void *DO_REALLOC(void *ptr, int size) {
  if (size <= 0) return NULL;
  void *r = GlobalReAlloc((HGLOBAL)ptr, size, GMEM_ZEROINIT);
  if (!r) {
    void *newblock = MALLOC(size);
    int s = GlobalSize(ptr);
    MEMCPY(newblock, ptr, s);
    FREE(ptr);
    r = newblock;
  }
  return r;
}

void MEMCPY(void *dest, const void *src, int n) {
  MoveMemory(dest, src, n);
}

int STRLEN(const char *str) {
  if (!str) return 0;
  return lstrlen(str);
}

void STRCPY(char *dest, const char *src) {
  lstrcpy(dest, src);
}

int STRICMP(const char *str1, const char *str2) {
  return lstrcmpi(str1, str2);
}

char *DO_STRDUP(const char *ptr) {
  if (ptr == NULL) return NULL;
  char *ret = static_cast<char *>(DO_MALLOC(STRLEN(ptr)+1));
  if (ret != NULL) {
    STRCPY(ret, ptr);
  }
  return ret;
}

void _assert_handler(char *reason, char *file, int line) {}
void _assert_handler_str(char *string, char *reason, char *file, int line) {}
