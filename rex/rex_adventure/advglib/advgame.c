/*********************************************************************
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      advgame.c
 * Description:   Generic adventure-game calls.
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
 * $Id: advgame.c,v 1.17 2003/02/16 11:38:54 ska Exp $
 *
 *********************************************************************/
#include <string.h>            /* memset */

#include "typedefs.h"          /* uint8_t etc */
#include "sysdeps.h"           /* fe_? */
#include "advlib/advlib.h"     /* adv_? */

#define ADVG_LIB
#include "advglib/advgame.h"   /* advg_? */
#include "advglib/gamedeps.h"  /* advg_handle_event */

#include "advg_private.h"      /* private library functions (_advg_?) */
#include "advgame_bitmaps.h"   /* button bitmaps */

/* The buttons to the right of the playfield. */
#define BUTTON_LOOK        1
#define BUTTON_USE         2
#define BUTTON_TAKE        3
#define BUTTON_QUIT        4

#define BUTTON_HEIGHT      16
#define BUTTON_WIDTH       32
#define BUTTON_HEIGHT_BITS 4

#define NR_BUTTONS         4

#define OBJECTS_START_W    ADVG_GAME_SCREEN_WIDTH
#define OBJECTS_START_H    (BUTTON_HEIGHT*4)

/**
 * Get the offset of the message by its number. Make this an internal function?
 */
#define GET_MSG_NR(nr) ((nr)*(ADVG_MSG_LINE_LEN+1))

/* Some helper-macros. It makes sense to have them as macros as long
 * as they are only called once, otherwise they are better off as
 * functions.
 */

/* Set the current action to act */
#define set_action(x, act) draw_button((x)->action, 0);   \
			   draw_button((act), 1);         \
			   (x)->action = (act)

/* Check if a point is where the player is */
#define check_point_player(p_player, p_point)                \
	 ((p_point)->x > (p_player)->x                    && \
	  (p_point)->x < (p_player)->x+ADVG_PLAYER_WIDTH  && \
	  (p_point)->y < (p_player)->y+ADVG_PLAYER_HEIGHT && \
	  (p_point)->y > (p_player)->y)

/* Check if the press is within the playfield. */
#define check_point_playfield(p_point)             \
	 ((p_point)->x < ADVG_GAME_SCREEN_WIDTH && \
	  (p_point)->y < ADVG_GAME_SCREEN_HEIGHT)

/* Check if any of the buttons on the right of the screen is pressed.
 * Returns a value between 1 and 6 if a button was pressed, 0 otherwise.
 */
#define check_button_pressed(p_point)                   \
      ( ((p_point)->x > ADVG_GAME_SCREEN_WIDTH &&       \
	 (p_point)->y < (NR_BUTTONS*BUTTON_HEIGHT)) ?   \
       (( ((p_point)->y & ~(BUTTON_HEIGHT-1)) >> BUTTON_HEIGHT_BITS)+1) : 0)


/* Prototypes (for the non-exported functions) */
static void init_player(advg_player_t *p_player, sint16_t x, sint16_t y);
static uint8_t check_scene_events(advg_game_t *p_game);
static uint8_t check_borders(advg_game_t *p_game, uint8_t *p_new_scene, sint16_t *p_new_x, sint16_t *p_new_y);
static uint8_t move_player(advg_game_t *p_game);
static void draw_player(advg_game_t *p_game);
static void draw_button(uint8_t nr, const uint16_t mode);

/* Draw one of the buttons (inverted or non-inverted) */
static void draw_button(uint8_t nr, const uint16_t mode)
{
#if defined(TARGET_REX)
  /* fe_draw_bitmap contains a nasty bug... */
  DsDisplayBitmapDraw(ADVG_GAME_SCREEN_WIDTH, ((nr)-1)*BUTTON_HEIGHT, button_bitmaps+((nr-1)*(BUTTON_HEIGHT*(BUTTON_WIDTH/8)+2)), mode);
#else
  fe_draw_bitmap(button_bitmaps+((nr-1)*(BUTTON_HEIGHT*(BUTTON_WIDTH/8)+2)),
		 ADVG_GAME_SCREEN_WIDTH, ((nr)-1)*BUTTON_HEIGHT, mode);
#endif
}


/* Loop through the events of the scene and see if any is active.
 *
 * This function will only return the first event if the player
 * stands on several events.
 */
static uint8_t check_scene_events(advg_game_t *p_game)
{
  int i;

  /* OPT: Maybe we should rewrite the loop
   *
   * for (i=0; i<5 && p_game->scene_events[i].event; i++)
   * (This function is called in the critical path)
   */
  for (i=0; i<5; i++)
    {
      advg_scene_event_t *p_event = &p_game->scene_events[i];

      if (p_event->event &&
	  p_game->player.x >= p_event->x    &&
	  p_game->player.x <= p_event->x_hi &&
	  p_game->player.y >= p_event->y    &&
	  p_game->player.y <= p_event->y_hi)
	{
	  if (p_event->event != p_game->curr_event)
	    {
	      /* An event has just been entered.*/
	      p_game->curr_event = p_event->event;
	      return (p_event->event | ADVG_EVENT_ENTER_EVENT_POINT);
	    }
	  /* Not an enter-event: if the player stops - return the event */
	  if (p_game->player.x == p_game->player.dst_x &&
	      p_game->player.y == p_game->player.dst_y)
	    return p_event->event;

	  /* No event otherwise */
	  return ADVG_EVENT_NULL;
	}
    }

  if (p_game->curr_event)
    {
      advg_event_t ret = p_game->curr_event;

      /* Player was standing on an event-point, but left it now */
      p_game->curr_event = ADVG_EVENT_NULL;
      return (ret | ADVG_EVENT_EXIT_EVENT_POINT);
    }

  /* The player does not stand close to an event */
  return ADVG_EVENT_NULL;
}


/* Check if the player tries to leave the current scene, return TRUE if the player should exit, false otherwise. */
static uint8_t check_borders(advg_game_t *p_game, uint8_t *p_new_scene, sint16_t *p_new_x, sint16_t *p_new_y)
{
  /* The divisions should not be very common here, only executed when
   * the player is at the borders. We should remove the ADVG_PLAYER_WIDTH and
   * and ADVG_PLAYER_HEIGHT definitions from this file, which will cause some
   * problems for this function.
   */
  if (p_game->player.x < 5)
    {
      /* Move to the left */
      if(p_game->scene % p_game->world_w)
	{
	  *p_new_x = (ADVG_GAME_SCREEN_WIDTH-ADVG_PLAYER_WIDTH-10);
	  *p_new_scene = p_game->scene-1;
	  goto horiz;
	}
    }
  else if (p_game->player.x > (ADVG_GAME_SCREEN_WIDTH-ADVG_PLAYER_WIDTH-5))
    {
      /* Move to the right */
      if (p_game->scene % p_game->world_w < p_game->world_w-1)
	{
	  *p_new_x = 10;
	  *p_new_scene = p_game->scene+1;
	  goto horiz;
	}
    }
  else if (p_game->player.y < 5)
    {
      /* Move up */
      if (p_game->scene >= p_game->world_w)
	{
	  *p_new_y = (ADVG_GAME_SCREEN_HEIGHT-ADVG_PLAYER_HEIGHT-10);
	  *p_new_scene = p_game->scene-p_game->world_w;
	  goto vert;
	}
    }
  else if (p_game->player.y > (ADVG_GAME_SCREEN_HEIGHT-ADVG_PLAYER_HEIGHT-5))
    {
      /* Move down */
      if (p_game->scene < (p_game->world_h-1)*p_game->world_w)
	{
	  *p_new_y = 10;
	  *p_new_scene = p_game->scene+p_game->world_w;
	  goto vert;
	}
    }
  else
    return FALSE; /* Player moves around on the middle of the screen, do nothing */

  /* Stop the player if he tries to go outside the world */
  init_player(&p_game->player, p_game->player.x, p_game->player.y);
  return FALSE;
 horiz:
  *p_new_y = (ADVG_GAME_SCREEN_HEIGHT/2);
  return TRUE;
 vert:
  *p_new_x = (ADVG_GAME_SCREEN_WIDTH/2);
  return TRUE;
}

/* Move the player. Returns 1 if the player is at the destination, 0
 * otherwise.
 */
static uint8_t move_player(advg_game_t *p_game)
{
  advg_player_t *p_player = &p_game->player;

  p_player->last_x = p_player->x;
  p_player->last_y = p_player->y;

  /* Player is standing on the destination, don't move */
  if (p_player->x == p_player->dst_x &&
      p_player->y == p_player->dst_y)
    return FALSE;

  /* Move player first horizontally, then vertically.
   *
   * OPT: Maybe this should be: Move player the shortest path first,
   * then the longest. (Or the other way around?)
   */
  if (p_player->x != p_player->dst_x)
    p_player->x += (p_player->dst_x > p_player->x)?1:-1;
  else if (p_player->y != p_player->dst_y)
    p_player->y += (p_player->dst_y > p_player->y)?1:-1;

  return TRUE;
}

/* Draw the player */
static void draw_player(advg_game_t *p_game)
{
  uint8_t bitmap[ADVG_PLAYER_HEIGHT*ADVG_PLAYER_WIDTH_BYTES+2];
  advg_player_t *p_player = &p_game->player;

  if (p_player->x != p_player->last_x ||
      p_player->y != p_player->last_y)
    {
      /* Player has moved - Redraw old background */
      adv_mask_background(&p_game->adv_scene, bitmap, p_player->last_x,
			  p_player->last_y, ADVG_PLAYER_WIDTH, ADVG_PLAYER_HEIGHT);
      fe_draw_bitmap(bitmap, p_player->last_x, p_player->last_y, 0);
    }
  /* Draw the player */
  adv_mask_object(&p_game->adv_scene, bitmap, p_game->p_player_mask,
		  p_game->p_player_bitmap, p_player->x, p_player->y);
  fe_draw_bitmap(bitmap, p_player->x, p_player->y, 0);
}


/* Reset the current action */
void _advg_reset_action(advg_game_t *p_game)
{
  /* Reset the ongoing action */
  draw_button(p_game->action, 0);
  p_game->obj_use = 0;
  advg_draw_objects(p_game);
  p_game->action = ADVG_ACTION_NULL;
}

/* Redraw the game screen */
void advg_draw_screen(void)
{
  int i;

  /* Draw the buttons */
  for (i=BUTTON_LOOK; i<(BUTTON_QUIT+1); i++)
    draw_button(i, 0);

  /* Draw some lines */
  fe_fill_area(0, ADVG_GAME_SCREEN_HEIGHT, FE_PHYS_WIDTH, 1);
}

/* Initialize a player struct */
void init_player(advg_player_t *p_player, sint16_t x, sint16_t y)
{
  p_player->last_x = p_player->dst_x = p_player->x = x;
  p_player->last_y = p_player->dst_y = p_player->y = y;
}

/* Display messages in the message area. */
void advg_display_message(advg_game_t *p_game, sint16_t nr, uint8_t n)
{
  char buf[2*ADVG_MSG_LINE_LEN+2]; /* Text, two NULL-chars */
  int i;

  /* Load message(s). Is this feasible on the REX? */
  fe_load_data(buf, GET_MSG_NR(nr), n*(ADVG_MSG_LINE_LEN+1), p_game->p_messages_filename);
  buf[ADVG_MSG_LINE_LEN] = '\0';
  buf[2*ADVG_MSG_LINE_LEN] = '\0'; /* Null-terminate even if the data is not read */

  /* Clear the message area */
  fe_clear_area(0, ADVG_GAME_SCREEN_HEIGHT+1, FE_PHYS_WIDTH, FE_PHYS_HEIGHT-ADVG_GAME_SCREEN_HEIGHT);
  for (i=0; i<n; i++)
    fe_print_xy(0, ADVG_GAME_SCREEN_HEIGHT+i*10, 0, buf+i*(ADVG_MSG_LINE_LEN+1) );
}

/* Draw all objects the player is carrying */
void advg_draw_objects(advg_game_t *p_game)
{
  int i;

  for(i=0; i<16; i++)
    {
      sint16_t x = OBJECTS_START_W+((i&3)<<3);  /* WIDTH + (i%4)*8 */
      sint16_t y = OBJECTS_START_H+((i>>2)<<3); /* HEIGHT + (i/4)*8 */

      if (p_game->objects & (1<<i)) /* The player has the object */
	fe_draw_bitmap(p_game->p_obj_bitmap + (i*10), x , y, (p_game->obj_use-1 == i)?1:0);
      else
	fe_clear_area(x, y, 8, 8);
    }
}

/* Load a scene and place the player there */
void advg_goto_scene(advg_game_t *p_game, sint8_t scene, sint16_t x, sint16_t y)
{
  p_game->scene = scene;
  adv_load_scene(&p_game->adv_scene, p_game->p_scene_filename, p_game->scene);
  init_player(&p_game->player, x, y);

  /* Enter the new scene and draw that */
  adv_draw_scene(&p_game->adv_scene);
}

/* Handle touchscreen presses */
advg_event_t _advg_handle_press(advg_game_t *p_game, fe_point_t *p_point)
{
  advg_event_t event=ADVG_EVENT_NULL;
  uint16_t btn;

  /* The touch-screen was used. The things executed inside this
   * if-check are not time-critical.
   *
   * Add someplace here: Redraw the icons with the selected one inverted.
   */
  if ( (btn = check_button_pressed(p_point)) )
    {
      /* User pressed one of the buttons. */
      set_action(p_game, btn);
      if (btn == BUTTON_QUIT)
	return ADVG_EVENT_EXIT;
    }
  else if (p_game->action)
    {
      if (check_point_player(&p_game->player, p_point))
	event = ADVG_EVENT_PLAYER; /* User pressed on the player */
      else if (p_point->x > ADVG_GAME_SCREEN_WIDTH &&
	       p_point->y > (OBJECTS_START_H))
	{
	  /* This is a press in the objects field, which object? */
	  event = (((p_point->y-(OBJECTS_START_H))>>3)<<2)+((p_point->x-ADVG_GAME_SCREEN_WIDTH)>>3)+1;

	  /* If the player has pressed use, set the object as the
	   * current use-object, otherwise generate an event for
	   * the object.
	   */
	  if ( advg_has_object(p_game, event) )
	    {
	      /* Player has this object */
	      if (p_game->action == ADVG_ACTION_USE &&
		  !p_game->obj_use)
		{
		  /* Use has been pressed and now the object. */
		  p_game->obj_use = event;
		  event = ADVG_EVENT_NULL;
		  advg_draw_objects(p_game);
		}
	    }
	  else /* Player is not allowed to look at objects he/she does not have */
	    event = 0;
	}
    }

  if (!event && check_point_playfield(p_point))
    {
      /* Press in the playfield (but not on the player) */
      p_game->player.dst_x = p_point->x;
      p_game->player.dst_y = p_point->y;
    }

  return event;
}

/* Handle player moves */
advg_event_t _advg_handle_move(advg_game_t *p_game)
{
  /* Draw the player even if it has not moved */
  draw_player(p_game);

  /* Move player (returns TRUE if player is moving) */
  if (move_player(p_game))
    {
      sint16_t new_x, new_y;
      uint8_t new_scene;
      advg_event_t event;

      /* If the player enters an event point or stops walking,
       * generate an event.
       */
      if ( (event = check_scene_events(p_game)) )
	return event;

      /* Check if the player exits this scene */
      if (check_borders(p_game, &new_scene, &new_x, &new_y))
	{
	  advg_goto_scene(p_game, new_scene, new_x, new_y);
	  return ADVG_EVENT_ENTER_SCENE;
	}

      /* Player stops walking */
      if (p_game->player.x == p_game->player.dst_x &&
	  p_game->player.y == p_game->player.dst_y)
	return ADVG_EVENT_PLAYER_STOP;
    }

  return ADVG_EVENT_NULL;
}
