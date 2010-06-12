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

#ifndef BLOCKDB_H
#define BLOCKDB_H

#include "common.h"
#include "chksum.h"
#include "chksumdb.h"

int blockdb_fd;

void init_block_db () {
  blockdb_fd = open(BLOCK_DB_PATH, O_CREAT | O_LARGEFILE | O_SYNC | O_RDWR,
		    S_IRUSR | S_IWUSR);
  if (blockdb_fd < 0) {
    perror("Error in init_block_db.");
    fprintf(stderr, "Error in init_block_db: open(2) returned %d. BLOCK_DB_PATH was %s. Exiting.\n",
	    blockdb_fd, BLOCK_DB_PATH);
    _exit(-1);
  }
}

off64_t add_block (const char * blk) {
  int i, j, k, c;
  uint16_t chksum = checksum(blk);
  entry ent = entries[chksum];
  for (j = 0; j < ent.number; j++) {
    off64_t elt = ent.array[j];
    char mapped[BLOCKSIZE];
    lseek64 (blockdb_fd, elt, SEEK_SET);
    for (k = 0; k < BLOCKSIZE;) {
      k += (c = read(blockdb_fd, mapped + k, BLOCKSIZE - k));
      if (c <= 0) {
	fprintf(stderr,
		"Error in add_block: read(2) returned %d. Exiting.\n", c);
	_exit(-1);}}
    if (memcmp (blk, mapped, BLOCKSIZE) == 0) {
      return elt;}}
  /* no deduplication possible, allocate new block */
  off64_t ret = lseek64 (blockdb_fd, (off64_t) 0, SEEK_END);
  if (ret < 0) {
    perror("Error in add_block.");
    fprintf(stderr, "Error in add_block: lseek64(3) returned %d. Exiting.\n",
	    (int) ret); _exit(-1);}
  for (k = 0; k < BLOCKSIZE;) {
    k += (c = write(blockdb_fd, blk + k, BLOCKSIZE - k));
    if (c < 0) {
      perror("Error in add_block.");
      fprintf(stderr, "Error in add_block: write(2) returned %d. Exiting.\n",
	      c); _exit(-1);}}
  add_checksum(chksum, ret);
  return ret;
}

#endif
