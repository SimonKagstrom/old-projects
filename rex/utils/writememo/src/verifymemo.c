 /*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * File path:     verifymemo.c
 * Description:   Addin that loads a memo and verifies it's contents.
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
 * $Id: verifymemo.c,v 1.1 2002/09/12 09:47:24 simon Exp $
 *
 ********************************************************************/
#include <rex/rex.h>       /* REX syscalls */
#include <rex/MemoDB.h>    /* MEMO_BODY etc */
#include <stdlib.h>        /* itoa */
#include <string.h>        /* strcpy etc */

#include "read.h"          /* read_memo */

#include "file.h"          /* The actual memo */

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


void main(void)
{
  int offset;
  int ret;
  int i;

  DsDisplayBlockClear(0, 0, 240, 120);
  DsPrintf(20,0,16,"Looking up memo...");

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

