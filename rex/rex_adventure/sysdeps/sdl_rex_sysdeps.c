/*********************************************************************
 * Copyright (C) 2001-2007 Simon Kagstrom
 *
 * Filename:      sdl_rex_sysdeps.c
 * Description:   This file contains the implementation of the
 *                libSDL-based sysdeps.
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
 * $Id: sdl_rex_sysdeps.c 15695 2007-07-02 10:58:43Z ska $
 *
 *********************************************************************/
#include <stdio.h>         /* printf */
#include <stdlib.h>        /* malloc */
#include <string.h>        /* strrchr etc */
#if defined(__GNUC__)
# define __USE_GNU
# include <errno.h>        /* program_invocation_name */
#endif /* __GNUC__ */
#include <time.h>          /* time */
#include <SDL/SDL.h>       /* The SDL library */
#include <SDL/SDL_ttf.h>   /* SDL_ttf */

#include "typedefs.h"
#include "sysdeps.h"

#define BACKGROUND_COLOR 0xff
#define MAX_COMMAND_DIR_LEN 255

#if defined(__unix__) || defined(__linux__)
# define DIR_SEP "/"
# define DIR_SEP_CHAR '/'
#else
# define DIR_SEP "\\" /* msdos */
# define DIR_SEP_CHAR '\\'
#endif


static uint8_t screen_data[(240 * 120) / 8];
static SDL_Surface *p_screen;     /* The framebuffer */
static TTF_Font *p_font;
static uint16_t left=FALSE;     /* Keys */
static uint16_t right=FALSE;
static uint16_t sel=FALSE;
static uint16_t back=FALSE;
static uint16_t exit_app=FALSE;
static uint8_t  size=3;         /* The size multiplication for drawing stuff */
static char command_dir[MAX_COMMAND_DIR_LEN];

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


/* Non-exported functions */
/* Draw a black pixel */
static void sdlfe_putpixel(Uint8 x, Uint8 y, Sint8 on)
{
  SDL_Rect r;

  /* Scale the pixels appropriately */
  r.x = x*size;
  r.y = y*size;
  r.w = size;
  r.h = size;

  /* Draw the pixels */
  if (on)
    SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, 0x00, 0x00, 0x00)); /* Black */
  else
    SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, 156, 167, 143)); /* Greenish */
}


/* Draw a rex bitmap */
static void sdlfe_draw_rex_bitmap(const Uint8 *p_bitmap, Uint8 xpos, Uint8 ypos, Uint8 mode)
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
		    SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, 0x00, 0x00, 0x00)); /* Black */
		  else
		    SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, 156, 167, 143)); /* Greenish */
		}
	      else
		{
		  if ((curr & (1<<i)))
		    SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, 156, 167, 143)); /* Greenish */
		  else
		    SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, 0x00, 0x00, 0x00)); /* Black */
		}
	      r.x-=size;
	    }
	}
      r.y+=size;
    }
}

/* Local functions */
static void sdlfe_handle_event(SDL_Event *p_event)
{
  switch( p_event->type )
    {
    case SDL_KEYDOWN:
      switch( p_event->key.keysym.sym )
	{
	case SDLK_LEFT:
	  left = FE_EVENT_LEFT;
	  break;
	case SDLK_RIGHT:
	  right = FE_EVENT_RIGHT;
	  break;
	case SDLK_SPACE:
	case SDLK_RCTRL:
	case SDLK_LCTRL:
	  sel = FE_EVENT_SELECT;
	  break;
	case SDLK_ESCAPE:
	  exit_app = FE_EVENT_EXIT;
	  break;
	case SDLK_BACKSPACE:
	  back = FE_EVENT_BACK;
	  break;
	default:
	  break;
	}
      break; /* SDL_KEYDOWN */
    case SDL_KEYUP:
      switch( p_event->key.keysym.sym )
	{
	case SDLK_LEFT:
	  left = FALSE;
	  break;
	case SDLK_RIGHT:
	  right = FALSE;
	  break;
	case SDLK_SPACE:
	case SDLK_RCTRL:
	case SDLK_LCTRL:
	  sel = FALSE;
	  break;
	case SDLK_BACKSPACE:
	  back = FALSE;
	  break;
	default:
	  break; /* Don't care. */
	}
      break; /* SDL_KEYUP */
    case SDL_QUIT:
      exit_app = TRUE;
      break;
    default:
      break;
    }
}

/* Implementation of required functionality */
void fe_clear_area(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  SDL_Rect r;

  r.x = x*size;
  r.y = y*size;
  r.w = w*size;
  r.h = h*size;
  SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, BACKGROUND_COLOR, BACKGROUND_COLOR, BACKGROUND_COLOR));
  SDL_Flip(p_screen);
}

void fe_fill_area(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  SDL_Rect r;

  r.x = x*size;
  r.y = y*size;
  r.w = w*size;
  r.h = h*size;
  SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, 0, 0, 0));
  SDL_Flip(p_screen);
}

void fe_clear_screen(void)
{
  fe_clear_area(0,0, FE_PHYS_WIDTH, FE_PHYS_HEIGHT);
}

void fe_set_pixel(uint8_t x, uint8_t y)
{
  sdlfe_putpixel(x,y, 1);
  SDL_Flip(p_screen);
}

void fe_clear_pixel(uint8_t x, uint8_t y)
{
  sdlfe_putpixel(x,y, 0);
  SDL_Flip(p_screen);
}

#define LINE_WIDTH (240 / 8)
/*
 * The code here looks stupid, but it's really just to get efficient
 * Z80 code with ZCC.
 */
void fe_draw_bitmap(uint8_t *p_bitmap, uint8_t x, uint8_t y, uint8_t inverted)
{
  uint8_t w, h;
  int j;
  int x_off = x & 7;
  int b2_x_off = (8 - x_off);
  int byte_width;
  int byte_mask;
  uint8_t *p_dst_start = ((uint8_t*)screen_data) + (x >> 3) + y * LINE_WIDTH;

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

  {
    uint8_t width = w;
    uint8_t height = h;
    int xpos,ypos;

    for (ypos = y; ypos < y + height; ypos++)
      {
        for (xpos = x; xpos < x + width; xpos++)
          {
            uint8_t curr = screen_data[ ypos * LINE_WIDTH + xpos/8];
            int bit_on = ( curr & (0x80 >> (xpos & 7)) ) != 0;

            if (bit_on)
              sdlfe_putpixel(xpos, ypos, 1);
            else
              sdlfe_putpixel(xpos, ypos, 0);
          }
      }
  }
  SDL_Flip(p_screen);
}
void _fe_draw_bitmap(uint8_t *p_bitmap, uint8_t xpos, uint8_t ypos, uint8_t mode)
{
#if defined SDCC || 1
  uint8_t width = p_bitmap[0];
  uint8_t height = p_bitmap[1];
  int byte_w = (width/8) + (width & 7 ? 1 : 0);
  int x,y;

  for (y=0; y<height; y++)
    {
      for (x=0; x<width; x++)
	{
	  uint8_t curr = p_bitmap[ (y * byte_w + x/8) + 2];
	  int bit_on = ( curr & (0x80 >> (x & 7)) ) != 0;

	  if (bit_on ^ mode)
	    fe_set_pixel(xpos + x, ypos + y);
	  else
	    fe_clear_pixel(xpos + x, ypos + y);
	}
    }
#else
#asm
#include "draw_bitmap.S"
#endasm
#endif /* SDCC */
}

void fe_print_xy(uint8_t x, uint8_t y, uint8_t mode, char *p_str)
{
  SDL_Surface *p_text;
  SDL_Color clr;
  int i, j;

  /* The font is red, the background... Some other color. */
  clr.r = 255;  clr.g = 0; clr.b = 0;

  if ( !(p_text = TTF_RenderText_Solid(p_font, p_str, clr)) )
    return; /* Empty string or error. */

  for (j=0; j<p_text->h; j++)
    {
      for (i=0; i<p_text->w; i++)
	{
	  Uint8 r,g,b;

	  SDL_GetRGB(get_SDL_pixel(p_text, i, j), p_text->format, &r, &g, &b);
	  if (r == 255)
	    sdlfe_putpixel(x+i, y+j, 1);
	}
    }

  /* Release the font memory */
  SDL_FreeSurface(p_text);
  SDL_Flip(p_screen);
}

#if 0
uint16_t fe_wait_event(game_t *p_game) /* Wait for the next event */
{
  SDL_Event event;

  if (SDL_WaitEvent(&event) == 0)
    {
      fprintf(stderr, "ERROR while waiting for SDL events\n");
      exit(3);
    }
  sdlfe_handle_event(&event);

  return left | right | sel | back | exit_app;
}
#endif

uint16_t fe_get_buttons(void)
{
  SDL_Event event;

  while( SDL_PollEvent(&event) )
    sdlfe_handle_event(&event);
  return left | right | sel | back | exit_app;
}

uint8_t fe_get_stylus(fe_point_t *p_point)
{
  Uint8 buttons;
  int x, y;

  SDL_PumpEvents();
  buttons = SDL_GetMouseState(&x, &y);
  if (buttons & SDL_BUTTON(1))
    {
      p_point->x = x/size;
      p_point->y = y/size;

      return TRUE;
    }

  return FALSE;
}

void fe_sleep(uint16_t ticks)
{
  if (ticks)
    SDL_Delay(ticks*15);
  else
    SDL_Delay(-1); /* Sleep forever */
}

uint8_t fe_load_data(char *p_dst, uint16_t offset, uint16_t len, char *p_filename)
{
  FILE *p_file;
  char namebuf[255];

  memset(namebuf, 0, 255);
  strcat(namebuf, p_filename);
  strcat(namebuf, ".txt");
  if (!(p_file = fopen(p_filename, "r")))
    {
      char buf[MAX_COMMAND_DIR_LEN*2];

      /* Try the directory of the executable as well. In non-GNU systems, this will check for /filename */
      memset(buf, 0, MAX_COMMAND_DIR_LEN*2);
      strncpy(buf, command_dir, MAX_COMMAND_DIR_LEN);
      strcat(buf, DIR_SEP);
      strncat(buf, namebuf, MAX_COMMAND_DIR_LEN-1);

      if ( !(p_file = fopen(buf, "r")) )
	{
	  fprintf(stderr, "fe_load_data: Could not open %s or %s\n", p_filename, buf);
	  return 1;
	}
    }
  fseek(p_file, offset, SEEK_SET);
  fread(p_dst, sizeof(uint8_t), len, p_file);

  return 0;
}

void fe_init()
{
  Uint32 options = SDL_HWSURFACE|SDL_DOUBLEBUF;
  SDL_Color p_colors[256];
  char *p;
  int i;

  p_colors[0].r=0x00; /* Foreground black */
  p_colors[0].g=0x00;
  p_colors[0].b=0x00;

  for (i=1; i<256; i++)
    {
      p_colors[i].r=156; /* Background greenish */
      p_colors[i].g=167;
      p_colors[i].b=143;
    }
  srand(time(NULL));

  /* Initialize SDL */
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
      exit(1);
    }
  atexit(SDL_Quit);

  /* Set video mode (8 bit color) */
  if ( (p_screen=SDL_SetVideoMode(FE_PHYS_WIDTH*size, FE_PHYS_HEIGHT*size, 8, options)) == NULL )
    {
      fprintf(stderr, "Couldn't set %dx%d video mode: %s\n", FE_PHYS_WIDTH, FE_PHYS_HEIGHT, SDL_GetError());
      exit(2);
    }
  /* Set the palette */
  SDL_SetPalette(p_screen, SDL_LOGPAL|SDL_PHYSPAL, p_colors, 0, 256);

  /* Fill the screen with white color */
  fe_clear_area(0, 0, FE_PHYS_WIDTH, FE_PHYS_HEIGHT);

  /* Set command_dir to the directory (relative to the working dir) of the executable */
#if defined(__GNUC__)
  strncpy(command_dir, program_invocation_name, MAX_COMMAND_DIR_LEN);
#endif  /* __GNUC__ */
  if ( (p = strrchr(command_dir, DIR_SEP_CHAR)) )
    p[0] = '\0';

  /* Initialise true-type font library. Arial 11pts looks approximately as the REX font. */
  TTF_Init();
  if ( !(p_font = TTF_OpenFont("arial.ttf", 11)) )
    {
      char buf[MAX_COMMAND_DIR_LEN*2];

      /* Try the directory of the executable as well. In non-GNU systems, this will check for /filename */
      memset(buf, 0, MAX_COMMAND_DIR_LEN*2);
      strncpy(buf, command_dir, MAX_COMMAND_DIR_LEN);
      strcat(buf, DIR_SEP);
      strncat(buf, "arial.ttf", MAX_COMMAND_DIR_LEN-1);

      if ( !(p_font = TTF_OpenFont(buf, 11)) )
	{
	  fprintf(stderr, "fe_load_data: Could not open %s or %s\n", "arial.ttf", buf);
	  exit(1);
	}
    }
}

void fe_finalize()
{
  exit(0);
}
