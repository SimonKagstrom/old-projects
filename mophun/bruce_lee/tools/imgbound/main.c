/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Tile creator
 *
 * $Id: main.c 649 2004-09-19 19:13:04Z ska $
 *
 ********************************************************************/
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h> /* Exit */
#include <stdio.h>  /* printf */
#include <string.h> /* strcmp etc. */

#define CMP(str_a, str_b) (strcmp(str_a, str_b) == 0)
#define fail(msg) do {fprintf(stderr, msg); exit(1);} while(0)

/* From the SDL documentation */
static Uint32 getpixel(SDL_Surface *p_image, int x, int y)
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

static void handle_image(const char *filename, SDL_Surface *p_img, int w, int h)
{
  int y1,x1;

  for (y1 = 0; y1 < p_img->h; y1 += h)
    {
      int min_x = 100000;
      int min_y = 100000;
      int max_x = 0;
      int max_y = 0;

      for (x1 = 0; x1 < p_img->w; x1 += w)
	{
	  int x,y;

	  for (y=0; y<h; y++)
	    {
	      for (x=0; x<w; x++)
		{
		  Uint8 r,g,b;

		  SDL_GetRGB(getpixel(p_img, x1+x, y1+y), p_img->format, &r, &g, &b);

		  if (r != 0 || g != 0 || b != 0)
		    {
		      if (min_x > x)
			min_x = x;
		      if (max_x < x)
			max_x = x;
		      if (min_y > y)
			min_y = y;
		      if (max_y < y)
			max_y = y;
		    }
		}
	    }
	  if (min_x == 100000)
	    min_x = 0;
	  if (min_y == 100000)
	    min_y = 0;
	  if (max_x == 0)
	    max_x = w-1;
	  if (max_y == 0)
	    max_y = h-1;
	  printf("{ {%d,%d},{%d,%d} },\t/* %dx%d */\n", min_x, min_y, max_x, max_y, max_x+1-min_x, max_y+1-min_y);
	}
    }
}


static void usage(void)
{
  fprintf(stderr,
	  "Usage: imgbound [-g X Y] FILE1 [FILE2 ...]\n\n"
	  "Printout the bounds of the images."
	  );
  exit(1);
}


int main(int argc, char *argv[])
{
  int i = 1;
  int g_x = 0;
  int g_y = 0;

  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
      exit(1);
    }
  atexit(SDL_Quit);

  if (argc < 2)
    usage();

  if (strcmp(argv[i], "-g") == 0)
    {
      if (argc < 5)
	usage();

      g_x = atoi(argv[++i]);
      g_y = atoi(argv[++i]);
      i++;
    }

  for (; i<argc; i++)
    {
      SDL_Surface *p_img;

      if ( !(p_img = IMG_Load(argv[i])) )
	{
	  fprintf(stderr, "Could not open %s\n", argv[i]);
	  exit(1);
	}

      if (g_x == 0)
	{
	  g_x = p_img->w;
	  g_y = p_img->h;
	}

      handle_image(argv[i], p_img, g_x, g_y);


      SDL_FreeSurface(p_img);
    }

  return 0;
}
