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

#ifndef CHKSUMDB_H
#define CHKSUMDB_H

#include "common.h"

typedef struct {
  uint32_t number; /* how much ones are saved */
  uint32_t allocated; /* how much ones are allocated */
  off64_t* array;
} entry;

entry entries[256*256];

void init_empty_chksum_db () {
  bzero (entries, 256*256);}

void realloc_entry(uint16_t checksum) {
  if (entries[checksum].allocated == 0) entries[checksum].array = NULL;
  entries[checksum].allocated += 512;
  off64_t* newarray = (off64_t*)
    realloc(entries[checksum].array,
	    entries[checksum].allocated * sizeof(off64_t));
  if (newarray == NULL) {
    fprintf
      (stderr, "Error in realloc_entry: realloc returned NULL. Exiting.\n");
    _exit(-1);}
  entries[checksum].array = newarray;
}

void add_checksum(uint16_t checksum, off64_t offset) {
  if (entries[checksum].number < entries[checksum].allocated) {
    entries[checksum].array[entries[checksum].number++] = offset;
  } else {
    realloc_entry(checksum);
    add_checksum(checksum, offset);
  }
}


typedef struct {
  uint16_t checksum;
  uint32_t number;
} fileformat;

void dump_to_file (int fd) {
  /* file format: uint16_t checksum, uint32_t number, off64_t[number] entries,
     ... */
  int i,k,c;
  lseek64(fd, (off64_t) 0, SEEK_SET);
  for (i = 0; i < 256*256; i++) {
    /* this is not nice but I am too lazy to do it properly! */
    fileformat ff;
    ff.checksum = (uint16_t) i;
    ff.number = entries[i].number;
    if (ff.number == 0) continue;
    off64_t* myarray = entries[i].array;
    char writebuf[sizeof(fileformat)+
		  sizeof(off64_t)*ff.number];
    char* wb = writebuf;
    memcpy(wb, &ff, sizeof(fileformat)); wb+=sizeof(fileformat);
    memcpy(wb, myarray, ff.number*sizeof(off64_t));
    for (k = 0; k < sizeof(writebuf);) {
      k += (c = write(fd, writebuf + k, sizeof(writebuf) - k));
      if (c == 0) {
	fprintf (stderr, "Warning: Zero Write in dump_to_file.\n");
      } else if (c < 0) { fprintf 
	  (stderr, "Error in dump_to_file: Write returned %d. Exiting.\n", c);
	_exit(-1);}
    }
  }
}

void load_from_file (int fd) {
  init_empty_chksum_db ();
  int i,k,c;
  fileformat ff;
 readon:
  for (i = 0; i < sizeof(ff);) {
    i += (c = read(fd, &ff + i, sizeof(ff) - i));
    if (c == 0) {
      /* EOF */
      return;
    } else if (c < 0) {
      fprintf(stderr,
	      "Error in load_from_file: read returned %d. Exiting.\n", c);
      _exit(-1);}
  }
  /* CHAR !!! */
  char* newarray = (char*) malloc (ff.number*sizeof(off64_t));
  for (i = 0; i < ff.number;) {
    i += (c = read(fd, newarray + i, ff.number*sizeof(off64_t) - i));
    if (c <= 0) {
      fprintf(stderr,
	      "Error in load_from_file: read returned %d. Exiting.\n", c);
      _exit(-1);} 
  }
  entries[ff.checksum].number = entries[ff.checksum].allocated = ff.number;
  entries[ff.checksum].array = (off64_t*) newarray;
  goto readon;
}

#endif
