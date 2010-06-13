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

#ifndef CHKSUM_H
#define CHKSUM_H

#include "common.h"

/* Define the checksum (16-bit) for the blocks (512 byte) */

uint16_t checksum__(const char* blk) {
  /* for now, just sum the bytes up */
  uint16_t ret = 0;
  const char* ptr = blk;
  const char* ptr2 = blk+BLOCKSIZE;
  for (; ptr < ptr2; ptr++) {
    ret = (ret + *ptr) % (256*256);
  }
  return ret;
}

uint32_t checksum(const char* blk) {
  uLong ret = crc32(0L, Z_NULL, 0);
  ret = crc32(ret, blk, BLOCKSIZE);
  return (uint32_t) ret;
}

#endif
