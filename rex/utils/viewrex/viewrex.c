/*********************************************************************
 *                
 * Copyright (C) 2001,  Simon Kagstrom
 *                
 * Filename:      view_rex.c
 * Description:   This file contains the implementation of a
 *                REX image viewer.
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
 * $Log: viewrex.c,v $
 * Revision 1.3  2001/12/18 14:26:15  simon
 * Cleanup
 *
 * Revision 1.2  2001/12/17 14:47:33  simon
 * Cleanup, exit on any key
 *
 * Revision 1.1.1.1  2001/12/17 14:40:02  simon
 * Imported viewrex
 *
 *
 ********************************************************************/
#include <unistd.h>        /* getopt etc */
#include <stdio.h>         /* printf */
#include <stdlib.h>        /* malloc */
#include <SDL/SDL.h>       /* The SDL library */

#define PHYS_WIDTH       240
#define PHYS_HEIGHT      120
#define BACKGROUND_COLOR 0xff

#define TRUE  1
#define FALSE 0

SDL_Surface *p_screen;     /* The framebuffer */
Uint8 i_exit=FALSE;

/* Wait for events */
void handle_event(SDL_Event *p_event)
{
  switch( p_event->type )
    {
    case SDL_KEYDOWN:
      i_exit = TRUE;
      break; /* SDL_KEYDOWN */
    case SDL_QUIT:
      i_exit = TRUE;
      break;
    default:
      break;
    }
}

/* Draw a rex bitmap */
void draw_rex_bitmap(Uint8 *p_bitmap, Uint8 i_x, Uint8 i_y, Uint8 i_size)
{
  Uint8 i_width = p_bitmap[0];
  Uint8 i_height = p_bitmap[1];
  SDL_Rect r;
  int x;
  int y;

  r.y = i_y*i_size;
  r.w = i_size;
  r.h = i_size;

  /* For each row */
  for (y=0; y<i_height; y++)
    {
      if (i_width>7)
	r.x = (i_x+(i_width+i_width%8-1))*i_size;
      else
	r.x = (i_x+7)*i_size;

      /* For all columns of bytes */
      for (x=(i_width/8+(i_width%8 > 0?1:0))-1; x >= 0; x--)
	{
	  Uint8 i_curr = p_bitmap[y*(i_width/8+(i_width%8 > 0?1:0))+x+2]; /* 2 skips the w/h fields */
	  Uint8 i;

	  for (i=0; i<8; i++)
	    {
	      if (i_curr & (1<<i))
		SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, 0x00, 0x00, 0x00)); /* Black */
	      r.x-=i_size;
	    }
	}
      r.y+=i_size;
    }
}

/* Print help message */
void print_usage()
{
  printf("viewrex displays REX 6000 images from binary files\n\n"
	 "Usage:\n"
	 "viewrex [options] infile\n\n"
	 "Options are:\n"
	 "-s SIZE          Set the magnification level (default 1)\n"
	 "-2               Set the magnification level to 2\n"
	 "-4               Set the magnification level to 4\n"
	 );
}


int main(int argc, char *argv[])
{
  Uint32 i_options = SDL_HWSURFACE|SDL_DOUBLEBUF;
  SDL_Color p_colors[256];
  Uint8 i_size = 1;
  Uint8 *p_bitmap;
  SDL_Event event;
  FILE *p_infile;
  Uint8 i_w=0;
  Uint8 i_h=0;
  SDL_Rect r;
  int i;
  char c;

  while ((c=getopt(argc, argv, "h24s:")) != -1)
    {
      switch(c)
	{
	case 's':
	  i_size = atoi(optarg);
	  break;
	case '2':
	  i_size = 2;
	  break;
	case '4':
	  i_size = 4;
	  break;
	case 'h':
	  print_usage();
	  return 0;
	case '?':	  
	  break;
	default:
	  abort();
	}
    }
  if (argc < 2)
    {
      print_usage();
      return 0;
    }

  if ( !(p_infile = fopen(argv[argc-1], "r")) )
    {
      fprintf(stderr, "fopen: Could not open file");
      return -1;
    }
  if ( fread(&i_w, sizeof(Uint8), 1, p_infile) < 0)
    {
      fprintf(stderr, "fread: Could not read from file");
      return -1;
    }
  if ( fread(&i_h, sizeof(Uint8), 1, p_infile) < 0)
    {
      fprintf(stderr, "fread: Could not read from file");
      return -1;
    }
  if ( !(p_bitmap = calloc(sizeof(Uint8), i_h*(i_w/8+(i_w%8?1:0))+2 )) )
    {
      perror("calloc");
      return -1;
    }
  p_bitmap[0] = i_w;
  p_bitmap[1] = i_h;
  if ( fread(p_bitmap+2, sizeof(Uint8), i_h*(i_w/8+(i_w%8?1:0)), p_infile) < 0)
    {
      fprintf(stderr, "fread: Could not read from file");
      return -1;
    }

  p_colors[0].r=0x00; /* Foreground black */
  p_colors[0].g=0x00;
  p_colors[0].b=0x00;

  for (i=1; i<256; i++)
    {
      p_colors[i].r=156; /* Background greenish */
      p_colors[i].g=167;
      p_colors[i].b=143;
    }

  /* Initialize SDL */
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
      exit(1);
    }
  atexit(SDL_Quit);

  /* Set video mode (8 bit color) */
  if ( (p_screen=SDL_SetVideoMode(PHYS_WIDTH*i_size, PHYS_HEIGHT*i_size, 8, i_options)) == NULL )
    {
      fprintf(stderr, "Couldn't set %dx%d video mode: %s\n", PHYS_WIDTH, PHYS_HEIGHT, SDL_GetError());
      exit(2);
    }
  /* Set the palette */
  SDL_SetPalette(p_screen, SDL_LOGPAL|SDL_PHYSPAL, p_colors, 0, 256);

  /* Fill the screen with greenish color */
  r.x = 0;
  r.y = 0;
  r.w = PHYS_WIDTH*i_size;
  r.h = PHYS_HEIGHT*i_size;
  SDL_FillRect(p_screen, &r, SDL_MapRGB(p_screen->format, BACKGROUND_COLOR, BACKGROUND_COLOR, BACKGROUND_COLOR));

  draw_rex_bitmap(p_bitmap, 0, 0, i_size);

  SDL_Flip(p_screen);

  while (!i_exit)
    {
      while( SDL_PollEvent(&event) )
	{
	  handle_event(&event);
	}
    }

  return 0;
}
