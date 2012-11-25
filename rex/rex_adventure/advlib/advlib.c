/*********************************************************************
 * Copyright (C) 2001-2002 Simon Kagstrom
 *
 * Filename:      advlib.c
 * Description:   This file contains the implementation of the
 *                AdvLib library.
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
 * $Id: advlib.c 6264 2006-01-21 09:46:06Z ska $
 *
 *********************************************************************/
#include "typedefs.h"
#include "sysdeps.h"
#include "advlib/advlib.h"

/* Temporary definitions - Undef on Release*/
#undef STATIC_FILE
#undef ADV_DEBUG

/* File-local definitions */
#define PIC_W 0
#define PIC_H 1

#ifndef min
#define min(a,b) (((a) < (b))?(a):(b))
#endif /* min */

#ifndef max
#define max(a,b) (((a) > (b))?(a):(b))
#endif /* min */


/* Debugging things */
#if defined(ADV_DEBUG)
#include <stdio.h>   /* printf */
#endif /* ADV_DEBUG */

#if defined(STATIC_FILE)
#include "string.h"  /* memcpy */
static char level_data[] = {
  'A','A','A','A','A','B','C','D','A','A','F','E','A',
  'A','F','E','A','A','B','C','D','E','A','A','A','A',
  'A','A','A','A','A','B','C','D','A','A','A','A','A',
  'A','E','A','A','A','B','C','D','E','A','A','A','E',
  'F','A','A','F','A','B','C','D','A','A','F','A','A',
  'E','E','E','F','F','E','E','E','A','A','A','A','E',
};
#endif /* STATIC_FILE */

/* Protoypes for non-exported functions */
static unsigned char get_tile(adv_scene_t *p_scene, sint16_t x, sint16_t y);
static uint8_t *get_tile_img_ptr(adv_scene_t *p_scene, sint16_t x, sint16_t y);
static void draw_masked(adv_scene_t *p_scene, uint8_t *p_dst, uint8_t *p_mask, uint8_t *p_obj,
			sint16_t x, sint16_t y, uint8_t w, uint8_t h);

/* Return the bit-number of the nearest power of two higher then size */

/* Return the tile (char) at (x, h) */
static unsigned char get_tile(adv_scene_t *p_scene, sint16_t x, sint16_t y)
{
  /* We should probably replace the divisions with >>'s (and assume power-of-two sizes) */
  return p_scene->p_buf[(y/p_scene->th)*p_scene->w+(x/p_scene->tw)];
}

/* Return a pointer to the tile image at (x, y) */
static uint8_t *get_tile_img_ptr(adv_scene_t *p_scene, sint16_t x, sint16_t y)
{
  return p_scene->p_tile_buf+(get_tile(p_scene, x, y)-'A')*p_scene->scene_mult;
}

/* Mask part of a bitmap (two bytes here) */
static uint8_t mask_bitmap(uint8_t *p_obj, uint8_t *p_mask, sint16_t xpos, sint16_t ypos,
			   uint8_t y, uint8_t b1, uint8_t b2)
{
  /* FIXME: Maybe uninline this (to save some execution speed) */

  /* We AND with 7 here since we know that the player object is 8 bits wide. */
  return ( ( (b1<<(xpos & 7)) | (b2 >> (7-(xpos & 7))) ) & (p_mask?p_mask[y+2]:0xff) )
    | (p_obj?p_obj[y+2]:0x0);
}


/* Draw the p_obj on p_scene masked with p_mask. If p_mask is NULL,
 * nothing is masked away. If p_obj is NULL, nothing is added on the
 * background. The last two options are useful for drawing only the
 * background.
 *
 * This function is still buggy and can be optimized in lots of ways.
 */
static void draw_masked(adv_scene_t *p_scene, uint8_t *p_dst, uint8_t *p_mask, uint8_t *p_obj,
			sint16_t xpos, sint16_t ypos, uint8_t w, uint8_t h)
{
  uint8_t *p_img_ul = get_tile_img_ptr(p_scene, xpos,     ypos)+2;   /* Upper left tile (+2 to skip the w/h) */
  uint8_t *p_img_ur = get_tile_img_ptr(p_scene, xpos+w, ypos)+2;     /* Upper right tile */
  uint8_t *p_img_ll = get_tile_img_ptr(p_scene, xpos,     ypos+h)+2;
  uint8_t *p_img_lr = get_tile_img_ptr(p_scene, xpos+w, ypos+h)+2;
  uint8_t min_h     = min(p_scene->th-ypos%p_scene->th, h);
  uint8_t field_w   = p_scene->tw>>3;
  uint8_t left_offs = (xpos%p_scene->tw)>>3;
  uint8_t right_offs= ((xpos+w)%p_scene->tw)>>3;
  uint8_t y;

  /* Fill in p_dst with p_obj above the background.
   *
   * p_mask has 0s where the object is and 1s at the empty areas of
   * the object. The algorithm is as follows:
   *
   * 1. And the background with the mask (clears the bits where the
   *    object should be drawn).
   * 2. Inclusive or the object bytes with the background (sets the
   *    object bits).
   *
   * This is a hard nut to crack since the object might be offsetted
   * somewhere in the middle of the background-bytes. The worst case
   * is seen below, assuming the object is not wider or higher than
   * the background tiles.
   *  _______________
   * |ul     |ur     |
   * |       |    ___|___Object
   * |     ..|.../   |
   * |_____:_|__:____|
   * |ll   : |lr:    |
   * |     :.|..:    |
   * |       |       |
   * |_______|_______|
   *
   * Below is the implementation of the special case where the width
   * of the object is exactly 1 byte. For this situation, at most two
   * bytes overlap the object horizontally, making it simpler. Other
   * cases should be similar.
   */

  p_dst[PIC_W] = w;
  p_dst[PIC_H] = h;

  /* For every row on the upper half, create the out-byte from the two
   * possible background tiles (might be the same).
   */
  for (y=0; y<min_h; y++)
    {
      uint8_t height_offs = (y+ypos%p_scene->th);
      uint8_t b1 = p_img_ul[height_offs*field_w+left_offs];
      uint8_t b2 = p_img_ur[height_offs*field_w+right_offs];

      p_dst[y+2] = mask_bitmap(p_obj, p_mask, xpos, ypos, y, b1, b2);
    }

  /* Handle the lower part. We only do this if the upper and lower
   * images are not the same. This starts on y=min_h.
   */
  for (; y < h; y++)
    {
      uint8_t height_offs = (y-min_h);
      uint8_t b1 = p_img_ll[height_offs*field_w+left_offs];
      uint8_t b2 = p_img_lr[height_offs*field_w+right_offs];

      p_dst[y+2] = mask_bitmap(p_obj, p_mask, xpos, ypos, y, b1, b2);
    }
}


/* Exported functions */
void adv_init_scene(adv_scene_t *p_scene, uint8_t *p_buf, sint16_t w, sint16_t h,
		    uint8_t nr_tiles, uint8_t *p_tile_buf)
{
  p_scene->p_buf = p_buf;
  p_scene->w = w;
  p_scene->h = h;
  p_scene->nr_tiles = nr_tiles;
  p_scene->p_tile_buf = p_tile_buf;
  p_scene->tw = p_tile_buf[PIC_W]; /* All images have the same size */
  p_scene->th = p_tile_buf[PIC_H];
  p_scene->scene_mult = p_scene->tw/8*p_scene->th+2;
}

void adv_load_scene(adv_scene_t *p_scene, char *p_filename, uint8_t scene)
{
  sint16_t scene_size = p_scene->w*p_scene->h;

#if defined(STATIC_FILE)
  memcpy(p_scene->p_buf, level_data, scene_size);
#else
  /* Load the data into the scene. If this fails, the program will exit. */
  fe_load_data(p_scene->p_buf, scene*(scene_size+1), scene_size, p_filename);
#endif /* STATIC_FILE */
}

void adv_mask_object(adv_scene_t *p_scene, uint8_t *p_dst, uint8_t *p_mask, uint8_t *p_obj,
		     sint16_t x, sint16_t y)
{
  draw_masked(p_scene, p_dst, p_mask, p_obj, x, y, p_obj[PIC_W], p_obj[PIC_H]);
}

void adv_mask_background(adv_scene_t *p_scene, uint8_t *p_dst, sint16_t x, sint16_t y,
			 uint8_t w, uint8_t h)
{
  draw_masked(p_scene, p_dst, NULL, NULL, x, y, w, h);
}

void adv_draw_scene(adv_scene_t *p_scene)
{
  int i;

  for (i=0; i<p_scene->w*p_scene->h; i++)
    fe_draw_bitmap(p_scene->p_tile_buf+(p_scene->p_buf[i]-'A')*p_scene->scene_mult,
		   (i%p_scene->w)*p_scene->tw, (i/p_scene->w)*p_scene->th, 0);
}

void adv_get_surroundings(adv_scene_t *p_scene, unsigned char *p_dst,
			  sint16_t x, sint16_t y, uint8_t w, uint8_t h)
{
  p_dst[0] = get_tile(p_scene, x, y); /* Upper left */
  p_dst[1] = get_tile(p_scene, x+w, y); /* Upper right */
  p_dst[2] = get_tile(p_scene, x, y+h); /* Lower left */
  p_dst[3] = get_tile(p_scene, x+w, y+h); /* Lower right */
}
