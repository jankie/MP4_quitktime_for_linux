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

#ifndef _SVC_DEVICE_H
#define _SVC_DEVICE_H

#include "../../common/dispatch.h"

// not done at all :) BU

class svc_device : public Dispatchable {
public:
  const char *getDeviceName();

  CfgItem *getCfgItem();

  int getSpace(unsigned int *space, unsigned int *spacefree);

  int reformat();

  // return a handle
  int putFile(const char *filename, const void *data, unsigned int length);
  int readFile(const char *filename, void *buffer, unsigned int offset, unsigned int length)=0;
  int getFileAttrib(const char *filename, const char *name, char *buf, int len);
  int setFileAttrib(const char *filename, const char *name, const char *buf, int len);

  // playlist manipulation
  int playlistCreate(const char *playlist_name);
  int playlistDelete(const char *playlist_name);

  int playlistGetNumItems(const char *playlist_name);
  int playlistEnumItem(const char *playlist_name, char *playstring, int len);

  int playlistAppendItem(const char *playlist_name, const char *playstring);
  int playlistRemoveItem(const char *playlist_name, int position);
};

#endif
