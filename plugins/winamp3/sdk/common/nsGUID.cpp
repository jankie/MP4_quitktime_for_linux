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

#include "nsGUID.h"

#include <stdio.h>

// Remember what a GUID is, goofball?
#include "GUID.h"

//
//	Yes I'm a lazy twirp.  I couldn't find any useful textparsing
//	stuff, but I know there HAS to be some, somewhere.
//
//	So, instead, for now, I'm just using printf and scanf
//	to read and write GUIDs as human readable strings.
//
char *	nsGUID::toChar( const GUID &guid, char *target ) {
	// Better hope your array has enough room, bucko!

	// {1B3CA60C-DA98-4826-B4A9-D79748A5FD73}
	sprintf( target, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		(int)guid.Data1, (int)guid.Data2, (int)guid.Data3,
		(int)guid.Data4[0], (int)guid.Data4[1], 
		(int)guid.Data4[2], (int)guid.Data4[3], 
		(int)guid.Data4[4], (int)guid.Data4[5], 
		(int)guid.Data4[6], (int)guid.Data4[7] );

	return target;
}

GUID		nsGUID::fromChar( const char *source ) {
	GUID guid;
	int Data1, Data2, Data3;
	int Data4[8];

	// {1B3CA60C-DA98-4826-B4A9-D79748A5FD73}
	sscanf( source, " { %08x - %04x - %04x - %02x%02x - %02x%02x%02x%02x%02x%02x } ",
		&Data1, &Data2, &Data3, Data4 + 0, Data4 + 1, 
		Data4 + 2, Data4 + 3, Data4 + 4, Data4 + 5, Data4 + 6, Data4 + 7 );

	// Cross assign all the values.  Yes, this is slow too.
	guid.Data1 = Data1;
	guid.Data2 = Data2;
	guid.Data3 = Data3;
	guid.Data4[0] = Data4[0];
	guid.Data4[1] = Data4[1];
	guid.Data4[2] = Data4[2];
	guid.Data4[3] = Data4[3];
	guid.Data4[4] = Data4[4];
	guid.Data4[5] = Data4[5];
	guid.Data4[6] = Data4[6];
	guid.Data4[7] = Data4[7];

	// and spit it out.
	return guid;
}

char *	nsGUID::toCode( const GUID &guid, char *target ) {
	// Better hope your array has enough room, bucko!

	//{ 0x1b3ca60c, 0xda98, 0x4826, { 0xb4, 0xa9, 0xd7, 0x97, 0x48, 0xa5, 0xfd, 0x73 } };
	sprintf( target, "{ 0x%08x, 0x%04x, 0x%04x, { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x } };",
		(int)guid.Data1, (int)guid.Data2, (int)guid.Data3,
		(int)guid.Data4[0], (int)guid.Data4[1], 
		(int)guid.Data4[2], (int)guid.Data4[3], 
		(int)guid.Data4[4], (int)guid.Data4[5], 
		(int)guid.Data4[6], (int)guid.Data4[7] );

	return target;
}

GUID		nsGUID::fromCode( const char *source ) {

	GUID guid;
	int Data1, Data2, Data3;
	int Data4[8];

	//{ 0x1b3ca60c, 0xda98, 0x4826, { 0xb4, 0xa9, 0xd7, 0x97, 0x48, 0xa5, 0xfd, 0x73 } };
	sscanf( source, " { 0x%08x, 0x%04x, 0x%04x, { 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x } } ; ",
		&Data1, &Data2, &Data3, Data4 + 0, Data4 + 1, 
		Data4 + 2, Data4 + 3, Data4 + 4, Data4 + 5, Data4 + 6, Data4 + 7 );

	// Cross assign all the values.  Yes, this is slow too.
	guid.Data1 = Data1;
	guid.Data2 = Data2;
	guid.Data3 = Data3;
	guid.Data4[0] = Data4[0];
	guid.Data4[1] = Data4[1];
	guid.Data4[2] = Data4[2];
	guid.Data4[3] = Data4[3];
	guid.Data4[4] = Data4[4];
	guid.Data4[5] = Data4[5];
	guid.Data4[6] = Data4[6];
	guid.Data4[7] = Data4[7];

	// and spit it out.
	return guid;
}
