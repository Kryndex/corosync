/*
 * Copyright (c) 2002-2004 MontaVista Software, Inc.
 * Copyright (c) 2004 Open Source Development Lab
 * Copyright (c) 2006-2007 Red Hat, Inc.
 *
 * All rights reserved.
 *
 * Author: Steven Dake (sdake@redhat.com), Mark Haverkamp (markh@osdl.org)
 *
 * This software licensed under BSD license, the text of which follows:
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the MontaVista Software, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>

#include <corosync/corotypes.h>
#include <corosync/list.h>
#include <corosync/engine/logsys.h>
#include "util.h"

LOGSYS_DECLARE_SUBSYS ("MAIN");

/*
 * Compare two names.  returns non-zero on match.
 */
int name_match(cs_name_t *name1, cs_name_t *name2) 
{
	if (name1->length == name2->length) {
		return ((strncmp ((char *)name1->value, (char *)name2->value,
			name1->length)) == 0);
	} 
	return 0;
}

/*
 * Get the time of day and convert to nanoseconds
 */
cs_time_t clust_time_now(void)
{
	struct timeval tv;
	cs_time_t time_now;

	if (gettimeofday(&tv, 0)) {
		return 0ULL;
	}

	time_now = (cs_time_t)(tv.tv_sec) * 1000000000ULL;
	time_now += (cs_time_t)(tv.tv_usec) * 1000ULL;

	return time_now;
}

void _corosync_out_of_memory_error (void)
{
	assert (0==1);
	exit (EXIT_FAILURE);
}

void _corosync_exit_error (
	enum e_ais_done err, const char *file, unsigned int line)
{
	log_printf (LOGSYS_LEVEL_ERROR, "AIS Executive exiting "
		"with status %d at %s:%u.\n", err, file, line);
	logsys_fork_completed ();
	logsys_flush ();
	exit (err);
}

#define min(a,b) ((a) < (b) ? (a) : (b))

char *getcs_name_t (cs_name_t *name)
{
	static char ret_name[CS_MAX_NAME_LENGTH];

	/* if string is corrupt (non-terminated), ensure it's displayed safely */
	if (name->length >= CS_MAX_NAME_LENGTH || name->value[name->length] != '\0') {
		memset (ret_name, 0, sizeof (ret_name));
		memcpy (ret_name, name->value, min(name->length, CS_MAX_NAME_LENGTH -1));
		return (ret_name);
	}
	return ((char *)name->value);
}

char *strstr_rs (const char *haystack, int byte)
{
	const char *end_address = strchr (haystack, byte);
	if (end_address) {
		end_address += 1; /* skip past { or = */
		end_address += strspn (end_address, " \t");
	}

	return ((char *) end_address);
}

void setcs_name_t (cs_name_t *name, char *str) {
	strncpy ((char *)name->value, str, CS_MAX_NAME_LENGTH);
	if (strlen ((char *)name->value) > CS_MAX_NAME_LENGTH) {
		name->length = CS_MAX_NAME_LENGTH;
	} else {
		name->length = strlen (str);
	}
}

int cs_name_tisEqual (cs_name_t *str1, char *str2) {
	if (str1->length == strlen (str2)) {
		return ((strncmp ((char *)str1->value, (char *)str2,
			str1->length)) == 0);
	} else {
		return 0;
	}
}

