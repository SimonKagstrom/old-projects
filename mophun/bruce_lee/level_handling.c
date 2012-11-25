/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      level_handling.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Level handling
 *
 * $Id: level_handling.c 1104 2005-01-16 05:59:19Z ska $
 *
 ********************************************************************/
#include <game.h>
#include "res.h"

#define LAMP_IS_SET(x, id) ( (x) & (1<<id) )

static void lamp_init(game_t *p_game, lamp_t *p_lamp, lamp_store_t *p_ls)
{
  p_lamp->id = p_ls->id;
  p_lamp->sprite.pt = p_ls->pt;
}

static void place_lamps(game_t *p_game, level_t *p_level)
{
  int i;

  memset(p_game->lamps, 0, N_LAMPS*sizeof(lamp_t));

  for (i=0; i<p_level->n_lamps; i++)
    {
      lamp_store_t *p_cur = &p_level->p_lamps[i];

      if (LAMP_IS_SET(*p_level->p_lamp_mask, p_cur->id))
	lamp_init(p_game, &p_game->lamps[i], p_cur);
    }
}

void level_goto(game_t *p_game, level_t *p_level, point_t p)
{
  MAP_HEADER *p_bgmap = &p_game->bgmap;
  int i = 0;
  int closest_dist = 10000;
  point_t closest;

  /* Dispose the old map */
  vMapDispose();

  /* Initialise the background map */
  p_bgmap->width = p_level->w;
  p_bgmap->height = p_level->h;
  p_bgmap->mapoffset = (uint8_t*)p_level->p_level_data;
  p_bgmap->tiledata = (uint8_t*)p_game->p_tiles;
  p_bgmap->flag = 0;
  p_bgmap->format = BG_TILES_FORMAT;    /* the format of the bitmaps */

  p_game->p_cur_level = p_level;

  if ( !vMapInit(p_bgmap) )
    error_msg("vMapInit failed");

  while (p_level->bruce_spawn_points[i].x != -1)
    {
      int dist = abs(p_level->bruce_spawn_points[i].x - p.x) + abs(p_level->bruce_spawn_points[i].y - p.y);

      if (dist < closest_dist)
	{
	  closest = p_level->bruce_spawn_points[i];
	  closest_dist = dist;
	  DbgPrintf("Pt %d:%d closest\n\n", closest.x, closest.y);
	}
      i++;
    }
  p_game->player.sprite.pt = closest;
  sprite_set_state(&p_game->player.sprite, SPRITE_IDLE);

  place_lamps(p_game, p_level);
}
