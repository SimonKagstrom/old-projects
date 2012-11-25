/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      levels.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   The level data
 *
 * $Id: levels.c 1104 2005-01-16 05:59:19Z ska $
 *
 ********************************************************************/
#include <game.h>

#include "levels/level_0.h"
#include "levels/level_1.h"
#include "levels/tile_bounds.h"

tile_type_t tile_type_map[] =
{
  EMPTY, /* 0 is unused */
  EMPTY,
  HARD,
  HARD,
  VINES,
  VINES,
  VINES,
  VINES,
};

/**/
uint32_t lamp_mask_0_3;

level_t levels[] =
{
  {
    .w = level_0_WIDTH / TILE_W,
    .h = level_0_HEIGHT / TILE_H,
    .p_level_data = level_0,
    .p_level_mask = level_0_mask,
    /*                        UP, DOWN, LEFT, RIGHT */
    .p_adjacent_screens = { NULL, NULL, NULL, &levels[1]},
    .bruce_spawn_points = level_0_events_bruce_spawn,
    .p_lamp_mask = &lamp_mask_0_3,
    .p_lamps = level_0_events_lamps,
    .n_lamps = sizeof(level_0_events_lamps)/sizeof(lamp_store_t)-1,
  },
  {
    .w = level_1_WIDTH / TILE_W,
    .h = level_1_HEIGHT / TILE_H,
    .p_level_data = level_1,
    .p_level_mask = level_1_mask,
    .p_adjacent_screens = { NULL, NULL, &levels[0], NULL },
    .bruce_spawn_points = level_1_events_bruce_spawn,
  },
};
