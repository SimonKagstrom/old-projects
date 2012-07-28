/*********************************************************************
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      game_loop.c
 * Description:   The main game loop. This runs in the client.
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
 * $Id: game_loop.c,v 1.7 2002/10/31 20:59:44 ska Exp $
 *
 *********************************************************************/
#include "sysdeps.h"
#include <advlib/advlib.h>

#include "advglib/advgame.h"
#include "advg_private.h"
#include "advglib/gamedeps.h"

/* Initialize a scene-event. FIXME: This should be moved to the library when space permits */
void advg_init_scene_event(advg_scene_event_t *p_event, sint16_t x, sint16_t y, uint8_t w, uint8_t h, advg_event_t event)
{
  p_event->x = x;
  p_event->y = y;
  p_event->x_hi = x+w;
  p_event->y_hi = y+h;
  p_event->event = event;

#if defined(ADVG_DEBUG)
  /* Draw lines around the region */
  for(; w>0; w--)
    {
      fe_set_pixel(x+w, y);
      fe_set_pixel(x+w, y+h);
    }
  for(; h>0; h--)
    {
      fe_set_pixel(x, y+h);
      fe_set_pixel(x+p_event->x_hi-p_event->x, y+h);
    }
#endif /* ADVG_DEBUG */
}

/* The main loop of the game */
void advg_do_game(advg_game_t *p_game)
{
  advg_handle_event(p_game, ADVG_EVENT_ENTER_SCENE); /* Enter the first scene */

  while (1)
    {
      uint16_t input = fe_get_buttons();
      advg_event_t event;
      fe_point_t point;

      /* Exit? */
      if ( (input & FE_EVENT_EXIT) || (p_game->state & ADVG_PLAYER_DEAD) )
	{
	  advg_handle_event(p_game, ADVG_EVENT_EXIT);
	  return;
	}

      /* 1. Check input */
      if (fe_get_stylus(&point))
	{
	  if ( (event = _advg_handle_press(p_game, &point)) ) /* lib-call */
	    {
	      advg_handle_event(p_game, event); /* client-call */
	      if (p_game->action)
		_advg_reset_action(p_game); /* lib-call */
	    }

	  /* OPT: Maybe, for the REX' sake, sleep a while here (so
	   * that the user may remove the stylus)
	   */
	  fe_sleep(3);
	}

      /* 2. Timer-tick (things like hitting a rock on the field should be checked here) */
      advg_handle_event(p_game, ADVG_EVENT_TICK);

      /* 3. Move */
      if ( (event = _advg_handle_move(p_game)) )
	{
	  advg_handle_event(p_game, event);
	  /* Reset the action, but not on entering/exiting event points */
	  if (p_game->action && !(event & (ADVG_EVENT_ENTER_EVENT_POINT | ADVG_EVENT_EXIT_EVENT_POINT)) )
	    _advg_reset_action(p_game);
	}
    }
}
