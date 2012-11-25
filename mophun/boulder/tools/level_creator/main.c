/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Create a level-set file
 *
 * $Id: main.c 485 2004-08-21 09:52:43Z ska $
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




typedef struct
{
  const char *dir;

  /* Assume only one level set */
  int n_levels;
  int n_level_datas;
  int n_specials;

  file_level_t         **pp_levels;
  file_special_t       **pp_specials;
  file_level_data_t    **pp_level_datas;
} file_t;


typedef struct
{  file_header_t   *p_file_hdr;
  file_section_table_t  *p_section_table;
  file_section_t  **pp_sections;
  int             n_sections;
  int             n_level_sets;
} outfile_t;

static int add_section(outfile_t *p_outfile, file_section_t *p_section);

static tile_t chr_to_tile(const char chr)
{
  switch(chr)
    {
    case ' ':
      return TILE_EMPTY;
    case '.':
      return TILE_DIRT;
    case 'o':
      return TILE_BOULDER;
    case 'G':
      return TILE_GHOST;
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
      return TILE_BLOCK;
    case 'B':
      return TILE_BOMBS;
    case 'd':
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

static int load_level_file_pass1(file_t *p_file, const char *filename, int n)
{
  FILE *infile = fopen(filename, "r");
  char *p_line = NULL;
  int data_found = 0;
  int line_len=0;
  int h = 0;

  if (!infile)
    return -1;

  p_file->pp_levels[n] = malloc(sizeof(file_level_t));
  assert(p_file->pp_levels[n]);
  memset(p_file->pp_levels[n], 0, sizeof(file_level_t));

  while ( getline(&p_line, &line_len, infile) != -1 )
    {
      int n_strs;
      char **pp_strs;

      if (!line_is_acceptable(p_line))
	continue;
      pp_strs = strs_get(p_line, &n_strs);

      if (!pp_strs)
	continue;

      if (STREQ(pp_strs[0], "diamonds:"))
	p_file->pp_levels[n]->diamonds_required = atoi(pp_strs[1]);
      if (STREQ(pp_strs[0], "time:"))
	p_file->pp_levels[n]->time = atoi(pp_strs[1]);
      if (STREQ(pp_strs[0], "point:"))
	{
	  p_file->pp_levels[n]->player_pt.x = atoi(pp_strs[1])*8;
	  p_file->pp_levels[n]->player_pt.y = atoi(pp_strs[2])*8;
	}
      else if (STREQ(pp_strs[0], "data:"))
	{
	  h = 0;
	  data_found = 1;
	}
      else if (data_found)
	{
	  int w = strlen(p_line)-1;

	  h++;

	  assert(w < 300 && h < 300);

	  p_file->pp_level_datas[n] = realloc(p_file->pp_level_datas[n],
					      sizeof(file_level_data_t) + sizeof(tile_t)*w*h);
	  p_file->pp_level_datas[n]->w = w;
	  p_file->pp_level_datas[n]->h = h;
	}

      strs_free(pp_strs, n_strs);
    }

  free(p_line);
  fclose(infile);

  return 0;
}

static int load_level_file_pass2(file_t *p_file, const char *filename, int n)
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
	      tile_t *p_tile_data = (tile_t *)(p_file->pp_level_datas[n]+1);
	      tile_t out = chr_to_tile(p_line[i]);

	      p_tile_data[h * p_file->pp_level_datas[n]->w + i] = out;
	    }
	  h++;
	}

      strs_free(pp_strs, n_strs);
    }

  free(p_line);
  fclose(infile);

  return 0;
}


static int load_level_file(file_t *p_file, const char *filename, int n)
{
  load_level_file_pass1(p_file, filename, n);
  load_level_file_pass2(p_file, filename, n);

  return 0;
}

#if 0
static int load_special_file_pass1(file_t *p_file, const char *filename, int n)
{
  FILE *infile = fopen(filename, "r");
  char *p_line = NULL;
  int data_found = 0;
  int line_len=0;
  int h = 0;
  int type = -1;

  if (!infile)
    return -1;

  p_file->pp_specials[n] = malloc(sizeof(file_special_t));
  assert(p_file->pp_specials[n]);
  memset(p_file->pp_specials[n], 0, sizeof(file_special_t));

  while ( getline(&p_line, &line_len, infile) != -1 )
    {
      int n_strs;
      char **pp_strs;

      if (!line_is_acceptable(p_line))
	continue;
      pp_strs = strs_get(p_line, &n_strs);

      if (!pp_strs)
	continue;

      if (STREQ(pp_strs[0], "type:"))
	{
	  if (STREQ(pp_strs[1], "explosion"))
	    {
	      /* Realloc these */
	      p_file->pp_levels[n]->type = SPECIAL_EXPLOSION;
	    }
	  else if (STREQ(pp_strs[1], "growing_wall"))
	    p_file->pp_levels[n]->type = SPECIAL_GROWING_WALL;
	  else if (STREQ(pp_strs[1], "elem_generator"))
	    p_file->pp_levels[n]->type = SPECIAL_ELEM_GENERATOR;
	  else
	    printf("ERROR: Unknown special type %s\n", pp_strs[1]);
	}

      strs_free(pp_strs, n_strs);
    }

  free(p_line);
  fclose(infile);

  return 0;
}


static int load_special_file_pass2(file_t *p_file, const char *filename, int n)
{
  FILE *infile = fopen(filename, "r");
  char *p_line = NULL;
  int data_found = 0;
  int line_len=0;
  int h = 0;
  int type = -1;

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

      if (STREQ(pp_strs[0], "point:"))

      strs_free(pp_strs, n_strs);
    }

  free(p_line);
  fclose(infile);

  return 0;
}
#endif


static int load_level_set_dir(file_t *p_file, const char *dirname)
{
  char *level_name;
  int i;

  memset(p_file, 0, sizeof(file_t));

  /* FIXME! Lookup the directories */
  for (i=0; i<32; i++)
    {
      if ( !(level_name = create_file_str(dirname, "level", i)) )
	return 0;
      p_file->n_levels++;
      p_file->pp_levels = realloc(p_file->pp_levels,
				  sizeof(file_level_t*) * p_file->n_levels);
      p_file->n_level_datas++;
      p_file->pp_level_datas = realloc(p_file->pp_level_datas,
				       sizeof(file_level_data_t*) * p_file->n_levels);

      load_level_file(p_file, level_name, i);
    }
  return 0;
}


static file_section_t *new_section(header_type_t type, uint16_t size)
{
  file_section_t *p_out = malloc(size + FILE_SECTION_MINSIZE);

  assert(p_out);

  memset(p_out, 0, size + FILE_SECTION_MINSIZE);
  p_out->magic = HDR_MAGIC;
  p_out->size = FILE_SECTION_MINSIZE+size;
  p_out->type = type;

  return p_out;
}

static file_section_t *create_section_level(file_level_t *p_level)
{
  file_section_t *p_out = new_section(HDR_TYPE_LEVEL, sizeof(file_level_t) +
				      p_level->n_specials * sizeof(uint16_t));
  int i;
  void *p;

  p_out->u.level = *p_level;
  p = (void*)(p_level+1);

  for (i=0; i<p_level->n_specials; i++)
    {
      uint16_t *p_cur = (uint16_t *)&((uint8_t*)p_out + sizeof(file_level_t) + FILE_SECTION_MINSIZE)[i];

      *p_cur = *p_cur;
      p = (void*)( (uint16_t*)p + 1);
    }

  return p_out;
}

static file_section_t *create_section_special(file_special_t *p_special)
{
  file_section_t *p_out = new_section(HDR_TYPE_SPECIAL, sizeof(file_special_t) +
				      p_special->private_data_size);

  memcpy(p_out, p_special, sizeof(file_special_t) + p_special->private_data_size);

  return p_out;
}

static file_section_t *create_section_level_data(file_level_data_t *p_level)
{
  file_section_t *p_out = new_section(HDR_TYPE_LEVEL_DATA, sizeof(file_level_data_t) +
				      p_level->w*p_level->h*sizeof(tile_t));

  p_out->u.level_data = *p_level;
  memcpy((uint8_t*)p_out + FILE_SECTION_MINSIZE + sizeof(file_level_data_t),
	 p_level+1, p_level->w*p_level->h * sizeof(tile_t));

  return p_out;
}

static int add_section(outfile_t *p_outfile, file_section_t *p_section)
{
  int n;
  int offset;

  if (!p_outfile->p_section_table)
    {
      p_outfile->p_section_table = malloc(sizeof(file_section_table_t));
      memset(p_outfile->p_section_table, 0, sizeof(file_section_table_t));
      n = p_outfile->p_section_table->n_sections++;
      offset = 0;
    }
  else
    {
      n = p_outfile->p_section_table->n_sections++;
      offset = p_outfile->p_section_table->entries[n-1].file_offset +
	p_outfile->p_section_table->entries[n-1].size;
    }

  p_outfile->p_section_table = realloc(p_outfile->p_section_table,
				       sizeof(file_section_table_t) +
				       sizeof(section_table_entry_t)*(n+1));
  assert(p_outfile->p_section_table);

  p_outfile->p_section_table->entries[n].type = p_section->type;
  p_outfile->p_section_table->entries[n].size = p_section->size;
  p_outfile->p_section_table->entries[n].file_offset = offset;

  p_outfile->pp_sections = realloc(p_outfile->pp_sections,
				   sizeof(file_section_t*) * (n+1));
  assert(p_outfile->pp_sections);
  p_outfile->pp_sections[n] = p_section;

  if (p_section->type == HDR_TYPE_LEVEL_SET)
    p_outfile->n_level_sets++;

  return n;
}

static void write_outfile(outfile_t *p_outfile, char *filename)
{
  FILE *p_file = fopen(filename, "w+");
  int off_add;
  int sec = 0;
  int i;

  if (!p_file)
    error_msg("Could not open %s!\n", filename);

  p_outfile->p_file_hdr = malloc(sizeof(file_header_t) + sizeof(uint16_t)*p_outfile->n_level_sets);
  memset(p_outfile->p_file_hdr, 0, sizeof(file_header_t) + sizeof(uint16_t)*p_outfile->n_level_sets);
  assert(p_outfile->p_file_hdr);

  p_outfile->p_file_hdr->magic = FILE_MAGIC;
  p_outfile->p_file_hdr->size = sizeof(file_header_t) + sizeof(uint16_t)*p_outfile->n_level_sets;
  p_outfile->p_file_hdr->section_table_offs = p_outfile->p_file_hdr->size;
  p_outfile->p_file_hdr->section_table_size = sizeof(file_section_table_t) +
    p_outfile->p_section_table->n_sections * sizeof(section_table_entry_t);
  p_outfile->p_file_hdr->API_version = API_VERSION;
  p_outfile->p_file_hdr->n_level_sets = p_outfile->n_level_sets;

  off_add = p_outfile->p_file_hdr->size + sizeof(file_section_table_t) +
    p_outfile->p_section_table->n_sections * sizeof(section_table_entry_t);

  /* Fixup the offsets */
  for (i=0; i<p_outfile->p_section_table->n_sections; i++)
    p_outfile->p_section_table->entries[i].file_offset += off_add;

  for (i = 0; i < p_outfile->p_section_table->n_sections; i++)
    {
      if (p_outfile->pp_sections[i]->type == HDR_TYPE_LEVEL_SET)
	p_outfile->p_file_hdr->level_set_idx[sec++] = i;
    }

  fwrite(p_outfile->p_file_hdr, p_outfile->p_file_hdr->size, 1, p_file);
  fwrite(p_outfile->p_section_table,
	 sizeof(file_section_table_t) + p_outfile->p_section_table->n_sections * sizeof(section_table_entry_t),
	 1, p_file);
  for (i=0; i<p_outfile->p_section_table->n_sections; i++)
    fwrite(p_outfile->pp_sections[i], p_outfile->pp_sections[i]->size, 1, p_file);

  fclose(p_file);
}



static void outfile_add_level_set(outfile_t *p_outfile, file_t *p_file)
{
  file_section_t *p_out;
  int i;

  p_out = new_section(HDR_TYPE_LEVEL_SET, sizeof(file_level_set_t) +
		      p_file->n_levels * sizeof(uint16_t));
  p_out->u.level_set.n_levels = p_file->n_levels;
  strcpy(p_out->u.level_set.name, "TJOHO");

  for (i=0; i<p_file->n_levels; i++)
    {
      file_section_t *p_level = create_section_level(p_file->pp_levels[i]);
      file_section_t *p_level_data = create_section_level_data(p_file->pp_level_datas[i]);
      int j;

      assert(p_level && p_level_data);

      /* Add the level */
      p_out->u.level_set.level_idx[i] = add_section(p_outfile, p_level);
      p_level->u.level.level_data_sect = add_section(p_outfile, p_level_data);

      /* Add the level specials */
      for (j=0; j<p_level->u.level.n_specials; j++)
	p_level->u.level.specials_idx[j] = add_section(p_outfile,
						       create_section_special(p_file->pp_specials[j]));
    }

  add_section(p_outfile, p_out);
}



int main(int argc, char *argv[])
{
  outfile_t outfile;
  file_t file;

  if (argc < 2)
    error_msg("Usage: creator INDIRECTORY [-o NAME]\n");

  memset(&file, 0, sizeof(file_t));
  memset(&outfile, 0, sizeof(outfile_t));

  load_level_set_dir(&file, argv[1]);
  outfile_add_level_set(&outfile, &file);
  write_outfile(&outfile, "levels.dat");

  return 0;
}
