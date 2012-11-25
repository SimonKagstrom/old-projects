/*********************************************************************
 *
 * Copyright (C) 2001-2003,  Simon Kagstrom <ska@bth.se>
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
 * $Id: sdl_rex_sysdeps.c $
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

#include <sysdeps/sysdeps.h>

#define BACKGROUND_COLOR 0xff
#define MAX_COMMAND_DIR_LEN 255

#if defined(__unix__) || defined(__linux__)
# define DIR_SEP "/"
# define DIR_SEP_CHAR '/'
#else
# define DIR_SEP "\\" /* msdos */
# define DIR_SEP_CHAR '\\'
#endif


static SDL_Surface *p_screen;     /* The framebuffer */
static TTF_Font *p_font;
static uint16_t left=FALSE;     /* Keys */
static uint16_t right=FALSE;
static uint16_t sel=FALSE;
static uint16_t back=FALSE;
static uint16_t exit_app=FALSE;
static uint8_t  size=1;         /* The size multiplication for drawing stuff */
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

void fe_draw_bitmap(uint8_t *p_bitmap, uint8_t x, uint8_t y, uint8_t inverted)
{
  uint8_t byte_w;
  uint8_t w;
  uint8_t h;

  sdlfe_draw_rex_bitmap(p_bitmap, x,y, inverted);
  SDL_Flip(p_screen);
  return;

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
      for(i=w-1; i>=0; i--)
	{
	  /* Turn the pixel on if it is set in the bitmap and if non-inverted mode is selected */
	  if ( ((p_bitmap[(h<<byte_w) + (i>>3)] & (1 << ( ((w-1)-i) & 7))) >= 1) - inverted > 0)
	    sdlfe_putpixel(x+i,y+h, 1);
	  else
	    sdlfe_putpixel(x+i,y+h, 0);
	}
    }

  SDL_Flip(p_screen);
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
    SDL_Delay(ticks*5);
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
