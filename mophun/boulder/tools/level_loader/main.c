/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Create a level-set file
 *
 * $Id: main.c 8611 2006-06-22 06:31:33Z ska $
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

#define STREQ(x,y) strcmp(x, y) == 0

static char tile_to_chr(tile_t tile)
{
  if (tile == TILE_EMPTY)
    return ' ';
  else if (tile == TILE_DIRT)
    return '.';
  else if (tile == TILE_BOULDER)
    return 'o';
  else if (tile == TILE_GHOST)
    return 'G';
  else if (tile == TILE_STONE_WALL)
    return 'W';
  else if (tile == TILE_WEAK_STONE_WALL)
    return 'w';
  else if (tile == TILE_TELEPORTER)
    return 't';
  else if (tile == TILE_LEFT_TRANSPORT)
    return 'l';
  else if (tile == TILE_RIGHT_TRANSPORT)
    return 'r';
  else if (tile == TILE_EXIT)
    return 'E';
  else if (tile == TILE_MAGIC_WALL)
    return '-';
  else if (tile == TILE_BLOCK)
    return 'b';
  else if (tile == TILE_BOMBS)
    return 'B';
  else if (tile == TILE_USER0)
    return '0';
  else if (tile == TILE_DIAMOND)
    return 'd';
  else if (tile == TILE_CONVEYOR_LEFT)
    return 'F';
  else if (tile == TILE_CONVEYOR)
    return 'C';
  else if (tile == TILE_CONVEYOR_RIGHT)
    return    'H';
  else if (tile == TILE_IRON_KEY)
    return    'j';
  else if (tile == TILE_GOLD_KEY)
    return    'k';
  else if (tile == TILE_RED_KEY)
    return    'K';
  else if (tile == TILE_WOODEN_DOOR)
    return    '!';
  else if (tile == TILE_USER0)
    return    '0';
  else if (tile == TILE_USER1)
    return    '1';
  else
    return '?';
  /* FIXME) Add the rest! */

  return 0;
}

file_section_t *get_level(FILE *p_infile, file_section_table_t *p_sectab, int level_index)
{
  file_section_t *p_out = file_section_get(p_infile, p_sectab, level_index);

  return p_out;
}

file_section_t *get_level_set(FILE *p_infile, file_section_table_t *p_sectab)
{
  int i;

  for (i=0; i<p_sectab->n_sections; i++)
    {
      if (p_sectab->entries[i].type == HDR_TYPE_LEVEL_SET)
	{
	  file_section_t *p_out = malloc(p_sectab->entries[i].size);

	  fseek(p_infile, p_sectab->entries[i].file_offset, SEEK_SET);
	  fread(p_out, p_sectab->entries[i].size, 1, p_infile);

	  return p_out;
	}
    }

  return NULL;
}

void print_level_data(file_section_t *p_level)
{
  int x,y;

  printf("level data:\n"
	 "  w,h:   (%d, %d)\n   ",
	 p_level->u.level_data.w,
	 p_level->u.level_data.h
	 );
  for (y=0; y<p_level->u.level_data.h; y++)
    {
      for (x=0; x<p_level->u.level_data.w; x++)
	printf("%c", tile_to_chr(p_level->u.level_data.data[y*p_level->u.level_data.w+x]));
      printf("\n   ");
    }

}

void print_header(file_header_t *p_hdr)
{
  int i;

  printf("header:\n"
	 "  magic:         0x%08x\n"
	 "  sectab_off:    %d\n"
	 "  sectab_size:   %d\n"
	 "  size:          %d\n"
	 "  API:           0x%02x\n"
	 "  n_level_sets:  %d\n",
	 p_hdr->magic, p_hdr->section_table_offs, p_hdr->section_table_size, p_hdr->size,
	 p_hdr->API_version, p_hdr->n_level_sets
	 );

  for (i=0; i<p_hdr->n_level_sets; i++)
    printf("    idx:         %d\n", p_hdr->level_set_idx[i]);
}

void print_level(file_section_t *p_level)
{
  printf("level:\n"
	 "  time:         %d\n"
	 "  player_pt:   (%d, %d)\n"
	 "  diamonds:     %d\n"
	 "  n_spec:       %d\n"
	 "  spec_init:    %d\n"
	 "  data_sect:    %d\n",
	 p_level->u.level.time,
	 p_level->u.level.player_pt.x, p_level->u.level.player_pt.y,
	 p_level->u.level.diamonds_required,
	 p_level->u.level.n_specials,
	 p_level->u.level.special_init_idx,
	 p_level->u.level.level_data_sect
	 );
}

void print_level_set(file_section_t *p_level_set)
{
  int i;

  printf("level set:\n"
	 "  n_levels: %d\n"
	 "  name:     %s\n",
	 p_level_set->u.level_set.n_levels, p_level_set->u.level_set.name
	 );

  for (i=0; i<p_level_set->u.level_set.n_levels; i++)
    printf("  %d\n", p_level_set->u.level_set.level_idx[i]);
}

file_section_table_t *read_section_table(FILE *p_infile, file_header_t *p_hdr)
{
  file_section_table_t *p_out;
  int i;

  fseek(p_infile, p_hdr->section_table_offs, SEEK_SET);

  p_out = malloc(p_hdr->section_table_size);
  if (fread(p_out, p_hdr->section_table_size, 1, p_infile) < 0)
    error_msg("Could not read section header\n");

  printf("Section table:\n"
	 " n_entries:      %d\n",
	 p_out->n_sections
	 );
  for (i=0; i<p_out->n_sections; i++)
    {
      printf("  offset:          %4d\n"
	     "  type:            %4d\n"
	     "  size:            %4d\n",
	     p_out->entries[i].file_offset,
	     p_out->entries[i].type,
	     p_out->entries[i].size
	     );
    }

  return p_out;
}

int main(int argc, char *argv[])
{
  file_section_table_t *p_section_table;
  file_section_t *p_level_set;
  file_header_t *p_hdr;
  file_handle_t infile;
  int i;

  if (argc < 2)
    return 1;

  if ( !(infile = fopen(argv[1], "r")) )
    error_msg("Could not open %s\n", argv[1]);

  p_hdr = file_read_header(infile);
  print_header(p_hdr);

  p_section_table = read_section_table(infile, p_hdr);
  printf("\n"
	 "Level set\n"
	 "------------\n");
  p_level_set = file_section_get(infile, p_section_table, p_hdr->level_set_idx[0]);
  print_level_set(p_level_set);

  for (i=0; i<p_level_set->u.level_set.n_levels; i++)
    {
      file_section_t *p_level = file_section_get(infile, p_section_table,
						 p_level_set->u.level_set.level_idx[i]);
      file_section_t *p_level_data;

      assert(p_level);
      p_level_data = file_section_get(infile, p_section_table,
				      p_level->u.level.level_data_sect);
      assert(p_level_data);
      print_level(p_level);
      print_level_data(p_level_data);
    }

  fclose(infile);
  return 0;
}
