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

#ifndef _SERVICES_H
#define _SERVICES_H

// lower-case service names are reserved by Nullsoft for future use
// upper-case service names are for 3rd parties to extend the service system

namespace WaSvc {
  enum {
    NONE='none',
    DEVICE='dev',		// portable device	svc_device.h
    FILEREADER='fsrd',		// file system reader (disk, zip, http)
    FILESELECTOR='fsel',	// file selector	svc_filesel.h
    IMAGEGENERATOR='imgn',	// image generator	svc_imggen.h
    IMAGELOADER='imgl',		// image loader		svc_imgload.h
    ITEMMANAGER='imgr',		// item manager		svc_itemmgr.h
    PLAYLISTREADER='plrd',	// playlist reader
    PLAYLISTWRITER='plwr',	// playlist writer
    MEDIACONVERTER='conv', // media converter
    MEDIACORE='core',     // media core
    SCRIPTOBJECT='maki', // third party script object
//    TRANSLATOR='xlat',		// text translator
    WINDOWCREATE='wndc',	// window creator
    XMLPROVIDER='xmlp',		// xml provider
  };
};

#endif
