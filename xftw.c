/*
 * Copyright (C) 2015 Bryan Christ <bryan.christ@gmail.com>
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

#define _BSD_SOURCE         // for snprintf()

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <limits.h>

#include "list.h"
#include "xftw.h"

typedef struct _dir_record_s    dir_record_t;

struct _dir_record_s
{
    struct list_head    list;

    struct dirent       entry;
    struct FTW          ftwbuf;

#if _FILE_OFFSET_BITS == 64
    struct stat64       info;
#else
    struct stat         info;
#endif

    char                filepath[PATH_MAX + 1];
};


static int
_xftw_recurse_path(const char *parent_path, struct list_head *list);

static int
_xftw_stat_record(const char *path, dir_record_t *record, int flags);

int
xftw(const char *path, XFTWFunc callback, int nfds, int flags, void *anything)
{
    dir_record_t        *record;
    dir_record_t        *node;
    struct list_head    *pos;
    struct list_head    *safe_copy;


    char                *curr_dir = NULL;

    int                 retval = 0;
    int                 level = 0;
    int                 type = 0;

    if(path == NULL) return -1;
    if(callback == NULL) return -1;

    record = (dir_record_t*)calloc(1, sizeof(*record));
    retval = _xftw_stat_record(path, record, flags);

    // make sure path can be opened and it's a directory
    if(retval == -1) return -1;
    if(!S_ISDIR(record->info.st_mode)) return -1;

    // nfds is unused for now
    if(nfds < 1)
    {

#ifdef OPEN_MAX
        nfds = OPEN_MAX;
#else
        nfds = sysconf(_SC_OPEN_MAX);
#endif

    }

    snprintf(record->filepath, PATH_MAX, "%s", path);

    INIT_LIST_HEAD(&record->list);

    retval = _xftw_recurse_path(path, &record->list);

    // first loop builds a list of files and folder
    list_for_each_safe(pos, safe_copy, &record->list)
    {
        node = list_entry(pos, dir_record_t, list);

        retval = _xftw_stat_record(node->filepath, node, flags);

        if(retval == 0)
        {
            if(S_ISDIR(node->info.st_mode))
            {
                type = FTW_D;

                if(flags & FTW_CHDIR)
                {
                    curr_dir = (char*)calloc(1, PATH_MAX + 1);
                    getcwd(curr_dir, PATH_MAX);
                    chdir(node->filepath);
                }

                level++;
                _xftw_recurse_path(node->filepath, &record->list);
                level--;

                if((flags & FTW_CHDIR) && (curr_dir != NULL))
                {
                    //printf(curr_dir);
                    chdir(curr_dir);
                    free(curr_dir);
                    curr_dir = NULL;
                }
            }

            if(S_ISREG(node->info.st_mode)) type = FTW_F;
        }
        else
        {
            type = FTW_NS;
        }

        node->ftwbuf.level = level;
        callback(node->filepath, &node->info, type, &node->ftwbuf, anything);

        // Cleanup entry as we are done with it
        free(node);
    }

    // Now that loop is done the entire list is invalid
    INIT_LIST_HEAD(&record->list);

    // perform callback on root node
    type = FTW_D;
    callback(path, &record->info, type, &record->ftwbuf, anything);

    // Cleanup root node
    free(record);

    return 0;
}

int
_xftw_recurse_path(const char *parent_path, struct list_head *list)
{
    DIR                 *dir;
    dir_record_t        *record;
    struct dirent       *result;

    if(parent_path == NULL) return -1;
    if(strcmp(parent_path, ".") == 0) return 0;
    if(strcmp(parent_path, "..") == 0) return 0;

    dir = opendir(parent_path);

    if(dir == NULL)
    {
        // return -1 on a hard error
        if(errno == EMFILE) return -1;
        if(errno == ENOMEM) return -1;

        // return 0 on a "soft" error
        return 0;
    }

    do
    {
        record = (dir_record_t *)calloc(1, sizeof(dir_record_t));

        readdir_r(dir, &record->entry, &result);

        if(result == NULL)
        {
            free(record);
            break;
        }

        if(strcmp(record->entry.d_name, ".") == 0)
        {
            free(record);
            continue;
        }

        if(strcmp(record->entry.d_name, "..") == 0)
        {
            free(record);
            continue;
        }

        // TODO:  add some sanity checking here
        sprintf(record->filepath,"%s/%s", parent_path, record->entry.d_name);

        /*
            append new node to the linked list.  we employ the append method
            because we want to make sure the new items are later iterated on
            by the custom callback.
        */
        INIT_LIST_HEAD(&record->list);
        list_add_tail(&record->list, list);

    }
    while(result != NULL);

    closedir(dir);

    return 0;
}

int
_xftw_stat_record(const char *path, dir_record_t *record, int flags)
{
    int retval = 0;

#if _FILE_OFFSET_BITS == 64

    if(flags & FTW_PHYS)
        retval = lstat64(path, &record->info);
    else
        retval = stat64(path, &record->info);
#else

    if(flags & FTW_PHYS)
        retval = lstat(path, &record->info);
    else
        retval = stat(path, &record->info);

#endif

    return retval;
}
