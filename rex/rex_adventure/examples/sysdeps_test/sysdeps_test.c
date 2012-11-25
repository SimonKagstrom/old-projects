/*********************************************************************
 * Copyright (C) 2002, 2007-2003,  Simon Kagstrom
 *
 * Filename:      sysdeps_test.c
 * Description:   A test program for the sysdeps.
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
 * $Id: sysdeps_test.c 15695 2007-07-02 10:58:43Z ska $
 *
 *********************************************************************/
#include <string.h>       /* memset */

#include "typedefs.h"
#include "sysdeps.h"
#include "bitmaps.h"

void test_blit(void)
{
  uint8_t img2[10];

  /* Stack-allocated image */
  img2[0]=8; img2[1]=8;
  img2[2]=0x7e; img2[3]=0xdb; img2[4]=0xff; img2[5]=0xff; img2[6]=0x7e; img2[7]=0x3c; img2[8]=0x1c; img2[9]=0x08;


  /* Blit some images at different places (see bitmaps.h) */
  fe_draw_bitmap(p_player, 50,60, 0); /* Unaligned, 8 pixels wide */
  fe_draw_bitmap(p_player, 51,72, 0); /* Aligned, 8 pixels wide */
  fe_draw_bitmap(img2, 64,64, 1); /* Aligned, 8 pixels wide, inverted */
  fe_draw_bitmap(p_grass, 16,16, 0);  /* Aligned position, 16 wide */

  fe_draw_bitmap(p_player, 48,30, 0); /* Aligned, 8 pixels wide */
  fe_draw_bitmap(p_grass, 65,19, 1); /* Unaligned, 16 pixels wide, inverted */
  fe_draw_bitmap(p_grass, 68,79, 1); /* Unaligned, 16 pixels wide, inverted */
  fe_draw_bitmap(p_grass, 168,79, 1); /* Unaligned, 16 pixels wide, inverted */

  fe_draw_bitmap(p_error_bitmap, 85,19, 0); /* Unaligned, 20 pixels wide (should not work) */
}

void test_pixel(void)
{
  fe_set_pixel(181, 82);
  fe_set_pixel(182, 82);
  fe_set_pixel(183, 82);
  fe_set_pixel(184, 82);

  fe_clear_pixel(182, 81);
  fe_clear_pixel(183, 81);
}

void test_printf(void)
{
  char buf[25];

  fe_print_xy(0, 100, 0, "Tjohopp"); /* Constant string */
  buf[0] = 'h'; buf[1] = 'e'; buf[2] = 'j'; buf[3] = '\0';
  fe_print_xy(70, 100, 0, buf); /* Buffer string */
}

void test_memo_read(void)
{
  char buf[25];
  uint8_t ret;

  memset(buf,0,25);

  if ( (ret = fe_load_data(buf, 0, 25, "test_scene")) )
    {
      /* ERROR! */
      fe_print_xy(0,0,0, "** File read failed! **: ");
      buf[0] = ret+'0';
      buf[1] = '\0';
      fe_print_xy(150,0,0, buf);
      return;
    }
  fe_print_xy(0,0,0, buf);
}


int main(void)
{
  /* Initialise the system dependent things */
  fe_init();

  test_printf();
  test_blit();
  test_pixel();
  test_memo_read();

  /* Wait for keypress */
  while ( !(fe_get_buttons() & FE_EVENT_EXIT) )
    fe_sleep(12);

  /* Finalize system-dependent things */
  fe_finalize();

  return 0;
}
