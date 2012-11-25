/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      battle.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   archon battle
 *
 * $Id:$
 *
 ********************************************************************/
#include <battle.h>

static uint8_t power_by_tile[] =
{
  2
  -2,
  0,
};

creature_t *battle_do(game_t *game, tile_t tile,
		      creature_t *light, creature_t *dark)
{
  int light_cycle = game->light_cycle - 3;
  int dark_strength = power_by_tile[tile.type] + light_cycle;
  int light_strength = -(power_by_tile[tile.type] + light_cycle);
  creature_t *winner = light;

  printf("Maboo! Battling between %d and %d!\n", light->type, dark->type);

  return winner;
}
