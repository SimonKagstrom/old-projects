/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Create a level-set file
 *
 * $Id: main.c 472 2004-08-20 19:33:10Z ska $
 *
 ********************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

#define __USE_BSD
#include <string.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <file_handling.h>
#include <tiles.h>

#include "utils.h"

#define STREQ(x,y) (strcmp(x, y) == 0)

static int n_objects, map_width, map_height;



static tile_t chr_to_tile(const char chr)
{
  switch(chr)
    {
    case ' ':
      return TILE_EMPTY;
    case '.':
      return TILE_DIRT;
    case 'o':
      n_objects++;
      return TILE_BOULDER;
    case 'G':
      return TILE_GHOST;
      n_objects++;
    case 'W':
      return TILE_STONE_WALL;
    case 'w':
      return TILE_WEAK_STONE_WALL;
    case 't':
      return TILE_TELEPORTER;
    case 'l':
      return TILE_LEFT_TRANSPORT;
    case 'r':
      return TILE_RIGHT_TRANSPORT;
    case 'E':
      return TILE_EXIT;
    case '-':
      return TILE_MAGIC_WALL;
    case 'b':
      n_objects++;
      return TILE_BLOCK;
    case 'B':
      return TILE_BOMBS;
    case 'd':
      n_objects++;
      return TILE_DIAMOND;
    case 'F':
      return TILE_CONVEYOR_LEFT;
    case 'C':
      return TILE_CONVEYOR;
    case 'H':
      return TILE_CONVEYOR_RIGHT;
    case 'j':
      return TILE_IRON_KEY;
    case 'k':
      return TILE_GOLD_KEY;
    case 'K':
      return TILE_RED_KEY;
    case '!':
      return TILE_WOODEN_DOOR;
    case ':':
      return TILE_IRON_DOOR;
    case '|':
      return TILE_RED_DOOR;
    case '0':
      return TILE_USER0;
    case '1':
      return TILE_USER1;
      /* FIXME: Add the rest! */
    default:
      printf("ERROR: Unknown character: '%c'\n", chr);
      break;
    }

  return -1;
}

static tile_t *load_level_file_pass1(const char *filename, int *p_size)
{
  FILE *infile = fopen(filename, "r");
  char *p_line = NULL;
  tile_t *p_out;
  int data_found = 0;
  int line_len=0;
  int h = 0, w = 0;

  if (!infile)
    return NULL;

  while ( getline(&p_line, &line_len, infile) != -1 )
    {
      int n_strs;
      char **pp_strs;

      if (!line_is_acceptable(p_line))
	continue;
      pp_strs = strs_get(p_line, &n_strs);

      if (!pp_strs)
	continue;

      else if (STREQ(pp_strs[0], "data:"))
	{
	  h = 0;
	  data_found = 1;
	}
      else if (data_found)
	{
	  w = strlen(p_line)-1;

	  h++;
	  *p_size = w*h;

	  assert(w < 300 && h < 300);
	}

      strs_free(pp_strs, n_strs);
    }

  free(p_line);
  fclose(infile);

  map_width = w;
  map_height = h;

  p_out = malloc(sizeof(tile_t)*(*p_size));

  return p_out;
}

static int load_level_file_pass2(tile_t *p_tiles, const char *filename)
{
  FILE *infile = fopen(filename, "r");
  char *p_line = NULL;
  int data_found = 0;
  int line_len=0;
  int h = 0;

  if (!infile)
    return -1;

  while ( getline(&p_line, &line_len, infile) != -1 )
    {
      int n_strs;
      char **pp_strs;

      if (!line_is_acceptable(p_line))
	continue;
      pp_strs = strs_get(p_line, &n_strs);

      if (!pp_strs)
	continue;

      if (STREQ(pp_strs[0], "id:"))
	continue;
      if (STREQ(pp_strs[0], "diamonds:"))
	continue;
      if (STREQ(pp_strs[0], "time:"))
	continue;
      else if (STREQ(pp_strs[0], "data:"))
	{
	  h = 0;
	  data_found = 1;
	}
      else if (data_found)
	{
	  int w = strlen(p_line)-1;
	  int i;

	  for (i=0; i<w; i++)
	    {
	      tile_t out = chr_to_tile(p_line[i]);

	      p_tiles[h * w + i] = out;
	    }
	  h++;
	}

      strs_free(pp_strs, n_strs);
    }

  free(p_line);
  fclose(infile);

  return 0;
}


static tile_t *create_level_file(const char *in_filename, const char *out_filename)
{
  FILE *p_outfile;
  tile_t *p_out;
  int size;

  n_objects = 0;
  if (!(p_out = load_level_file_pass1(in_filename, &size)) )
    {
      printf("Pass one of the level loading failed!\n");
      return NULL;
    }
  if (load_level_file_pass2(p_out, in_filename) < 0)
    {
      printf("Pass two failed!\n");
      return NULL;
    }
  printf("Writing map, size %d:%d, %d objects\n", map_width, map_height, n_objects);

  if (n_objects > 80)
    printf("WARNING: Too many objects!\n");

  if ( !(p_outfile = fopen(out_filename, "w+")) )
    return NULL;
  fwrite(p_out, sizeof(tile_t), size, p_outfile);
  fclose(p_outfile);

  return p_out;
}


int main(int argc, char *argv[])
{
  char buf[255];

  if (argc < 2)
    error_msg("Usage: creator INFILE [-o NAME]\n");

  sprintf(buf, "%s.dat", argv[1]);
  create_level_file(argv[1], buf);

  return 0;
}
