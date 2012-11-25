/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      levels.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Level data
 *
 * $Id: levels.c 1104 2005-01-16 05:59:19Z ska $
 *
 ********************************************************************/
#include <boulder.h>
#include <levels.h>
#include <specials.h>

static bool_t l1_init_special(game_t *p_game, uint8_t special,
			      tile_t *p_tile, mask_tile_t *p_mask_tile, int16_t x, int16_t y)
{
  elem_t *p_elem;

  switch(special)
    {
    case 0:
      p_elem = elem_add(p_game, LEVER, pt_to_sprite(pt(x, y)), (void*)((lever_state_t)OFF));
      p_elem->u.lever.special = special;
      *p_tile = TILE_EMPTY;
      break;
    case 1:
      p_elem = elem_add(p_game, DIAMOND, pt_to_sprite(pt(x, y)), NULL);
      *p_tile = TILE_EMPTY;
      break;
    default:
      error_msg("Invalid special for level 1 %d\n", special);
      break;
    }

  /* Setup the tiles */
  p_mask_tile->attr |= MASK_ATTR_USER(special);

  return TRUE;
}


special_t l1_specials[] =
  {
    SPECIAL_EXPLOSION(2, POINT(9,1), POINT(6,19)), /* Special 0 */
    SPECIAL_GROWING_WALL( POINT(24,32),TILE_STONE_WALL,UP), /* Special 1 */
  };

special_t l2_specials[] =
  {
    SPECIAL_ELEM_GENERATOR( POINT(3,1),DOWN,GHOST,5,NULL), /* Special 0 */
  };

/* The level definitions */
level_t levels[] =
  {
    { /* Level 0 */
      .w = 23,
      .h = 22,
      .player_pt = {8,8},
      .diamonds_required = 20,
      .time = 150,
      .u.res_id = LEVEL_0,
      .res_packed_size = LEVEL_0_SIZE,
      .res_unpacked_size = LEVEL_0_UNPACKED_SIZE
    },
    { /* Level 1 */
      .w = 37,
      .h = 33,
      .player_pt = {8,8},
      .diamonds_required = 16,
      .time = 200,
      .n_specials = sizeof(l1_specials)/sizeof(special_t),
      .p_specials = l1_specials,
      .init_special_fn = l1_init_special,
      .u.res_id = LEVEL_1,
      .res_packed_size = LEVEL_1_SIZE,
      .res_unpacked_size = LEVEL_1_UNPACKED_SIZE
    },
    { /* Level 2 */
      .w = 40,
      .h = 32,
      .player_pt = {8,8},
      .diamonds_required = 6,
      .time = 300,
      .n_specials = sizeof(l2_specials)/sizeof(special_t),
      .p_specials = l2_specials,
      .init_special_fn = special_init_generic,
      .u.res_id = LEVEL_2,
      .res_packed_size = LEVEL_2_SIZE,
      .res_unpacked_size = LEVEL_2_UNPACKED_SIZE
    },
  };

int N_LEVELS = sizeof(levels) / sizeof(level_t);
