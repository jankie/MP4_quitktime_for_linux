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

#ifndef _NOTIFMSG_H
#define _NOTIFMSG_H

// these are in their own header file to keep from having to muck with
// basewnd.h all the time to add a new global message

// messages for childNotify
// THESE MUST NOT BE USED ACROSS API OR DLLs!!!!
enum {
  CHILD_NOP,

  CHILD_DELETED,	// when child window is in its destructor

  CHILD_GOTFOCUS,
  CHILD_KILLFOCUS,

  // for transient child windows
  CHILD_RETURN_CODE,		// for self-destructing modal windows

  // for children of frame-type windows to resize themselves
  CHILD_SETTITLEWIDTH,
//CUT
  CHILD_HIDEYHIDEY,	// hidey!
  CHILD_UNHIDEYHIDEY,	// unhidey!
//CUT

  // when buttons are pushed they send this to their parent BaseWnd
  CHILD_NOTIFY_LEFTPUSH,
  CHILD_NOTIFY_RIGHTPUSH,
  CHILD_NOTIFY_LEFTDOUBLECLICK,
  CHILD_NOTIFY_RIGHTDOUBLECLICK,

  // edit wnd data change notifies
  CHILD_EDIT_DATA_MODIFIED,	// user edit
  CHILD_EDIT_CANCEL_PRESSED, // esc pressed
  CHILD_EDIT_ENTER_PRESSED, // enter pressed

  // windowshading
  CHILD_WINDOWSHADE_CAPABLE,// return width of shade
  CHILD_WINDOWSHADE_ENABLE,
	CHILD_WINDOWSHADE_DISABLE,

  // droplist selection changed
  CHILD_SEL_CHANGED,		// data1 is new pos

  CHILD_NOTIFY_USERMESSAGEBASE,	// offset from here if you define usermessages

  CHILD_LISTWND_SELCHANGED,  // listwnd events
  CHILD_LISTWND_DBLCLK,
  CHILD_LISTWND_POPUPMENU,

  FRAMEWND_QUERY_SLIDE_MODE,
  FRAMEWND_SET_SLIDE_MODE,

  NUM_NOTIFY_MESSAGES
};

#endif
