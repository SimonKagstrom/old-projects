 /*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * File path:     read.c
 * Description:   Read memo function.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 *
 * $Id: read.c,v 1.1 2002/09/12 09:46:03 simon Exp $
 *
 ********************************************************************/
#include <string.h>        /* strcpy */
#include <rex/rex.h>       /* REX syscalls */
#include <rex/MemoDB.h>    /* MEMO_BODY etc */

#define NAME_SIZE 40

/* Read data from a memo. Returns 0 if the operation succeeded and < 0 otherwise. */
int read_memo(char *name, char *p_dst, int offset, int len)
{
  unsigned short DbHandle;
  char *p_name;
  char str[NAME_SIZE];
  int nchars;
  int ret=0;

  if ( !(DbHandle = DbOpen(DBID_MEMO)) )
    return -1;

  strcpy(str, name);

  /* Lookup memo */
  if (DbFindRecord(DbHandle, IDX_MEMO_TITLE, 1, str, 0, 0) == 1)
    {
      /* Read the memo title */
      if (DbReadField(DbHandle, MEMO_TITLE, &p_name) < 0)
	ret = -3;
      else
	{
	  /* Check that the title matches and if so read the memo body */
	  if (strcmp(p_name, str) != 0)
	    ret = -5;
	  else if (DbReadText(DbHandle, MEMO_BODY, offset, len, &nchars, p_dst) != 0)
	    ret = -2;
	  else
	    ret = nchars;
	}
    }
  else
    ret = -4;

  /* Close the memo database */
  DbClose(DbHandle);

  return ret;
}
