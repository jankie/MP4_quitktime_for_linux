/*
 * FAAD - Winamp3 plugin
 * Copyright (C) 2001 Menno
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: cnv_aacpcm.cpp,v 1.3 2001/10/15 17:49:49 menno Exp $
 */

#include "cnv_aacpcm.h"
#include "aacpcm.h"

static WACNAME wac;
WAComponentClient *the = &wac;

#include "sdk/studio/services/servicei.h"
static waServiceT<svc_mediaConverter, AacPcm> aacpcm;

// {3AF667AD-3CF8-459e-8C7C-BD8CD1D6F8C2}
static const GUID guid = 
{ 0x3af667ad, 0x3cf8, 0x459e, { 0x8c, 0x7c, 0xbd, 0x8c, 0xd1, 0xd6, 0xf8, 0xc2 } };


WACNAME::WACNAME() : CfgItemI("AAC files support")
{
#ifdef FORTIFY
    FortifySetName("cnv_aacpcm.wac");
    FortifyEnterScope();
#endif
}

WACNAME::~WACNAME()
{
#ifdef FORTIFY
    FortifyLeaveScope();
#endif
}

GUID WACNAME::getGUID()
{
    return guid;
}

void WACNAME::onRegisterServices()
{
    api->service_register(&aacpcm);
    api->core_registerExtension("*.aac", "AAC Files");
}

void WACNAME::onDestroy()
{
    api->service_deregister(&aacpcm);
    WAComponentClient::onDestroy();
}
