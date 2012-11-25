/*********************************************************************
 *
 * Copyright (C) 2001-2003,  Simon Kagstrom <ska@bth.se>
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
 * $Id: rex_sysdeps.c $
 *
 *********************************************************************/
#include <stdlib.h>         /* abs, rand */
#include <string.h>         /* strcpy */

#include <sysdeps/sysdeps.h> /* Also includes the REX API files */

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

#if !FE_REX_OPT
void fe_draw_bitmap(uint8_t *p_bitmap, uint8_t x, uint8_t y, uint8_t inverted)
{
#if 1
  uint8_t byte_w;
  uint8_t w;
  uint8_t h;

  /* Get the width and height of the image to draw */
  w = p_bitmap[0];
  h = p_bitmap[1];
  p_bitmap += 2;
  byte_w = (w>>3)-1; /* w/8-1 (to get the shift number below) */

  /* Draw each row of the bitmap */
  while (h > 0)
    {
      int i;

      h--;
      /* For every pixel in the row */
      for(i=0; i<w; i++)
	{
	  /* Turn the pixel on if it is set in the bitmap and if non-inverted mode is selected */
	  if ( ((p_bitmap[(h<<byte_w) + (i>>3)] & (1 << ( ((w-1)-i) & 7))) >= 1) - inverted > 0)
	    fe_set_pixel(x+i, y+h);
	  else
	    fe_clear_pixel(x+i, y+h);
	}
    }
#else
  DsDisplayBitmapDraw(x,y,p_bitmap,inverted);
#endif
}
#endif

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
#else /* !SDCC */
  sint16_t db_handle;
  uint8_t ret=0;

  if ( !(db_handle = DbOpen(DBID_MEMO)) )
    return 1;
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

	  /* Name matches, read text */
	  if (DbReadText(db_handle, MEMO_BODY, offset, len, &nchars, p_dst) < 0)
	    ret = 2;
	}
      else
	ret = 4;
    }

  DbClose(db_handle);

  return ret;
#endif /* !SDCC */
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
