/*********************************************************************
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      bitmaps.h
 * Description:   Bitmap data for the game.
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
 * $Id: bitmaps.h 6264 2006-01-21 09:46:06Z ska $
 *
 *********************************************************************/

#ifndef BITMAPS_H
#define BITMAPS_H

#include "typedefs.h"

/* FIXME: These should be const's (or moved to someplace else) */
uint8_t p_grass[] =
{
  /* Grass */
  16, 16,
  0x00, 0x00,
  0x00, 0x00,
  0x40, 0x00,
  0xb0, 0x08,
  0x00, 0x04,
  0x00, 0x04,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x40,
  0x00, 0x82,
  0x00, 0x80,
  0x10, 0x00,
  0x28, 0x00,
  0x00, 0x00,
  0x00, 0x00,
  0x00, 0x01,
};

uint8_t p_player[] =
{
  8, 8,
  0x7e,
  0xdb,
  0xff,
  0xff,
  0x7e,
  0x3c,
  0x1c,
  0x08,
};


uint8_t p_error_bitmap[] =
{
  20, 10,
  0x00, 0x00, 0x00,
  0x3f, 0xff, 0xc0,
  0x5c, 0x03, 0xa0,
  0x60, 0x00, 0x60,
  0x6f, 0xff, 0x60,
  0x60, 0x00, 0x60,
  0x4f, 0xff, 0x20,
  0x40, 0x00, 0x20,
  0x3f, 0xff, 0xc0,
  0x00, 0x00, 0x00,
};

#endif /* BITMAPS_H */
