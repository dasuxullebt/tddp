/* Copyright (C) 2010 Christoph-Simon Senjak

    This file is part of TDDP.

    TDDP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TDDP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TDDP.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMON_H
#define COMMON_H

#define _LARGEFILE64_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

/* BLOCKSIZE NEEDS TO BE <4096 IN BLOCKDB.H - CHECK IF CHANGED */
#define BLOCKSIZE 512
#define CHECKSUMSIZE 2


char *chksum_db_path, *block_db_path;

#define CHKSUM_DB_PATH (chksum_db_path)
#define BLOCK_DB_PATH (block_db_path)

#endif
