/*********************************************************************
 *                
 * Copyright (C) 2001,  Simon Kagstrom
 *                
 * Filename:      conv2rex.c
 * Description:   This file contains the implementation of the
 *                libSDL-based frontend.
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
 * $Log: conv2rex.c,v $
 * Revision 1.3  2001/12/18 14:21:57  simon
 * Added possibility to create icon file
 *
 * Revision 1.2  2001/12/17 14:47:05  simon
 * * Added support for arbitrary depths
 * * Cleanup
 * * Write to binary file now possible
 *
 * Revision 1.1.1.1  2001/12/16 16:43:24  simon
 * Imported conv2rex
 *                
 ********************************************************************/
#include <unistd.h>        /* getopt etc */
#include <stdio.h>         /* printf etc */
#include <errno.h>         /* perror */
#include <ctype.h>         /* isprint */
#include <stdlib.h>        /* malloc etc */
#include <SDL/SDL.h>       /* The SDL library */
#include <SDL/SDL_image.h> /* The SDL_image library */

#define TRUE 1
#define FALSE 0

#define TYPE_C            1
#define TYPE_STRIPPED_BIN 2
#define TYPE_ICON         3
#define TYPE_BIN          0

/* From the SDL documentation */
Uint32 get_pixel(SDL_Surface *p_image, int x, int y)
{
  int i_bpp = p_image->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *)p_image->pixels + y * p_image->pitch + x * i_bpp;

  switch(i_bpp)
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

/* Prints out one byte (in a REX-image that is) */
void print_char(unsigned char c)
{
  int i;

  for (i=7; i>=0; i--)
    {
      if (c & (1<<i))
	printf("#");
      else
	printf(".");
    }
}

/* Display a image (SDL) */
void print_image(SDL_Surface *p_image)
{
  int x;
  int y;

  SDL_LockSurface(p_image);
  for(y=0; y<p_image->h; y++)
    {
      for (x=0; x<p_image->w; x++)
	{
	  Uint8 r, g, b;

	  SDL_GetRGB(get_pixel(p_image, x, y), p_image->format, &r, &g, &b);

	  if (!(r == 0xff && g == 0xff && b == 0xff))
	    printf("#");
	  else
	    printf(".");
	}
      printf("\n");
    }
  SDL_UnlockSurface(p_image);
}

/* Finds the rightmost pixel in an image (determine real width of image) */
int find_rightmost_pixel(SDL_Surface *p_image)
{
  Sint32 x;
  Sint32 y;
  Sint32 i_rightmost = 0;

  SDL_LockSurface(p_image);
  for(y=0; y<p_image->h; y++)
    {
      for (x=p_image->w-1; x>= 0; x--)
	{
	  Uint8 r, g, b;

	  SDL_GetRGB(get_pixel(p_image, x, y), p_image->format, &r, &g, &b);

	  if (!(r == 0xff && g == 0xff && b == 0xff) && x > i_rightmost)
	    {
	      i_rightmost = x;
	      break;
	    }
	}
    }
  SDL_UnlockSurface(p_image);

  return i_rightmost;
}

/* Write p_bitmap as a binary file */
int write_bin_file(Uint8 *p_bitmap, char *p_outfile, int i_type)
{
  Sint32 i_w = p_bitmap[0];
  Sint32 i_h = p_bitmap[1];
  FILE *p_out;
  int i;

  if ( !(p_out = fopen(p_outfile, "w")) )
    {
      fprintf(stderr, "fopen: Could not open file");
      return -1;
    }
  
  for (i=i_type; i<i_h*(i_w/8+(i_w%8?1:0))+2; i++)
    fwrite(p_bitmap+i, sizeof(Uint8), 1, p_out);
  fclose(p_out);

  return 0;
}

/* Write p_bitmap as a C source file */
int write_c_file(Uint8 *p_bitmap, char *p_outfile, int b_use_uint8, char *p_struct_name)
{
  Sint32 i_w = p_bitmap[0];
  Sint32 i_h = p_bitmap[1];
  Uint8 x, y;
  FILE *p_out;

  if ( !(p_out = fopen(p_outfile, "w")) )
    {
      fprintf(stderr,"fopen");
      return -1;
    }

  fprintf(p_out, 
	  "const %s %s[] =\n"
	  "{\n"
	  "  %d, %d,\n", b_use_uint8?"uint8_t":"char", p_struct_name, i_w, i_h);

  /* For each row */
  for (y=0; y<i_h; y++)
    {
      fprintf(p_out, "  ");

      /* For each block of eight columns */
      for (x=0; x<(i_w/8+(i_w%8?1:0)); x++)
	{
	  Uint8 i_curr = p_bitmap[y*(i_w/8+(i_w%8?1:0))+x+2];
	  fprintf(p_out, "0x%s%x, ", (i_curr < 0x10?"0":""), i_curr);
	}
      if (y != i_h)
	fprintf(p_out, "\n");
    }
  fprintf(p_out, "\n};\n");

  fclose(p_out);

  return 0;
}

/* Write p_bitmap as a resource (icon) file */
int write_icon_file(char *p_infile, char *p_outfile)
{
  SDL_Surface *p_image = IMG_Load(p_infile);
  Sint32 x, y;
  FILE *p_out;

  if (!p_image)
    {
      fprintf(stderr, "Could not open infile\n");
      return -1;
    }

  if ( !(p_out = fopen(p_outfile, "w")) )
    {
      fprintf(stderr, "fopen: Could not open outfile");
      return -1;
    }

  /* Write header */
  fprintf(p_out,
	  "APPNAME Addin\n"
	  "COMMENT Comment\n"
	  "ICON\n");

  /* For every pixel in the image */
  for (y=0; y<p_image->h; y++)
    {
      for (x=0; x<p_image->w; x++)
	{
	  Uint8 r,g,b;

	  SDL_GetRGB(get_pixel(p_image, x, y), p_image->format, &r, &g, &b);

	  if (!(r == 0xff && g == 0xff && b == 0xff))
	    fprintf(p_out, "X");
	  else
	    fprintf(p_out, ".");		
	}
      fprintf(p_out, "\n");
    }

  fclose(p_out);

  return 0;
}


/* Convert p_infile to p_outfile (REX format), use uint8_t if
 * b_use_uint8 is set and name the resulting C structure
 * p_struct_name.
 */
int convert_image(char *p_infile, Uint8 **p_bitmap_in)
{
  SDL_Surface *p_image = IMG_Load(p_infile);
  Sint32 i_bitmap_count = 2;
  Uint8 *p_bitmap;
  Sint32 x, y;

  /* File not found */
  if (!p_image)
    return -1;

  SDL_LockSurface(p_image);

  if ( !(p_bitmap = calloc(sizeof(Uint8), (p_image->w*p_image->h+2))) )
    {
      perror("calloc");
      return -1;
    }

  p_bitmap[0] = p_image->w; /* Maybe find_rightmost_pixel(p_image)+1; */
  p_bitmap[1] = p_image->h;

  /* For each row */
  for (y=0; y<p_image->h; y++)
    {
      /* For each block of eight columns */
      for (x=0; x<p_image->w; x+=8)
	{
	  Uint8 i_curr = 0;
	  int i;

	  /* For every pixel (all bits in a char) */
	  for(i=0; i<8; i++)
	    {	      
	      Uint8 r,g,b;

	      /* I think this works, but SDL seems to corrupt some
	       * images (?).  It seems SDL is fine with images that
	       * are a multiple of 8 or 4 wide.
	       */
	      if ((x+7)-i >= p_image->w)
		continue;
	      SDL_GetRGB(get_pixel(p_image, (x+7)-i, y), p_image->format, &r, &g, &b);

	      if (!(r == 0xff && g == 0xff && b == 0xff))
		i_curr |= (1<<i); /* Add it to the current byte */
	    }
	  p_bitmap[i_bitmap_count++] = i_curr;
	}
    }
  SDL_UnlockSurface(p_image);
  *p_bitmap_in = p_bitmap;

  return 0;
}

/* Print help message */
void print_usage()
{
  printf("conv2rex converts images to REX 6000 bitmaps (to include in\n"
	 "c-files), REX binary files and icon (resurce files). White in\n"
	 "the image is assumed to be clear areas, black filled. The images\n"
	 "can have any pixeldepth and be of any format supported by \n"
	 "SDL_Image.\n\n"
	 "Usage:\n"
	 "conv2rex [options] infile outfile\n\n"
	 "Options are:\n"
	 "-h                This text\n"
	 "-c                Create C-source (default) file\n"
	 "-i                Create icon (resource) file\n"
	 "-b                Create binary file\n"
	 "-s                Create stripped binary file (no width/height. Used\n"
         "                  for the icons etc)\n"
	 "-u                Use uint8_t instead of char (only if not binary file)\n"
	 "-n NAME           Specify the name of the bitmap (normally p_image)\n"
	 );
}

int main(int argc, char *argv[])
{
  char *p_name = "p_image";
  int b_use_uint8 = FALSE;
  int i_type = TYPE_C;
  Uint8 *p_bitmap;
  char c;

  while ((c=getopt(argc, argv, "cisbhun:")) != -1)
    {
      switch(c)
	{
	case 'c':
	  i_type = TYPE_C;
	  break;
	case 'i':
	  i_type = TYPE_ICON;
	  break;
	case 'b':
	  i_type = TYPE_BIN;
	  break;
	case 's':
	  i_type = TYPE_STRIPPED_BIN;
	  break;
	case 'h':
	  print_usage();
	  return 0;
	case 'u':
	  b_use_uint8 = TRUE;
	  break;
	case 'n':
	  p_name = optarg;
	  break;
	case '?':	  
	  break;
	default:
	  abort();
	}
    }

  if (argc < 3)
    {
      print_usage();
      return 0;
    }

  /* Initialize SDL */
  if ( SDL_Init(0) < 0 )
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      exit(1);
    }
  atexit(SDL_Quit);

  if (convert_image(argv[argc-2], &p_bitmap) < 0)
    {
      fprintf(stderr, "Error in conversion\n");
      return -1;
    }

  switch(i_type)
    {
    case TYPE_C:
      if (write_c_file(p_bitmap, argv[argc-1], b_use_uint8, p_name) < 0)
	{
	  fprintf(stderr, "Error when writing C-file\n");
	  return -1;
	}
      break;
    case TYPE_ICON:
      if (write_icon_file(argv[argc-2], argv[argc-1]) < 0)
	{
	  fprintf(stderr, "Error when writing C-file\n");
	  return -1;
	}
      break;
    case TYPE_BIN:
    case TYPE_STRIPPED_BIN:
      write_bin_file(p_bitmap, argv[argc-1], i_type);
      break;
    }


  return 0;
}
