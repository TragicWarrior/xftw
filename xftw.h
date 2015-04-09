/*
 * Copyright (C) 2013 Bryan Christ <bryan.christ@mediafire.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

/*
    Summary

    This library approximates the behavior of the now obsolete
    POSIX ftw() and it's successor nftw().  Some nftw() analogs
    are implemented and some are unfisihed.  Unfortunately, there will
    never be compelete parity due to the intentional behavior of xftw().
    Wherever possible, ntfw() behavior is preserved.

    Rationale

    1) Some implmentations of ftw() and ntfw() rely on fts(3).  FTS
    is incompatible with _FILE_OFFSET_BITS = 64 on GNU libc systems.

    2) Unlike ftw() and ntfw(), xftw() allows the user to provide a data
    pointer that will passed down to the iterator callback.  This is
    quite useful when you need relay information down to the iterator
    layer and perform unique actions.

*/

#ifndef _XFTW_H_
#define	_XFTW_H_


// BSD systems don't have lstat64 & stat64 monikers and Apple uses 64-bit
// structures by default
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__APPLE__)

#define stat64  stat
#define lstat64 lstat

#endif


// included only for FTW data structure and defines
#include <ftw.h>


#if _FILE_OFFSET_BITS == 64

typedef int (* XFTWFunc) (const char *filepath,
            struct stat64 *info,
            int type, struct FTW *ftwbuf, void *anything);
#else


typedef int (* XFTWFunc) (const char *filepath,
            struct stat *info,
            int type, struct FTW *ftwbuf, void *anything);

#endif

int xftw(const char *filepath, XFTWFunc fn, int nfd, int flags, void *anything);

#endif	/* xftw.h */
