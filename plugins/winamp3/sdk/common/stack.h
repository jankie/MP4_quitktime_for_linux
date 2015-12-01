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
#ifndef _STACK_H
#define _STACK_H

#include "common.h"

#include "../studio/assert.h"

#include "std.h"

// a self-growing stack. note that it never shrinks (for now)

#define STACK_SIZE_INCREMENT 256

template<class T>
class COMEXP Stack {
public:
  Stack() {
    nslots = 0;
    cur = 0;
    stack = NULL;
  }
  ~Stack() {
    FREE(stack);
  }
  int push(T item) {
    if (stack == NULL) {
      nslots = STACK_SIZE_INCREMENT;
      stack = (T *)MALLOC(sizeof(T) * nslots);
    } else if (cur >= nslots) {
      T *newstack;
      int newnslots;
      newnslots = nslots + STACK_SIZE_INCREMENT;
      newstack = (T *)MALLOC(newnslots * sizeof(T));
      MEMCPY(newstack, stack, nslots * sizeof(T));
      FREE(stack);
      stack = newstack;
      nslots = newnslots;
    }
    stack[cur++] = item;
    return cur;
  }
  int peek() {
    return cur;
  }
  T top() {
    ASSERT(cur >= 0);
    if (cur == 0) return NULL;
    return stack[cur-1];
  }
  int pop(T *ptr) {
    ASSERT(cur >= 0);
    if (cur == 0) return 0;
    ASSERT(stack != NULL);
    *ptr = stack[--cur];
    return 1;
  }
  int isempty() {
    return cur == 0;
  }
private:
  int nslots, cur;
  T *stack;
};

#endif
