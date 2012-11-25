/*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * File path:     writememo.c
 * Description:   Addin to write a memo for the emulator. This is
 *                completely based on Davrex' MemoWriter, so credit
 *                should be passed on to him.
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
 * $Id: writememo.c,v 1.4 2002/09/12 09:45:04 simon Exp $
 *
 ********************************************************************/
#include <rex/rex.h>       /* REX syscalls */
#include <rex/MemoDB.h>    /* MEMO_BODY etc */
#include <stdlib.h>        /* itoa */
#include <string.h>        /* strcpy etc */

#include "read.h"          /* read_memo */

#include "file.h"          /* The actual memo */

#define TEXT_SIZE 2048
#define NAME_SIZE 40

/* Wait for a keypress to exit. This is completely ripped from WriteMemo */
void wait_anykey(void)
{
  MSG msg;
  int f;

  f=1;
  while (f)
    {
      DsEventMessageGet(msg);
      switch (msg.message)
	{
	case MSG_DS_CLOSE:
	  f=0; break;
	case MSG_DS_KEY_DOWN:
	  f=0; break;
	case MSG_DS_TOUCH_DOWN:
	  break;
	case MSG_DS_COMMAND:
	  f=0;break;
	  break;
	}
    }
}

/* Write data to a memo */
int write_memo(char **pp_filedata, char *filename)
{
  char stktext[TEXT_SIZE];
  char stkname[NAME_SIZE];
  unsigned int offset = 0;
  int dbhandle;
  int i;

  DbBeginTransaction();
  if ( (dbhandle=DsDbOpen(DBID_MEMO)) == 0)
    return -1;

  /* Copy filename and text to the stack */
  memset(stkname, 0, NAME_SIZE);
  memset(stktext, 0, TEXT_SIZE);
  strcpy(stkname, filename);
  strcpy(stktext, pp_filedata[0]);

  /* Create a new entry in the memo table:
   *
   * record ID: 0               DB_RECID  (32 bits?)
   * name:      stkname         DB_VARSTR (string)
   * lock:      unlocked (255)  DB_INT8   (8 bits)
   * lines:     0               DB_INT16  (16 bits)
   * dummy:     0               DB_INT32  (32 bits)
   * text:      stktext         DB_VARSTR (string)
   *
   * Two dummy values ends, not sure why.
   */
  if (DbInsertRecord(dbhandle, 0L, stkname, -1, 0, 0L, stktext, 0, 0) != 0)
    return -2;

  if (DbFlush() != 0)
    return -3;

  /* Insert the text (except the first chunk) */
  for (i=1; i<NR_CHUNKS; i++)
    {
      offset += strlen(stktext);
      memset(stktext, 0, TEXT_SIZE);
      strcpy(stktext, pp_filedata[i]);

      if (DbInsertText(dbhandle, MEMO_BODY, offset, strlen(stktext), stktext) != 0)
	return -4;

      if (DbFlush() != 0)
	return -3;
    }

  DbClose(dbhandle);
  DbEndTransaction(1,0);

  return 0;
}

/* Try to find a memo, returns 0 if the operation succeeded or < 0 otherwise. */
int lookup_memo(char *name)
{
  unsigned short DbHandle;
  char *p_name;
  char str[NAME_SIZE];
  int ret=0;

  if ( !(DbHandle = DbOpen(DBID_MEMO)) )
    return -1;

  strcpy(str, name);

  /* Lookup memo */
  if (DbFindRecord(DbHandle, IDX_MEMO_TITLE, 1, str, 0, 0) == 1)
    {
      /* Check if the title matches */
      if (DbReadField(DbHandle, MEMO_TITLE, &p_name) < 0)
	ret = -3;
    }
  else
    ret = -4;

  /* Close the memo database */
  DbClose(DbHandle);

  return ret;
}



void main(void)
{
  int offset;
  int ret;
  int i;

  DsDisplayBlockClear(0, 0, 240, 120);
  DsPrintf(20,0,16,"Creating memo...");

  /* Try to write a memo */
  if ( (ret = write_memo(pp_file_data, FILENAME)) >= 0)
    {
      /* Success! */
      DsPrintf(10,20,32,"File written:");
      DsPrintf(100,20,32,FILENAME);
    }
  else
    {
      /* Failure! */
      DsDisplayBlockClear(0, 20, 240, 11);
      switch (ret)
	{
	case -1: DsPrintf(10,20,32,"Error open"); break;
	case -2: DsPrintf(10,20,32,"Error insert"); break;
	case -3: DsPrintf(10,20,32,"Error flush"); break;
	case -4: DsPrintf(10,20,32,"Error InsertText"); break;
	default: break;
	}
    }

  /* See if we can find the memo */
  if ( (ret = lookup_memo(FILENAME)) >= 0)
    DsPrintf(10,30,32,"Lookup succeeded!"); /* Success! */
  else
    {
      /* Failure */
      DsDisplayBlockClear(0, 30, 240, 11);
      switch (ret)
	{
	case -1: DsPrintf(10,30,32,"Lookup error DbOpen"); break;
	case -2: DsPrintf(10,30,32,"Lookup error DbReadText"); break;
	case -3: DsPrintf(10,30,32,"Lookup error DbReadField"); break;
	case -4: DsPrintf(10,30,32,"Lookup error DbFindRecord"); break;
	default: break;
	}
    }

  /* Read each chunk of data from the memo */
  offset = 0;
  for (i=0; i<NR_CHUNKS; i++)
    {
      int len = strlen(pp_file_data[i]);
      char buf[512];

      memset(buf, 0, 512);

      if ( (ret = read_memo(FILENAME, buf, offset, len)) >= 0)
	{
	  char str[NAME_SIZE];
	  int j;

	  /* Success! Display the data and the length of the memo */
	  DsDisplayBlockClear(0, 40, 240, 11);

	  DsPrintf(10,40,32,"Read OK, length: ");
	  itoa(str, offset+len);
	  DsPrintf(100,40,32, str);

	  DsDisplayBlockClear(0, 110, 240, 11);
	  DsPrintf(10,110,32, buf);

	  /* Verify that the data matches the input */
	  for (j=0; j<len; j++)
	    {
	      if (buf[j] != *(pp_file_data[i]+j))
		{
		  DsDisplayBlockClear(0, 40, 240, 11);
		  DsPrintf(10,40,32,"Data not consistent:");
		  goto out;
		}
	    }
	}
      else
	{
	  /* Failure! */
	  DsDisplayBlockClear(0, 40, 240, 11);
	  switch (ret)
	    {
	    case -1: DsPrintf(10,40,32,"Read error DbOpen"); break;
	    case -2: DsPrintf(10,40,32,"Read error DbReadText"); break;
	    case -3: DsPrintf(10,40,32,"Read error DbReadField"); break;
	    case -4: DsPrintf(10,40,32,"Read error DbFindRecord"); break;
	    case -5: DsPrintf(10,40,32,"Read error strcmp"); break;
	    default: break;
	    }
	}

      offset+=len;
    }
 out:
  /* Cleanup after the app */
  wait_anykey();

  DsEventClear();
  DsDisplayBlockClear(0, 0, 240, 120);
  DsAddinTerminate();
}
