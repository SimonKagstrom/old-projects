/*********************************************************************
 * Copyright (C) 2001, 2003, 2006-2007-2004-2002  Simon Kagstrom
 *
 * Filename:      rex_sysdeps.c
 * Description:   This file contains the implementation of the
 *                REX 6000 sysdeps.
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
 * $Id: rex_sysdeps.c 15695 2007-07-02 10:58:43Z ska $
 *
 *********************************************************************/
#include <stdlib.h>         /* abs, rand */
#include <string.h>         /* strcpy */

#include "sysdeps.h"        /* Also includes the REX API files */

/* Local functions */
/* Return the current keypress */
#if defined(SDCC)
# define get_key() (IN(RX_REG_KI_DATA))
#else
# define get_key() (REGISTER_READ(REG_KI_DATA))
#endif /* SDCC */

/* Functions that needs implementing */
#if !FE_REX_OPT
void fe_clear_area(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  DsDisplayBlockClear(x, y, w, h);
}
#endif

#if !FE_REX_OPT
void fe_fill_area(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  DsDisplayBlock(x, y, w, h, 1);
}
#endif

#if !FE_REX_OPT
void fe_clear_screen(void)
{
  fe_clear_area(0,0, FE_PHYS_WIDTH, FE_PHYS_HEIGHT);
}
#endif

#if !FE_REX_OPT
void fe_set_pixel(uint8_t x, uint8_t y)
{
# if defined(SDCC) /* What to do here? */
  RxDisplayBlock(x, y, 1, 1, 1);
# else
  DsSetPixel(x,y);
# endif /* SDCC */
}
#endif

#if !FE_REX_OPT
void fe_clear_pixel(uint8_t x, uint8_t y)
{
# if defined(SDCC) /* What to do here? */
  RxDisplayBlockClear(x, y, 1, 1);
# else
  DsClearPixel(x,y);
# endif /* SDCC */
}
#endif

#define LINE_WIDTH (240 / 8)
/*
 * The code here looks stupid, but it's really just to get efficient
 * Z80 code with ZCC. This *only* works when the bitmaps are 8-bit
 * aligned in width.
 */
void fe_draw_bitmap(uint8_t *p_bitmap, uint8_t x, uint8_t y, uint8_t inverted)
{
  uint8_t w, h;
  int j;
  int x_off = x & 7;
  int b2_x_off = (8 - x_off);
  int byte_width;
  int byte_mask;
  uint8_t *p_dst_start = (uint8_t*)0xa000 + (x >> 3) + y * LINE_WIDTH;

  w = p_bitmap[0];
  h = p_bitmap[1];
  p_bitmap += 2;

  byte_width = w >> 3; /* w / 8, for ZCC */
  /* Invert to handle the more common case with only b1 valid without
   * doing the complement */
  byte_mask = ~((1 << x_off) - 1);

  if (inverted)
    inverted = 0xff;

  /* Iterate through the source */
  for (j = 0; j < h; j++)
    {
      uint8_t *b1 = p_dst_start + j * LINE_WIDTH;
      int y_offset = j * byte_width;
      int i;

      /* For every byte */
      for (i = 0; i < byte_width; i++, b1++)
	{
          uint8_t bitmap_byte = p_bitmap[y_offset + i] ^ inverted;

	  /* x == 4, x_off = 4
	   * What is the order of the bits on the screen?
	   *
	   *       Case 1                  Case 2
	   *              111111                 111111
	   *  01234567  89012345     01234567  89012345
	   *
	   *  01234567  01234567     76543210  76543210
	   *  ________  ________     ________  ________
	   * |    XXXX||XXXX    |   |XXXX    ||    XXXX|
	   * |    XXXX||XXXX    |   |XXXX    ||    XXXX|
	   * |    XXXX||XXXX    |   |XXXX    ||    XXXX|
	   * |____XXXX||XXXX____|   |XXXX____||____XXXX|
	   *   b1          b2         b1          b2
           *
           *
           * x == 3, case 1
	   *              111111
	   *  01234567  89012345
	   *
	   *  01234567  01234567
	   *  ________  ________
	   * |   XXXXX||XXX    |
	   * |   XXXXX||XXX    |
	   * |   XXXXX||XXX    |
	   * |___XXXXX||XXX____|
	   *   b1          b2
	   *
	   * X is the current source byte, b1 and b2 the two
	   * destination bytes. We assume that the bitmap is in-bounds
	   * (i.e. we never cross to the next destination line).
	   */

	  /* Case 2 */
	  *b1 = (*b1 & byte_mask) | (bitmap_byte >> x_off);
          if (x_off != 0)
            {
              uint8_t *b2 = b1 + 1;

              *b2 = (*b2 & ~byte_mask) | (bitmap_byte << b2_x_off);
            }
	}
    }
}

#if !FE_REX_OPT
uint16_t fe_get_buttons(void)
{
  return get_key();
}
#endif

#if !FE_REX_OPT
uint8_t fe_get_stylus(fe_point_t *p_point)
{
  /* Get data from touch screen */
  return DsTouchDataGet(p_point);
}
#endif

#if !FE_REX_OPT
void fe_print_xy(uint8_t x, uint8_t y, uint8_t mode, char *p_str)
{
  DsPrintf(x, y, mode, p_str);
}
#endif

/* Load data from a memo. This is not perfect. */
uint8_t fe_load_data(char *p_dst, uint16_t offset, uint16_t len, char *p_filename)
{
#if defined(SDCC)
#define IDX_MEMO_TITLE 2
#define MEMO_TITLE     2
#define MEMO_BODY      6
  RxDbHandle db_handle;
  uint8_t ret=0;

  if ( !(db_handle = RxDbOpen(RX_DBID_MEMO)) )
    ret = 1;
  else
    {
      char name[40];

      /* The filename needs to be on the stack */
      strcpy(name, p_filename);
      /* Lookup memo */
      if (RxDbFindRecord(db_handle, IDX_MEMO_TITLE, 1, name, 0, 0) == 1)
	{
	  uint16_t nchars;
	  volatile char *p_name = (char*)0xffff;

	  /* Check that the title matches */
	  if (RxDbReadField(db_handle, MEMO_TITLE, &p_name) < 0)
	    {
	      ret = 3;
	      goto out;
	    }
	  if (p_name == (char*)0xffff)
	    fe_print_xy(0, 10, 0, "p_name unchanged!");
	  else
	    fe_print_xy(0, 10, 0, p_name);

	  if (strcmp(p_name, name) != 0)
	    ret = 5;
	  else if (RxDbReadText(db_handle, MEMO_BODY, offset, len, &nchars, p_dst) != 0)
	    ret = 2;
	}
      else
	ret = 4;
    }

 out:
  RxDbClose(db_handle);

  return ret;
#else
  sint16_t db_handle;
  uint8_t ret=0;

  if ( !(db_handle = DbOpen(DBID_MEMO)) )
    ret = 1;
  else
    {
      char *p_name;
      char name[40];

      /* The filename needs to be on the stack */
      strcpy(name, p_filename);

      /* Lookup memo, read title */
      if ( (DbFindRecord(db_handle, IDX_MEMO_TITLE, 1, name, 0, 0) == 1) &&
	   (DbReadField(db_handle, MEMO_TITLE, &p_name) >= 0) )
	{
	  uint16_t nchars;

#if 0
	  if (strcmp(p_name, name) != 0)
	    ret = 5;
	  else
	    {
	      if (DbReadText(db_handle, MEMO_BODY, offset, len, &nchars, p_dst) < 0)
		ret = 2;
	    }
#else
	  /* Name matches, read text */
	  if (DbReadText(db_handle, MEMO_BODY, offset, len, &nchars, p_dst) < 0)
	    ret = 2;
#endif
	}
      else
	ret = 4;
    }

  DbClose(db_handle);

  return ret;
#endif /* SDCC */
}

#if !FE_REX_OPT
void fe_init()
{
  DsEventClear(); /* Clear events */
  DsClearScreen(); /* Clear the display */
}
#endif

#if !FE_REX_OPT
void fe_finalize()
{
  DsClearScreen();
  DsEventClear();
  DsAddinTerminate();
}
#endif

#if !FE_REX_OPT
void fe_sleep(uint16_t ticks)
{
  DsSleep(ticks);
}
#endif
