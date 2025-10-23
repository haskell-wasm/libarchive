/*-
 * Copyright (c) 2003-2007 Tim Kientzle
 * Copyright (c) 2012 Michihiro NAKAJIMA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "archive_platform.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "archive.h"
#include "archive_private.h"

/* A table that maps names to functions. */
static const
struct { const char *name; int (*setter)(struct archive *); } names[] =
{
	{ "b64encode",		archive_write_add_filter_b64encode },
#if !defined(__wasi__) || HAVE_LIBBZ2
	{ "bzip2",		archive_write_add_filter_bzip2 },
#endif
	{ "compress",		archive_write_add_filter_compress },
#if !defined(__wasi__)
	{ "grzip",		archive_write_add_filter_grzip },
#endif
#if !defined(__wasi__) || HAVE_LIBZ
	{ "gzip",		archive_write_add_filter_gzip },
#endif
#if !defined(__wasi__)
	{ "lrzip",		archive_write_add_filter_lrzip },
#endif
#if !defined(__wasi__) || HAVE_LIBLZ4
	{ "lz4",		archive_write_add_filter_lz4 },
#endif
#if !defined(__wasi__) || HAVE_LIBLZMA
	{ "lzip",		archive_write_add_filter_lzip },
	{ "lzma",		archive_write_add_filter_lzma },
	{ "lzop",		archive_write_add_filter_lzop },
#endif
	{ "uuencode",		archive_write_add_filter_uuencode },
#if !defined(__wasi__) || HAVE_LIBLZMA
	{ "xz",			archive_write_add_filter_xz },
#endif
	{ "zstd",		archive_write_add_filter_zstd },
	{ NULL,			NULL }
};

int
archive_write_add_filter_by_name(struct archive *a, const char *name)
{
	int i;

	for (i = 0; names[i].name != NULL; i++) {
		if (strcmp(name, names[i].name) == 0)
			return ((names[i].setter)(a));
	}

	archive_set_error(a, EINVAL, "No such filter '%s'", name);
	a->state = ARCHIVE_STATE_FATAL;
	return (ARCHIVE_FATAL);
}
