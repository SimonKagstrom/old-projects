/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Create maps from tiles and pictures
 *
 * $Id: main.c 1104 2005-01-16 05:59:19Z ska $
 *
 ********************************************************************/
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdlib.h> /* Exit */
#include <stdio.h>  /* printf */
#include <string.h> /* strcmp etc. */
#include <time.h>   /* time */

#define CMP(str_a, str_b) (strcmp(str_a, str_b) == 0)
#define fail(msg) do {fprintf(stderr, msg); exit(1);} while(0)

/* From the SDL documentation */
static Uint32 getpixel(SDL_Surface *p_image, int x, int y)
{
  Uint8 *p;
  Uint32 out;
  int bpp;

  SDL_LockSurface(p_image);
  bpp = p_image->format->BytesPerPixel;
  p = (Uint8 *)p_image->pixels + y * p_image->pitch + x * bpp;

  switch(bpp)
    {
    case 1:
      out = *p;
      break;
    case 2: /* This will cause some problems ... */
      out = *(Uint16 *)p;
      break;
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
	out = p[0] << 16 | p[1] << 8 | p[2];
      else
	out = p[0] | p[1] << 8 | p[2] << 16;
      break;
    case 4:
      out = *(Uint32 *)p;
      break;
    default:
      out = 0;       /* shouldn't happen, but avoids warnings */
      break;
    }
  SDL_UnlockSurface(p_image);
  return out;
}




static SDL_Surface *alloc_surface(int w, int h)
{
  Uint32 rmask, gmask, bmask, amask;
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
     on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif

  return SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, rmask,gmask,bmask,amask);
}

#define abs(x) ( ((x) < 0) ? -(x) : (x) )

/* Is p_new present in p_orig */
static int is_unique(SDL_Surface *p_orig, SDL_Surface *p_new)
{
  int n_entries = p_orig->h/8;
  int i;

  for (i=0; i<n_entries; i++)
    {
      int x,y;
      int found = 0;

      /* Compare the entire image */
      for (y=0; y<8; y++)
	{
	  for(x=0; x<8; x++)
	    {
	      Uint32 orig_pixel, new_pixel;
	      Uint8 or,og,ob, nr,ng,nb;

	      orig_pixel = getpixel(p_orig, x, i*8 + y);
	      new_pixel = getpixel(p_new, x, y);
	      SDL_GetRGB(orig_pixel, p_orig->format, &or, &og, &ob);
	      SDL_GetRGB(new_pixel, p_new->format, &nr, &ng, &nb);
	      /* This is not exact because of different pixel formats */
	      if (abs(or - nr) > 2 ||
		  abs(og - ng) > 2 ||
		  abs(ob - nb) > 2)
		{
		  found = 1;
		  goto next_it;
		}
	    }
	}
    next_it:
      if (found == 0)
	return i;
    }

  /* Found non-unique! */
  return -1;
}


static int handle_pass(SDL_Surface *p_tiles, SDL_Surface *p_src, char *p_name,
		       void (*fn)(int x, int y, int n))
{
  int w, h;

  for (h=0; h<p_src->h; h+=8)
    {
      for (w=0; w<p_src->w; w+=8)
	{
	  SDL_Surface *p_tmp = alloc_surface(8,8);
	  SDL_Rect src, dst;
	  int n;

	  src.x = w; dst.x = 0;
	  src.y = h; dst.y = 0;
	  src.w = 8; dst.w = 8;
	  src.h = 8; dst.h = 8;

	  SDL_BlitSurface(p_src, &src, p_tmp, &dst);
	  if ( (n = is_unique(p_tiles, p_tmp)) < 0)
	    {
	      SDL_FreeSurface(p_tmp);
	      return -1;
	    }
	  else
	    fn(w, h, n+1);
	  SDL_FreeSurface(p_tmp);
	}
    }

  return 0;
}

static void usage(void)
{
  fprintf(stderr,
	  "Usage: eventmap TILEFILE FILE1 [FILE2...]\n\n"
	  "Create the events from FILE1, FILE2, [...]. The infiles can have arbitrary widths\n"
	  "and heights (although it should be divisible by 8). The infiles can use any\n"
	  "format supported by SDL_image.\n"
	  "\n"
	  "-n basename      Set the base name of the map (default: __mapNR)\n"
	  );
  exit(1);
}

static void bruce_fn(int x, int y, int n)
{
  if (n == 2)
    printf("  {%d,%d},\n", x,y-6);
}

static void ninja_fn(int x, int y, int n)
{
  if (n == 3)
    printf("  {%d,%d},\n", x,y-4);
}

static void yamo_fn(int x, int y, int n)
{
  if (n == 3)
    printf("  {%d,%d},\n", x,y-4);
}

static int lamp_id = 0;

static void lamp_fn(int x, int y, int n)
{
  if (n == 4)
    printf("  {{%d,%d}, %d},\n", x,y, lamp_id++);
}

int main(int argc, char *argv[])
{
  SDL_Surface *p_tileimage;
  char *p_basename;
  int i, n_imgs = 0;
  int first = 1;

  srand(time(0));

  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
      exit(1);
    }
  atexit(SDL_Quit);

  if (argc < 4)
    usage();

  p_basename = argv[first];

  if ( !(p_tileimage = IMG_Load(argv[first+1])) )
    fail("IMG_Load tilefile\n");
  for (i=first+2; i<argc; i++)
    {
      SDL_Surface *p_in;
      char buf[255];

      if ( !(p_in = IMG_Load(argv[i])) )
	fail("IMG_Load\n");

      printf("point_t %s_bruce_spawn[] ={\n", p_basename);
      if (handle_pass(p_tileimage, p_in, buf, bruce_fn) < 0)
	fail("handle_pass\n");
      printf("  {-1,-1}\n};\n\n");

      printf("point_t %s_ninja_spawn[] ={\n",  p_basename);
      if (handle_pass(p_tileimage, p_in, buf, ninja_fn) < 0)
	fail("handle_pass\n");
      printf("  {-1,-1}\n};\n\n");

      printf("point_t %s_yamo_spawn[] ={\n",  p_basename);
      if (handle_pass(p_tileimage, p_in, buf, yamo_fn) < 0)
	fail("handle_pass\n");
      printf("  {-1,-1}\n};\n\n");

      printf("lamp_store_t %s_lamps[] ={\n",  p_basename);
      if (handle_pass(p_tileimage, p_in, buf, lamp_fn) < 0)
	fail("handle_pass\n");
      printf("  {{-1,-1}, 31}\n};\n\n");


      n_imgs++;
    }
  if (n_imgs <= 0)
    usage();

  return 0;
}
