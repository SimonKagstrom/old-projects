/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      game.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Archon
 *
 * $Id:$
 *
 ********************************************************************/
#include <string.h>
#include <game.h>
#include <battle.h>
#include <point.h>

#include "bitmaps.h"

#define BRICK_WIDTH  13
#define BRICK_HEIGHT 11

static const uint8_t *tile_by_type[ 3 ] =
{
  image_dark,
  image_light,
  image_magenta,
};

static const uint8_t *piece_by_type[ 8 * 2 ] =
{
  image_valkyrie, image_banshee,
  image_golem,    image_troll,
  image_unicorn,  image_basilisk,
  image_dinji,    image_shapeshifter,
  image_wizard,   image_sorceress,
  image_phoenix,  image_dragon,
  image_archer,   image_manticore,
  image_knight,   image_goblin,
};

static void game_draw_one_pos(game_t *game, point_t where,
			      int inverted)
{
  tile_t tile = playfield_get_tile(&game->playfield, where);
  const uint8_t *tile_image = tile_by_type[ tile.type ];
  int x = where.x;
  int y = where.y;

  fe_draw_bitmap(tile_image,
		 x * BRICK_WIDTH, y * BRICK_HEIGHT, 0);

  if (playfield_is_powerpoint(where))
    fe_draw_bitmap(image_powerpoint,
		   x * BRICK_WIDTH + (BRICK_WIDTH/2 - 3), y * BRICK_HEIGHT + (BRICK_HEIGHT/2 - 3), 0);

  if (tile.piece)
    {
      int add = tile.piece->dark ? 1 : 0;
      const uint8_t *piece_image = piece_by_type[tile.piece->type*2 + add];

      fe_draw_bitmap(piece_image,
		     x * BRICK_WIDTH + (8-BRICK_WIDTH/2), y * BRICK_HEIGHT + (8-BRICK_HEIGHT/2), inverted);
    }
}

static void game_draw_status(game_t *game, int turn)
{
}

static void game_draw(game_t *game, int turn)
{
  uint8_t x,y;

  /* fe_draw_bitmap(title_image, FE_PHYS_WIDTH - 64, 2, 0); */

  game_draw_status(game, turn);

  fe_print_xy(PF_W * BRICK_WIDTH + 4, 46, FE_FONT_NORMAL, "Status");
  /* Draw a line (captured pieces below) */
  fe_fill_area(PF_W * BRICK_WIDTH + 4, 58,
	       FE_PHYS_WIDTH - PF_W * BRICK_WIDTH - 8, 1);

  /* Draw the board */
  for (y = 0; y < PF_H; y++)
    {
      for (x = 0; x < PF_W; x++)
	game_draw_one_pos(game, point(x, y), 0);
    }
}

static void game_do_round_player(game_t *game)
{
  playfield_t *pf = &game->playfield;
  creature_t *selected = NULL;
  int spell = SPELL_NONE;

  /* Wait for input */
  while ( 1 )
    {
      fe_point_t where;

      if ( fe_get_buttons() == FE_EVENT_EXIT)
	fe_finalize();

      if (fe_get_stylus(&where) == TRUE)
	{
	  /* got it! */
	  point_t field_pt = point(where.x / BRICK_WIDTH, where.y / BRICK_HEIGHT);

	  if (field_pt.x >= PF_W ||
	      field_pt.y >= PF_H)
	    {
	      /* Press outside the field */

	      /*
	       * Handle spells: press the sorceress, press type of
	       * spell and then destination
	       */
	      if (0)
		spell = SPELL_TELEPORT; /* ... */
	    }
	  else if (!selected)
	    {
	      creature_t *cur = playfield_get_creature(pf, field_pt);

	      /* Nothing to select */
	      if ( !cur || (cur && cur->dark != game->turn) )
		{
		  selected = NULL;
		  continue;
		}

	      selected = cur;
	      game_draw_one_pos(game, field_pt, TRUE);
	    }
	  else /* selected */
	    {
	      point_t src = selected->pos;
	      point_t dst = field_pt;
	      move_res_t res;

	      res = playfield_can_move(pf, &dst,
				       src, dst);
	      if (res == OK)
		{
		  playfield_do_move(pf, src, dst);
		  game_draw_one_pos(game, src, FALSE);
		  game_draw_one_pos(game, dst, FALSE);
		  return;
		}
	      else if (res == BATTLE)
		{
		  creature_t *other = playfield_get_creature(pf, dst);
		  creature_t *light;
		  creature_t *dark;
		  creature_t *winner;

		  if (selected->dark)
		    {
		      light = other;
		      dark = selected;
		    }
		  else
		    {
		      light = selected;
		      dark = other;
		    }

		  winner = battle_do(game, playfield_get_tile(pf, dst),
				     light, dark);
		  playfield_remove_creature(pf, src);
		  playfield_set_creature(pf, winner, dst);
		  return;
		}
	      /* NOK unselect */

	      /* Clear tile */
	      game_draw_one_pos(game, selected->pos, FALSE);
	      selected = NULL;
	    }
	}
    }
}

static void game_do(game_t *game)
{
  while (1)
    {
      int cycle = game->light_cycle;

      game_do_round_player(game);

      /* Update the light cycle and turn */
      game->turn = !game->turn;
      cycle++;
      cycle = cycle % 6;
      game->light_cycle = cycle;
    }
}

static void game_init(game_t *game)
{
  memset(game, 0, sizeof(game_t));
  playfield_init(&game->playfield);
}

int main(int argc, char *argv[])
{
  game_t game;

  fe_init();
  fe_clear_screen();

  game_init(&game);
  game_draw(&game, 0);
  game_do(&game);

  return 0;
}
