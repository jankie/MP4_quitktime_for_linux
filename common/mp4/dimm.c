/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2000, 2001.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Dave Mackie		dmackie@cisco.com
 */

#include "quicktime.h"


int quicktime_dimm_init(quicktime_dimm_t *dimm)
{
	dimm->numBytes = 0;
}

int quicktime_dimm_delete(quicktime_dimm_t *dimm)
{
}

int quicktime_dimm_dump(quicktime_dimm_t *dimm)
{
	printf("    total immediate bytes\n");
	printf("     numBytes %llu\n", dimm->numBytes);
}

int quicktime_read_dimm(quicktime_t *file, quicktime_dimm_t *dimm)
{
	dimm->numBytes = quicktime_read_int64(file);
}

int quicktime_write_dimm(quicktime_t *file, quicktime_dimm_t *dimm)
{
	quicktime_atom_t atom;

	quicktime_atom_write_header(file, &atom, "dimm");

	quicktime_write_int64(file, dimm->numBytes);
	
	quicktime_atom_write_footer(file, &atom);
}

