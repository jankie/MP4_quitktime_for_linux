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


#include "../../jnetlib/jnetlib.h"
#include "../../jnetlib/httpget.h"

#include "urlgrab.h"

#include "std.h"

#include "../studio/api.h"

UrlGrab::UrlGrab(const char *url, const char *filename) {
  dns = new JNL_AsyncDNS();
  httpGetter = new JNL_HTTPGet(dns);
  bytes_read = 0;

  char tmp[WA_MAX_PATH];
  SPRINTF(tmp,"User-Agent:%s build %i (Mozilla)", api->main_getVersionString(), api->main_getBuildNumber());
  httpGetter->addheader(tmp);
  httpGetter->addheader("Accept:*/*");
  httpGetter->connect(url);

  tmpfilename = filename;
  fp = fopen(filename, "wb");
  ready = 0;
}

UrlGrab::~UrlGrab() {
  if (ready != 1 && tmpfilename.getValue() != NULL) {
    unlink(tmpfilename.getValue());
  }
  shutdown();
//  if (tmpfilename.getValue() != NULL) unlink(tmpfilename);
}

int UrlGrab::run() {
  if (ready != 0) return ready;
  if (!httpGetter) return 1;
  int r = httpGetter->run();
  if (r == -1) {
    shutdown();
    ready = -1;
    return -1;
  }
  while (httpGetter->bytes_available() > 0) {
    char buf[4096];
    int bytesread = httpGetter->get_bytes(buf, 4096);
    bytes_read += bytesread;
    fwrite(buf, bytesread, 1, fp);
  }
  if (r == 1) {
    shutdown();
    ready = 1;
  }
  return ready;
}

int UrlGrab::shutdown() {
  delete httpGetter; httpGetter = NULL;
  delete dns; dns = NULL;
  if (fp != NULL) fclose(fp); fp = NULL;
  return 1;
}
