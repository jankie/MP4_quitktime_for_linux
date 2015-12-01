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


#include "std.h"
#include "string.h"
#include "nsguid.h"

String::String(const char *initial_val) {
  val = NULL;
  setValue(initial_val);
}

String::String(const String &s) {
  val = NULL;
  setValue(s.getValue());
}

String::~String() {
  FREE(val);
}

const char *String::setValue(const char *newval) {
  if (newval == val) return val; //FG
  FREE(val); return val = STRDUP(newval);
}

int String::len() const {
  return STRLEN(getValue());
}

int String::isempty() {
  return (!val || !*val);
}

void String::toupper() {
  if (!isempty()) for (char *p = val; *p; p++) *p = TOUPPER(*p);
}

void String::tolower() {
  if (!isempty()) for (char *p = val; *p; p++) *p = TOLOWER(*p);
}

int String::isequal(const char *otherval) const {
  return !STRCMP(otherval, getValue());
}

const char *String::printf(const char *format, ...) {
  va_list args;
  va_start (args, format);
  vsprintf(format,args);
  va_end(args);
  return getValue();
}


void String::vsprintf(const char *format, va_list args) {
  if(!format) return;
  
  va_list saveargs=args;
  // roughly evaluate size of dest string
  const char *p=format;
  int length=0;
  while(*p) {
    if(*(p++)!='%') length++;
    else {
      void *arg=va_arg(args, void *);
      for (;;) {
        const char f=*p++;
        if(f=='c') length++;
        else if(f=='i') length+=16;
        else if(f=='d') length+=32;
        else if(f=='x') length+=32;
        else if(f=='s') length+=STRLEN((const char *)arg);
        else if(ISDIGIT(f)) continue;
        else ASSERTPR(0,"undefined format passed to stringprintf!");
        break;
      }
    }
  }
  if(val) {
    if(len() < length) 
      val=(char *)REALLOC(val,length+1);
  } else val=(char *)MALLOC(length+1);

  // now write the string in val
  int real_len = ::vsprintf(val,format,saveargs);
  ASSERTPR(real_len <= length, "String.printf overflow");
  // realloc to actual size used
  if (real_len > 0 && (length - real_len) > wastage_allowed) {
    val = (char *)REALLOC(val, real_len+1);
  }
}

const char *String::cat(const char *value) {
  if (value == NULL || *value == 0) return getValue();
  if (val == NULL) return setValue(value);
  return setValue(StringPrintf("%s%s", getValue(), value));
}

StringPrintf::StringPrintf(const char *format, ...)
  : String(NULL) {
  va_list args;
  va_start (args, format);
  vsprintf(format,args);
  va_end(args);
}

StringPrintf::StringPrintf(int val)
  : String(NULL) {
  *this += val;
}

StringPrintf::StringPrintf(GUID g)
  : String(NULL) {
  char splab[nsGUID::GUID_STRLEN+1];
  nsGUID::toChar(g, splab);
  cat(splab);
}

RecycleString::RecycleString(const char *initial_val) : String() {
  setValue(initial_val);
}

const char *RecycleString::reuseValue(const char *newval) {
  // try to recycle the space
  if (newval == val) return val;
  if (newval != NULL && val != NULL && STRLEN(newval) <= len()) {
    STRCPY(val, newval);
  } else String::setValue(newval);	// just reset it
  return val;
}
