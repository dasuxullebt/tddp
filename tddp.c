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

#include "common.h"
#include "chksum.h"
#include "chksumdb.h"
#include "blockdb.h"

int main (int argc, char* argv[]) {
  chksum_db_path = argv[2];
  block_db_path = argv[3];
  if (strcmp(argv[1], "create") == 0) {
    init_block_db();
    int fd = open(CHKSUM_DB_PATH, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
      fprintf(stderr, "Error in main: open chksumdb returned %d. Exiting.\n",
	    fd);
    _exit(-1);
    }
    return 0;
  } else if (strcmp(argv[1], "addtape") == 0) {
    init_block_db();
    char* tapepath = argv[4];
    char* ctapepath = argv[5];
    int cdb = open(chksum_db_path, O_LARGEFILE | O_RDWR);
    if (cdb < 0) {
      fprintf(stderr, "Could not checksum-db. %d.\n", cdb);
      _exit(-1);
    }
    load_from_file(cdb);
    int tape = open(tapepath, O_LARGEFILE | O_RDONLY);
    if (tape < 0) {
      perror("Error opening tape.");
      fprintf(stderr, "Could not open tape %s. %d.\n", tapepath, tape);
      _exit(-1);
    }
    int ctape = open(ctapepath, O_LARGEFILE | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (ctape < 0) {
      fprintf(stderr, "Could not open ctape. %d.\n", ctape);
      _exit(-1);
    }
    /* WE DO NOT YET SUPPORT FILELENGTHS NOT MULTIPLE 512 */
    int k, c;
    char buf[BLOCKSIZE];
  beginread:
    for (k = 0; k < BLOCKSIZE;) {
      k += (c = read(tape, buf + k, BLOCKSIZE - k));
      if (c == 0) {
	dump_to_file(cdb);
	fprintf(stderr, "EOF of tape. Anything should be fine. Exiting.");
	return 0;
      } else if (c<0) {
	fprintf(stderr, "Error reading tape. %d\n", tape);
	_exit(-1);
      }
    }
    off64_t myblk = add_block(buf);
    for (k = 0; k < sizeof(off64_t);) {
      k += (c = write(ctape, &myblk + k, sizeof(off64_t) - k));
      if (c < 0) {
	fprintf(stderr, "Could not write ctape: %d\n", c);
	_exit(-1);
      }
    }
    goto beginread;
  } else if (strcmp(argv[1], "restoretape") == 0) {
    /* restoring only to stdout */
    char* ctapepath = argv[4];
    int ctape = open(ctapepath, O_LARGEFILE | O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
    if (ctape < 0) {
      fprintf(stderr, "Could not open ctape %s. %d.\n", ctapepath, ctape);
      _exit(-1);
    }
    init_block_db();

    off64_t wr;
    char blockread[BLOCKSIZE];
    while (read(ctape, &wr, sizeof(off64_t)) > 0) {
      lseek64 (blockdb_fd, wr, SEEK_SET);
      read(blockdb_fd, blockread, BLOCKSIZE);
      write(1, blockread, BLOCKSIZE);}
  }
}
