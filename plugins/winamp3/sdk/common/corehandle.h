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

#ifndef _COREHANDLE_H
#define _COREHANDLE_H

#include "../studio/corecb.h"

// a helper class to hide the API for playback cores within an object for you

typedef unsigned int CoreToken;

// Fwd References
class CfgItem;

class CoreHandle : public CoreCallbackI {
public:
  enum { maincore_token=0 };

  CoreHandle(CoreToken token=maincore_token);
  virtual ~CoreHandle();

  int setNextFile(const char *playstring);

  void prev();
  void play();
  void pause();
  void stop();
  void next();

//  int getPlayingStatus();	// -1 paused, 0 stopped, 1 playing

private:
  void userButton(int button);
  CoreToken token;
};

#endif
