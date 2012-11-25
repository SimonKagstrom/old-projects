/*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      example_game.c
 * Description:   Game-specific things for the example.
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
 * $Id: example_game.c 3175 2005-07-07 13:32:32Z ska $
 *
 ********************************************************************/

/* WARNING: If you intend to play the game, you should not read this
 * file since it contains the solution of the game. All scene-specific
 * code is contained in this file.
 *
 * You have been warned!
 */

#include <string.h>            /* memset */
#include "typedefs.h"          /* uint8_t etc */
#include "sysdeps.h"           /* fe_? */

#include "advlib/advlib.h"     /* adv_? */
#include "advglib/advgame.h"   /* advg_? */
#include "advglib/gamedeps.h"  /* definitions that are implemented here */
#include "bitmaps.h"           /* background images etc */

#if defined(TARGET_SDL)
#define DEBUG
#endif /* TARGET_SDL */

#define TILE_GRASS            'A'
#define TILE_VROAD_LEFT       'B'
#define TILE_VROAD_MIDDLE     'C'
#define TILE_VROAD_RIGHT      'D'
#define TILE_HROAD_UP         'E'
#define TILE_HROAD_DOWN       'F'
#define TILE_ROAD_VTOH_UP     'G'
#define TILE_ROAD_VTOH_DOWN   'H'
#define TILE_ROAD_HTOV_LEFT   'I'
#define TILE_ROAD_HTOV_RIGHT  'J'
#define TILE_FLOOR_MMID       'K'
#define TILE_ROCK             'L'
#define TILE_TREE             'M'
#define TILE_HOUSE_DLEFT      'N'
#define TILE_HOUSE_DRIGHT     'O'
#define TILE_HOUSE_ULEFT      'P'
#define TILE_HOUSE_URIGHT     'Q'
#define TILE_MOUNTAIN_ULEFT   'R'
#define TILE_MOUNTAIN_UMIDDLE 'S'
#define TILE_MOUNTAIN_URIGHT  'T'
#define TILE_MOUNTAIN_MLEFT   'U'
#define TILE_MOUNTAIN_MMIDDLE 'V'
#define TILE_MOUNTAIN_MRIGHT  'W'
#define TILE_MOUNTAIN_DLEFT   'X'
#define TILE_MOUNTAIN_DMIDDLE 'Y'
#define TILE_MOUNTAIN_DRIGHT  'Z'
#define TILE_FLOOR_ULEFT      '['
#define TILE_FLOOR_UMID       '\\'
#define TILE_FLOOR_URIGHT     ']'
#define TILE_FLOOR_MLEFT      '^'
#define TILE_FLOOR_MRIGHT     '_'
#define TILE_FLOOR_DLEFT      '`'
#define TILE_FLOOR_DMID       'a'
#define TILE_FLOOR_DRIGHT     'b'

#define OBJ_NONE           0
#define OBJ_ROPE           1
#define OBJ_FRUIT          2
#define OBJ_DIRTY_COINS    3
#define OBJ_BOTTLE         4
#define OBJ_SHINING_COINS  5

#define WORLD_WIDTH        3    /* The number of scenes, horizontally */
#define WORLD_HEIGHT       3    /* The number of scenes, vertically (a 9-scene world) */

#define SCENE_TROLL        0
#define SCENE_START        3
#define SCENE_CABIN        7
#define SCENE_INSIDE_CABIN 8
#define NR_SCENES          (WORLD_WIDTH*WORLD_HEIGHT)

/* Std messages*/
#define NR_STD_MESSAGES       7 /* What value should we have here? */
#define SCENE_MESSAGES_START (ADVG_NR_OBJECTS+NR_STD_MESSAGES)
#define EVENT_MESSAGES_START (SCENE_MESSAGES_START+NR_SCENES*2)

/* Standard messages */
#define YOU_ARE_DEAD          ADVG_NR_OBJECTS
#define WHY_DO_THAT          (ADVG_NR_OBJECTS+1)
#define YOU_TAKE_ITEM        (ADVG_NR_OBJECTS+2)
#define YOU_DROP_ITEM        (ADVG_NR_OBJECTS+3)
#define YOU_CANNOT_TAKE_THAT (ADVG_NR_OBJECTS+4)
#define NOT_USED_LIKE_THAT   (ADVG_NR_OBJECTS+5)
#define NOTHING_SPECIAL      (ADVG_NR_OBJECTS+6)

/* Event messages */
#define EV_START_LOOK_TREE           (EVENT_MESSAGES_START+0)
#define EV_START_USE_ROPE_ON_TREE    (EVENT_MESSAGES_START+1)
#define EV_TROLL_LOOK                (EVENT_MESSAGES_START+3)
#define EV_TROLL_FRUIT               (EVENT_MESSAGES_START+4)
#define EV_TROLL_TAKE                (EVENT_MESSAGES_START+6)
#define EV_TROLL_COINS               (EVENT_MESSAGES_START+7)
#define EV_HOUSE_GARDEN_LOOK         (EVENT_MESSAGES_START+9)
#define EV_HOUSE_DOOR_LOOK           (EVENT_MESSAGES_START+11)
#define EV_TABLE_LOOK                (EVENT_MESSAGES_START+12)
#define EV_TABLE_TAKE_BOTTLE         (EVENT_MESSAGES_START+13)
#define EV_USE_BOTTLE_ON_COINS       (EVENT_MESSAGES_START+14)

static void    handle_player_stops_events(advg_game_t *p_game);
static void    handle_player_events(advg_game_t *p_game);
static void    handle_objects(advg_game_t *p_game, advg_event_t event);
static uint8_t handle_scene_start(advg_game_t *p_game, advg_event_t event);
static uint8_t handle_scene_troll(advg_game_t *p_game, advg_event_t event);
static uint8_t handle_scene_cabin(advg_game_t *p_game, advg_event_t event);
static uint8_t handle_scene_inside_cabin(advg_game_t *p_game, advg_event_t event);
static uint8_t handle_scene_default(advg_game_t *p_game, advg_event_t event);
static void    handle_tick_event(advg_game_t *p_game);

/* This function is called to dispatch events */
void advg_handle_event(advg_game_t *p_game, advg_event_t event)
{
  uint8_t ret;

  /* This is an example event loop.
   *
   * You should note that the code has been made as compact as
   * possible. It can therefore be a bit hard to follow, and it
   * certainly is possible to write much simpler event handlers.
   *
   * The structure is as follows:
   *
   * 1. Check if the event is a TICK, i.e a non-event that is called
   *    on every iteration of the game loop. Since this is the most
   *    frequent event, it is checked before all other events.
   *
   * 2. Check if the event is the EXIT-event. If so, end the game.
   *
   * 3. Clear the scene-events if this is an ENTER_SCENE event (called
   *    upon initial entry of a scene). This is an optimisation and can
   *    otherwise be done in every scene-handler.
   *
   * 4. Execute the scene-specific event handlers (that handles
   *    ENTER_SCENE and the scene-events). If an scene-specific handler
   *    was executed, we are done.
   *
   * 5. Check if the event is a PLAYER_STOP event, i.e. if the player
   *    stops walking. This might have been overridden by the
   *    scene-specific handler. This mainly does things like look at
   *    the scene in general etc.
   *
   * 6. Check if the event is an OBJECT-event, i.e. if the player used
   *    an object.
   *
   * 7. Lastly, check if the event is PLAYER-event. This means that
   *    the user pressed on the player, probably because he used an
   *    object on the player.
   */

  /* Event ticks arrive at each round of the main game loop, and
   * should therefore be handled fast (so we have them first).
   */
  if (advg_event_is(event, ADVG_EVENT_TICK))
    {
      handle_tick_event(p_game);
      return;
    }
  if (advg_event_is(event, ADVG_EVENT_EXIT))
    {
      advg_kill_player(p_game);
      return;
    }

  if (advg_event_is(event, ADVG_EVENT_ENTER_SCENE))
    {
      advg_clear_all_scene_events(p_game);
      advg_clear_messages();
    }
  /* Give the scene event handler the control */
  switch(p_game->scene)
    {
    case SCENE_TROLL:
      ret = handle_scene_troll(p_game, event);
      break;
    case SCENE_START:
      ret = handle_scene_start(p_game, event);
      break;
    case SCENE_CABIN:
      ret = handle_scene_cabin(p_game, event);
      break;
    case SCENE_INSIDE_CABIN:
      ret = handle_scene_inside_cabin(p_game, event);
    default:
      ret = handle_scene_default(p_game, event);
      break;
    }

  /* If the scene specific handler overrides the generic event-handlers, just return here. */
  if (ret)
    return;

  /* Other events, like if the player stops. */
  if (advg_event_is(event, ADVG_EVENT_PLAYER_STOP))
    handle_player_stops_events(p_game);
  else if (advg_event_nr(event) <= ADVG_EVENT_LAST_OBJECT) /* Handle presses on objcets */
    handle_objects(p_game, event);
  else if (advg_event_is(event, ADVG_EVENT_PLAYER)) /* Player-event */
    handle_player_events(p_game);
}

/* This function is called once in each loop to update other players
 * and check if the player should be stopped.
 */
static void handle_tick_event(advg_game_t *p_game)
{
  advg_player_t *p_player = &p_game->player;
  unsigned char coll[4];
  int i;

  /* Fill coll with the blocks that surrounds the player */
  adv_get_surroundings(&p_game->adv_scene, coll, p_player->x, p_player->y,
		       ADVG_PLAYER_WIDTH, ADVG_PLAYER_HEIGHT);
  for (i=0; i<4; i++)
    {
      if (coll[i] >= TILE_ROCK)
	{
	  /* Player has collided with something hard, stop him */
	  advg_stop_player(p_player);
	  break;
	}
      /* Maybe add others here. */
    }
}

/* Handle NULL-events (when the user stops at some place without an
 * event). We know that event is NULL here, so no need to pass
 * that.
 */
static void handle_player_stops_events(advg_game_t *p_game)
{
  switch(p_game->action)
    {
    case ADVG_ACTION_LOOK:
      /* User looks at the scene in general. The scene-descriptions
       * are 2 lines long and start at SCENE_MESSAGES_START
       */
      advg_display_message(p_game, p_game->scene*2+SCENE_MESSAGES_START, 2);
      break;
    case ADVG_ACTION_TAKE_OBJECT:
    case ADVG_ACTION_USE:
      advg_display_message(p_game, WHY_DO_THAT, 1);
    default:
      break;
    }
}


/* Handle global events which involve the player (and are not handled
 * by the scene-specific event-handlers). event is ADVG_EVENT_PLAYER
 * here, so don't pass that.
 */
static void handle_player_events(advg_game_t *p_game)
{
  if (p_game->action == ADVG_ACTION_USE)
    {
      switch (p_game->obj_use)
	{
	case OBJ_FRUIT:
	  advg_display_message(p_game, YOU_ARE_DEAD, 1);
	  advg_kill_player(p_game);
	  return;
	case OBJ_NONE:
	  /* Use pressed and then a press on the player (or somewhere
	   * else) directly
	   */
	default:
	  break;
	}
      /* Use something */
    }
  /* Fall-through for look, take, get etc */
  advg_display_message(p_game, WHY_DO_THAT, 1);
}

/* Handle an object-related event */
static void handle_objects(advg_game_t *p_game, advg_event_t event)
{
  /* The player pressed one of the items. The things here might be
   * overridden by the scene-handlers. A typical example would be
   * dropping: This should be allowed in some scenes, but not in
   * others.
   */
  switch(p_game->action)
    {
    case ADVG_ACTION_LOOK:
      /* Print how the object looks. The first 16 messages are
       * object-description. Note that the object events are numbered
       * from 1 to 16, whereas the messages are numbered from 0 to 15.
       */
      advg_display_message(p_game, event-1, 1);
      break;
    case ADVG_ACTION_USE:
      if (p_game->action == ADVG_ACTION_USE &&
	  p_game->obj_use == OBJ_BOTTLE &&
	  event == OBJ_DIRTY_COINS)
	{
	  advg_display_message(p_game, EV_USE_BOTTLE_ON_COINS, 2);
	  advg_add_object(p_game, OBJ_SHINING_COINS);
	  advg_remove_object(p_game, OBJ_DIRTY_COINS);
	  advg_remove_object(p_game, OBJ_BOTTLE);
	  advg_draw_objects(p_game);
	  break;
	}
      /* Fall-through - using these two objects makes no sense */
    case ADVG_ACTION_TAKE_OBJECT: /* You cannot take something you already have */
      advg_display_message(p_game, WHY_DO_THAT, 1);
    default:
      break;
    }
}

/* Handling for the start scene. Returns TRUE if the event was handled here. */
#define SC_START_TEST_EVENT ADVG_EVENT_FIRST_USER
static uint8_t handle_scene_start(advg_game_t *p_game, advg_event_t event)
{
  switch (advg_event_nr(event))
    {
    case ADVG_EVENT_ENTER_SCENE:
      /* We have entered the scene, now initialise an event on
       * (24,24). This is the tree with the fruit (the coordinates
       * specifies the middle of tile (1,1)).
       */
      advg_init_scene_event(&p_game->scene_events[0], 8, 8, 24, 24, SC_START_TEST_EVENT);
      break;
    case SC_START_TEST_EVENT:
      /* This is the tree-event. If the player just entered or exited
       * the event-point, i.e. if EXIT_EVENT_POINT or ENTER_EVENT_POINT
       * are set, don't do anything.
       */
      if ((event & ADVG_EVENT_EXIT_EVENT_POINT) ||
	  (event & ADVG_EVENT_ENTER_EVENT_POINT))
	break;

      /* Handle the fruit event here */
      if (p_game->action == ADVG_ACTION_USE && p_game->obj_use == OBJ_ROPE)
	{
	  /* The player used the rope on the tree, add the fruit and
	   * redraw the objects.
	   */
	  advg_display_message(p_game, EV_START_USE_ROPE_ON_TREE, 2);
	  advg_add_object(p_game, OBJ_FRUIT);
	  advg_draw_objects(p_game);
	}
      else if (p_game->action == ADVG_ACTION_LOOK) /* Player looks at the tree */
	advg_display_message(p_game, EV_START_LOOK_TREE, 1);
      break;
    default:
      return FALSE;
    }

  /* We override the default handlers */
  return TRUE;
}

#define TROLL_EVENT ADVG_EVENT_FIRST_USER
/* The default scene handler (do nothing special) */
static uint8_t handle_scene_troll(advg_game_t *p_game, advg_event_t event)
{
  /* Don't do anything special on the default scene. */
  switch (advg_event_nr(event))
    {
    case ADVG_EVENT_ENTER_SCENE:
      advg_init_scene_event(&p_game->scene_events[0], 60, 36, 23, 28, TROLL_EVENT);

      /* The troll doesn't move, so we don't need to mask it */
      fe_draw_bitmap(p_troll_bitmap, 70, 52, 0);
      break;
    case TROLL_EVENT:
      /* Don't do anything special on enter/exit */
      if ((event & ADVG_EVENT_EXIT_EVENT_POINT) ||
	  (event & ADVG_EVENT_ENTER_EVENT_POINT))
	break;

      if (p_game->action == ADVG_ACTION_TAKE_OBJECT)
	advg_display_message(p_game, EV_TROLL_TAKE, 1);
      else if (p_game->action == ADVG_ACTION_LOOK) /* Player looks at the troll */
	advg_display_message(p_game, EV_TROLL_LOOK, 1);
      else if (p_game->action == ADVG_ACTION_USE)
	{
	  if (p_game->obj_use == OBJ_SHINING_COINS)
	    {
	      uint8_t buf[2+8];

	      /* Give the coins to the troll. Will work */
	      advg_display_message(p_game, EV_TROLL_COINS, 2);
	      adv_mask_background(&p_game->adv_scene, buf, 70, 52, 8, 8);
	      /* erase the troll */
	      fe_draw_bitmap(buf, 70, 52, 0);
	      advg_remove_object(p_game, OBJ_SHINING_COINS);
	      advg_draw_objects(p_game);
	    }
	  else
	    advg_display_message(p_game, WHY_DO_THAT, 1);
	}

      break;
    default:
      return FALSE;
    }

  return TRUE;
}
#undef TROLL_EVENT

#define CABIN_DOOR_EVENT ADVG_EVENT_FIRST_USER
#define CABIN_GARDEN_EVENT (CABIN_DOOR_EVENT+1)
static uint8_t handle_scene_cabin(advg_game_t *p_game, advg_event_t event)
{
  /* Don't do anything special on the default scene. */
  switch (advg_event_nr(event))
    {
    case ADVG_EVENT_ENTER_SCENE:
      /* Add the garden and the door as events */
      advg_init_scene_event(&p_game->scene_events[1], 80, 20, 32, 22, CABIN_GARDEN_EVENT);
      advg_init_scene_event(&p_game->scene_events[0], 88, 60, 16, 8, CABIN_DOOR_EVENT);
      break;
    case CABIN_GARDEN_EVENT:
      if ((event & ADVG_EVENT_EXIT_EVENT_POINT) ||
	  (event & ADVG_EVENT_ENTER_EVENT_POINT))
	break;
      if (p_game->action == ADVG_ACTION_LOOK ||
	  p_game->action == ADVG_ACTION_TAKE_OBJECT) /* Player looks or takes in the garden */
	{
	  advg_display_message(p_game, EV_HOUSE_GARDEN_LOOK, 2);
	  advg_add_object(p_game, OBJ_DIRTY_COINS);
	  advg_draw_objects(p_game);

	  /* Deregister the event */
	  advg_clear_scene_event(&p_game->scene_events[1]);
	}
      break;
    case CABIN_DOOR_EVENT:
      if ((event & ADVG_EVENT_EXIT_EVENT_POINT) ||
	  (event & ADVG_EVENT_ENTER_EVENT_POINT))
	break;
      if (p_game->action == ADVG_ACTION_LOOK) /* Player looks at the door */
	advg_display_message(p_game, EV_HOUSE_DOOR_LOOK, 1);
      if (p_game->action == ADVG_ACTION_USE &&
	  p_game->obj_use == OBJ_FRUIT)
	{
	  /* The door is unlocked - enter! */
	  advg_goto_scene(p_game, SCENE_INSIDE_CABIN, ADVG_GAME_SCREEN_WIDTH/2, ADVG_GAME_SCREEN_HEIGHT/2+15);
	}
      break;
    default:
      return FALSE;
    }

  return TRUE;
}
#undef CABIN_DOOR_EVENT
#undef CABIN_GARDEN_EVENT

#define BOTTLE_X 16*7+2
#define BOTTLE_Y 16*1+4
#define TABLE_EVENT ADVG_EVENT_FIRST_USER
#define DOOR_EVENT ADVG_EVENT_FIRST_USER+1
/* The default scene handler (do nothing special) */
static uint8_t handle_scene_inside_cabin(advg_game_t *p_game, advg_event_t event)
{
  /* Don't do anything special on the default scene. */
  switch (advg_event_nr(event))
    {
    case ADVG_EVENT_ENTER_SCENE:
      advg_init_scene_event(&p_game->scene_events[0], BOTTLE_X-12, BOTTLE_Y-12, 32, 32, TABLE_EVENT);
      advg_init_scene_event(&p_game->scene_events[1], 40, ADVG_GAME_SCREEN_HEIGHT-16, 80, 16, DOOR_EVENT);

      /* Put the bottle on the table (the bottle is the fourth bitmap) */
      fe_draw_bitmap(p_objects_bitmap+3*(8+2), BOTTLE_X, BOTTLE_Y, 0);
      break;
    case TABLE_EVENT:
      /* Don't do anything special on enter/exit */
      if ((event & ADVG_EVENT_EXIT_EVENT_POINT) ||
	  (event & ADVG_EVENT_ENTER_EVENT_POINT))
	break;
     if (p_game->action == ADVG_ACTION_LOOK)
       advg_display_message(p_game, EV_TABLE_LOOK, 1);
     else if (p_game->action == ADVG_ACTION_TAKE_OBJECT)
       {
	  uint8_t buf[2+8];

	  /* Take the bottle */
	  advg_display_message(p_game, EV_TABLE_TAKE_BOTTLE, 1);
	  adv_mask_background(&p_game->adv_scene, buf, BOTTLE_X, BOTTLE_Y, 8, 8);
	  /* Erase the bottle from the table */
	  fe_draw_bitmap(buf, BOTTLE_X, BOTTLE_Y, 0);
	  advg_add_object(p_game, OBJ_BOTTLE);
	  advg_draw_objects(p_game);

	  /* Deregister the event */
	  advg_clear_scene_event(&p_game->scene_events[0]);
	}

      break;
    case DOOR_EVENT:
      advg_goto_scene(p_game, SCENE_CABIN, 90, 65);
      break;
    default:
      return FALSE;
    }

  return TRUE;
}
#undef TABLE_EVENT
#undef DOOR_EVENT
#undef BOTTLE_X
#undef BOTTLE_Y



/* The default scene handler (do nothing special) */
static uint8_t handle_scene_default(advg_game_t *p_game, advg_event_t event)
{
  /* Don't do anything special on the default scene. */
  if (advg_event_is(event, ADVG_EVENT_ENTER_SCENE))
    return TRUE;

  /* Don't override the other event-handlers if we didn't enter the scene */
  return FALSE;
}



/* --- The game starts here --- */
int main(int argc, char *argv[])
{
  uint8_t tile_buf[NR_TILES*2*16 + 2*NR_TILES];
  advg_game_t game;

  /* Initialise the system-dependent stuff */
  fe_init();

  /* The background data _needs_ to be on the stack */
  memcpy(tile_buf, p_background_data, NR_TILES*2*16 + 2*NR_TILES);

  /* Initialise the adventure game library. */
  advg_init_game(&game, NR_TILES,
		 WORLD_WIDTH, WORLD_HEIGHT,
		 tile_buf,
		 p_player_mask_bitmap, p_player_bitmap,
		 NR_OBJECTS, p_objects_bitmap,
		 "test_scene", "test_messages",
		 NULL);

  /* Draw the game screen */
  advg_draw_screen();

  /* Give the player a rope */
  advg_add_object(&game, OBJ_ROPE);
  advg_draw_objects(&game);

  /* Go to the first scene */
  advg_goto_scene(&game, SCENE_START, ADVG_GAME_SCREEN_WIDTH/2, ADVG_GAME_SCREEN_HEIGHT/2);

  /* Run the game */
  advg_do_game(&game);

  fe_finalize();

  return 0;
}
