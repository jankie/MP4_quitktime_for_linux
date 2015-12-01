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

#ifndef		_NSGUID_H
#define		_NSGUID_H


#	ifndef GUID_DEFINED // GUID Snippet
#	define GUID_DEFINED
//
//	This is the GUID, according to Microsoft.  
//
//	If you have a different compiler, either implement this
//	in an upstream header file, or make sure your compiler's
//	GUID primitive is the same as this one.
//
//	YOU HAVE BEEN WARNED.
//

							typedef struct _GUID
							{
									unsigned long Data1;
									unsigned short Data2;
									unsigned short Data3;
									unsigned char Data4[8];
							} GUID;

							#ifndef _REFCLSID_DEFINED
							#define REFGUID const GUID &
							#define _REFCLSID_DEFINED
							#endif

// This is the minimum amount of poo poo that you have to
// implement to duplicate microsof's GUID primitive.

#	endif // GUID Snippet

#include "common.h"

//
//
//	So, hmmm.  Now, I guess, we implement some conversion functions to allow
//	us to have GUIDs translatable to and from other data types.
//
//	Class static methods make good namespaces.
class COMEXP nsGUID
{
	public:
		// To the "Human Readable" character format.
		// {1B3CA60C-DA98-4826-B4A9-D79748A5FD73}
		static char *	toChar( const GUID &guid, char *target );
		static GUID		fromChar( const char *source );
		// To the "C Structure" character format.
		//	{ 0x1b3ca60c, 0xda98, 0x4826, { 0xb4, 0xa9, 0xd7, 0x97, 0x48, 0xa5, 0xfd, 0x73 } };
		static char *	toCode( const GUID &guid, char *target );
		static GUID		fromCode( const char *source );

  // strlen("{xx xxx xxx-xxxx-xxxx-xxxx-xxx xxx xxx xxx}"
  enum { GUID_STRLEN = 38 };
};





#endif	//_NSGUID_H
