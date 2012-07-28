/*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      null_game.c
 * Description:   An empty definition of a game.
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
 * $Id: null_game.c,v 1.6 2002/11/09 11:25:57 ska Exp $
 *
 ********************************************************************/
#include "sysdeps.h"           /* fe_init() etc */
#include "advglib/advgame.h"   /* advg_init_game() etc */
#include "advglib/gamedeps.h"  /* advg_handle_event()  */

#include "../example_game/bitmaps.h"  /* Some sample bitmaps. Change this to your own */

#define WORLD_WIDTH  2 /* 1x1 worlds are not allowed */
#define WORLD_HEIGHT 2

/* A tile is a 16*16 bitmap, i.e. 2*16 bytes big plus 2 w/h bytes. */
#define TILE_SIZE    (NR_TILES*2*16 + 2*NR_TILES)

void advg_handle_event(advg_game_t *p_game, advg_event_t event)
{
  /* We only check for the exit event here. */
  if (advg_event_is(event, ADVG_EVENT_EXIT))
    {
      /* Kill the player (game will exit) */
      advg_kill_player(p_game);
      return;
    }

  /* Implement your game events here. */
}

void main()
{
  uint8_t tile_buf[TILE_SIZE];
  advg_game_t game;

  /* Initialise the system-dependent stuff */
  fe_init();

  /* The background data _needs_ to be on the stack */
  memcpy(tile_buf, p_background_data, TILE_SIZE);

  /* Initialise the adventure game library. */
  advg_init_game(&game, NR_TILES,
		 WORLD_WIDTH, WORLD_HEIGHT,
		 tile_buf,
		 p_player_mask_bitmap, p_player_bitmap,
		 NR_OBJECTS, p_objects_bitmap,
		 "test_scene", "test_messages",
		 NULL);

  /* Draw the objects (none here) */
  advg_draw_objects(&game);

  /* Draw the game screen */
  advg_draw_screen();

  /* Go to the first scene (scene 0), start in the middle */
  advg_goto_scene(&game, 0, ADVG_GAME_SCREEN_WIDTH/2, ADVG_GAME_SCREEN_HEIGHT/2);

  /* Run the game */
  advg_do_game(&game);

  /* When we are here, the game is over */
  fe_finalize();
}
