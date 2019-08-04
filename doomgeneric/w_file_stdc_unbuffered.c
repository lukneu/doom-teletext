//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	WAD I/O functions.
//

#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "m_misc.h"
#include "w_file.h"
#include "z_zone.h"

typedef struct
{
    wad_file_t wad;
    int fd;
} stdc_wad_file_t;

extern wad_file_class_t stdc_wad_file;

static unsigned int W_StdC_FileLength(int fd)
{
    long savedpos;
    long length;

    // save the current position in the file
    savedpos = lseek(fd, 0, SEEK_CUR);
    
    // jump to the end and find the length
    length = lseek(fd, 0, SEEK_END);

    // go back to the old location
    lseek(fd, savedpos, SEEK_SET);

    return length;
}

static wad_file_t *W_StdC_OpenFile(char *path)
{
    stdc_wad_file_t *result;
    int fd;

    fd = open(path, O_RDONLY);

    if (fd <= 0)
    {
        return NULL;
    }

    // Create a new stdc_wad_file_t to hold the file handle.

    result = Z_Malloc(sizeof(stdc_wad_file_t), PU_STATIC, 0);
    result->wad.file_class = &stdc_wad_file;
    result->wad.mapped = NULL;
    result->wad.length = W_StdC_FileLength(fd);
    result->fd = fd;

    return &result->wad;
}

static void W_StdC_CloseFile(wad_file_t *wad)
{
    stdc_wad_file_t *stdc_wad;

    stdc_wad = (stdc_wad_file_t *) wad;

    close(stdc_wad->fd);
    Z_Free(stdc_wad);
}

// Read data from the specified position in the file into the 
// provided buffer.  Returns the number of bytes read.

size_t W_StdC_Read(wad_file_t *wad, unsigned int offset,
                   void *buffer, size_t buffer_len)
{
    stdc_wad_file_t *stdc_wad;
    size_t result;

    stdc_wad = (stdc_wad_file_t *) wad;

    // Jump to the specified position in the file.

    lseek(stdc_wad->fd, offset, SEEK_SET);

    // Read into the buffer.

    result = read(stdc_wad->fd, buffer, buffer_len);

    return result;
}


wad_file_class_t stdc_wad_file = 
{
    W_StdC_OpenFile,
    W_StdC_CloseFile,
    W_StdC_Read,
};


