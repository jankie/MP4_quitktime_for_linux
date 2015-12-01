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
 * $Id: cnv_aacpcm.h,v 1.1 2001/10/11 12:57:09 menno Exp $
 */

#ifndef _CNV_AACPCM_H
#define _CNV_AACPCM_H

#include "sdk/studio/wac.h"
#include "sdk/common/rootcomp.h"
#include "sdk/attribs/cfgitemi.h"
#include "sdk/attribs/attrint.h"

#define WACNAME WACcnv_aacpcm

class WACNAME : public WAComponentClient, public CfgItemI
{
public:
    WACNAME();
    virtual ~WACNAME();

    virtual const char *getName() { return "AAC to PCM converter"; };
    virtual GUID getGUID();

    virtual void onRegisterServices();
    virtual void onDestroy();
    
    virtual int getDisplayComponent() { return FALSE; };

    virtual CfgItem *getCfgInterface(int n) { return this; }
};

#endif
