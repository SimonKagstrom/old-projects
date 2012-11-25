/*********************************************************************
 *
 * Copyright (C) 2003, Simon Kagstrom <ska@bth.se>
 *
 * Filename:      sdl_ui.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Drawing functions
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
 * $Id:$
 *
 ********************************************************************/
#include <SDL/SDL.h>

static int size=2;

/* Helpers */
/* From the SDL documentation */
static Uint32 get_SDL_pixel(SDL_Surface *p_image, int x, int y)
{
  int bpp = p_image->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)p_image->pixels + y * p_image->pitch + x * bpp;

  switch(bpp)
    {
    case 1:
      return *p;
    case 2: /* This will cause some problems ... */
      return *(Uint16 *)p;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
	return p[0] << 16 | p[1] << 8 | p[2];
      else
	return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
      return *(Uint32 *)p;
    default:
      return 0;       /* shouldn't happen, but avoids warnings */
    }
}


/* Draw a black pixel */
void putpixel(SDL_Surface *p_where, int x, int y, int on)
{
  SDL_Rect r;

  /* Scale the pixels appropriately */
  r.x = x*size;
  r.y = y*size;
  r.w = size;
  r.h = size;

  /* Draw the pixels */
  if (on)
    SDL_FillRect(p_where, &r, SDL_MapRGB(p_screen->format, 0x00, 0x00, 0x00)); /* Black */
  else
    SDL_FillRect(p_where, &r, SDL_MapRGB(p_screen->format, 156, 167, 143)); /* Greenish */
}


/* Draw a rex bitmap */
void draw_rex_bitmap(SDL_Surface *p_where, const Uint8 *p_bitmap, Uint8 xpos, Uint8 ypos, Uint8 mode)
{
  Uint8 width = p_bitmap[0];
  Uint8 height = p_bitmap[1];
  SDL_Rect r;
  int x;
  int y;

  /* Scale the pixels appropriately */
  r.y = ypos*size;
  r.w = size;
  r.h = size;

  /* For each row */
  for (y=0; y<height; y++)
    {
      if (width>7)
	r.x = (xpos+(width+width%8-1))*size;
      else
	r.x = (xpos+7)*size;

      /* For all columns of bytes */
      for (x=(width/8+(width%8 > 0?1:0))-1; x >= 0; x--)
	{
	  Uint8 curr = p_bitmap[y*(width/8+(width%8 > 0?1:0))+x+2]; /* 2 skips the w/h fields */
	  Uint8 i;

	  /* For each bit */
	  for (i=0; i<8; i++)
	    {
	      /* If this bit is set - draw a black pixel there */
	      if (!mode)
		{
		  if ((curr & (1<<i)))
		    SDL_FillRect(p_where, &r, SDL_MapRGB(p_screen->format, 0x00, 0x00, 0x00)); /* Black */
		  else
		    SDL_FillRect(p_where, &r, SDL_MapRGB(p_screen->format, 156, 167, 143)); /* Greenish */
		}
	      else
		{
		  if ((curr & (1<<i)))
		    SDL_FillRect(p_where, &r, SDL_MapRGB(p_screen->format, 156, 167, 143)); /* Greenish */
		  else
		    SDL_FillRect(p_where, &r, SDL_MapRGB(p_screen->format, 0x00, 0x00, 0x00)); /* Black */
		}
	      r.x-=size;
	    }
	}
      r.y+=size;
    }
}
