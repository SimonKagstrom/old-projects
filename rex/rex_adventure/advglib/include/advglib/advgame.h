/*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      advgame.h
 * Description:   Definitions for a test game for advlib.
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
 * $Id: advgame.h,v 1.16 2003/03/02 14:04:37 ska Exp $
 *
 ********************************************************************/
#ifndef ADVGAME_H
#define ADVGAME_H

#include <string.h>            /* strncpy */

#include "typedefs.h"
#include "advlib/advlib.h"

/** @file
 *
 * Adventure game library. This library implements game independent
 * routines for drawing, moving around and doing operations like look,
 * take, drop and use. It depends on advlib for scene-handling and
 * graphics routines.
 *
 * To use the library you have to write a c-file that contains a
 * main-function and implementations of the functions in
 * gamedeps.h. The required functions deal with event-handling and
 * reaction to collisions with the background.
 *
 * Each game also needs two files, a scene definition file and a file
 * with game messages. The scene file is described in the
 * documentation of advlib.h. The messages file is a text file with
 * messages. Each message can be @c ADVG_MSG_LINE_LEN characters long,
 * and each line <I>must</I> be that long (i.e. should be padded with
 * whitespaces).
 */

#if defined(TARGET_REX) || defined(TARGET_SDL)
# define __USE_DEFINES /* Sorry, this is used to fool doxygen */
#endif

#define ADVG_SCENE_WIDTH        13   /* Tiles are 16*16 -> total width is 208 pixels */
#define ADVG_SCENE_HEIGHT       6    /* Corresponding height is 120 pixels or 5 blocks */
#define ADVG_GAME_SCREEN_WIDTH  (FE_PHYS_WIDTH-32)
#define ADVG_GAME_SCREEN_HEIGHT (FE_PHYS_HEIGHT-24)

#define ADVG_PLAYER_WIDTH        8 /* REMOVE ME! */
#define ADVG_PLAYER_HEIGHT       8
#define ADVG_PLAYER_WIDTH_BYTES  1

#define ADVG_OBJECT_WIDTH        8
#define ADVG_OBJECT_HEIGHT       8

#define ADVG_PLAYER_DEAD        (1<<7) /* State */

#define ADVG_NR_OBJECTS         16 /**< The maximum number of objects the player can have */
#define ADVG_MSG_LINE_LEN       48 /**< The maximum length of messages (48 chars) */

#define ADVG_ACTION_NULL         0
#define ADVG_ACTION_LOOK         1 /* Corresponds to the buttons above */
#define ADVG_ACTION_USE          2
#define ADVG_ACTION_TAKE_OBJECT  3

#define __MAX_STR_LEN            20

/* Events, stored in a 8 bit integer.
 *
 * There are predefined events for a NULL-event, all objects (1..16),
 * entering a scene, presses on the player and entering and exiting an
 * event-point.
 */
#define ADVG_EVENT_NULL               0 /* No event */
#define ADVG_EVENT_FIRST_OBJECT       1
#define ADVG_EVENT_LAST_OBJECT       16 /* Object-events are between 1 and 16*/
#define ADVG_EVENT_ENTER_SCENE       17 /* When the player enters the scene */
#define ADVG_EVENT_PLAYER            18 /* A press on the player */
#define ADVG_EVENT_TICK              19 /* One loop has passed */
#define ADVG_EVENT_PLAYER_STOP       20 /* The player stops walking */
#define ADVG_EVENT_EXIT              21 /* The exit-button was pressed */
#define ADVG_EVENT_FIRST_USER        25 /* Above this are user-specified events (on scenes, for instance) */
#define ADVG_EVENT_EXIT_EVENT_POINT  (1<<6) /* Player exits an event-point */
#define ADVG_EVENT_ENTER_EVENT_POINT (1<<7) /* Player entered an event-point */

/* Helper macros. */
/**
 * Remove an object from the player. This function is normally used
 * from the event-handlers, where the object-events are passed as a
 * value between 1 and 16.
 *
 * @param p_game a pointer to the game
 * @param nr the number of the object to remove (between 1 and 16)
 */
#if defined(__USE_DEFINES)
#define advg_remove_object(p_game, nr) (((p_game)->objects) &= ~(1<<((nr)-1)))
#else
void advg_remove_object(advg_game_t *p_game, int nr);
#endif

/**
 * Add an object to the player.
 *
 * @param p_game a pointer to the game.
 * @param nr the number of the object to add (between 1 and 16).
 */
#if defined(__USE_DEFINES)
#define advg_add_object(p_game, nr) (((p_game)->objects) |= (1<<((nr)-1)))
#else
void advg_add_object(advg_game_t *p_game, int nr);
#endif

/**
 * Check if the player has an object or not.
 *
 * @param p_game a pointer to the game.
 * @param nr the number of the object to check (between 1 and 16).
 *
 * @return TRUE if the player has the object, FALSE otherwise
 */
#if defined(__USE_DEFINES)
#define advg_has_object(p_game, nr) (((p_game)->objects) & (1<<((nr)-1)))
#else
uint8_t advg_has_object(advg_game_t *p_game, int nr);
#endif

/**
 * End the life of the player.
 *
 * @param p_game the current game.
 */
#if defined(__USE_DEFINES)
#define advg_kill_player(p_game) ((p_game)->state |= ADVG_PLAYER_DEAD)
#else
void advg_kill_player(advg_game_t *p_game);
#endif

/**
 * Get the number of an event.
 *
 * @param event the event to check.
 *
 * @return the number of the event.
 */
#if defined(__USE_DEFINES)
#define advg_event_nr(event) ((event) & ~(ADVG_EVENT_ENTER_EVENT_POINT | ADVG_EVENT_EXIT_EVENT_POINT))
#else
uint8_t advg_event_nr(advg_event_t event);
#endif

/**
 * Returns if an event is a certain number.
 *
 * @param event the event to check.
 * @param nr the number to check.
 *
 * @return TRUE if the event is nr, FALSE otherwise.
 */
#if defined(__USE_DEFINES)
#define advg_event_is(event, nr) (((event) & ~(ADVG_EVENT_ENTER_EVENT_POINT | ADVG_EVENT_EXIT_EVENT_POINT)) == (nr))
#else
uint8_t advg_event_is(advg_event_t event, uint8_t nr);
#endif

/**
 * The event type.
 *
 * @see advg_handle_event()
 */
typedef uint8_t advg_event_t;

/**
 * The container for the objects.
 */
typedef uint16_t advg_objects_t;

/**
 * The structure for scene-events. This structure is used to specify
 * event-regions on a scene. The structure should be filled in by
 * advg_init_scene_event(), and this should be done from the
 * game-specific implementation (in advg_handle_event()).
 *
 * @todo this will need some explanation.
 * @see advg_init_scene_event()
 * @see advg_handle_event().
 */
typedef struct
{
  sint16_t x;         /**< The low x-coordinate of the event region */
  sint16_t y;         /**< The low y-coordinate of the event region */
  sint16_t x_hi;      /**< The high x-coordinate of the event region */
  sint16_t y_hi;      /**< The high y-coordinate of the event region */
  advg_event_t event; /**< The event-number */
} advg_scene_event_t;

/**
 * The structure for the player.
 */
typedef struct
{
  sint16_t x;      /**< Current x-position */
  sint16_t y;      /**< Current y-position */
  sint16_t last_x; /**< Last x-position (used to erase the player from its last position) */
  sint16_t last_y; /**< Last y-position */
  sint16_t dst_x;  /**< The destination x-position (where the player wants to go) */
  sint16_t dst_y;  /**< The destination y-position */
} advg_player_t;

/**
 * The main game structure. You will probably not need to access this
 * structure directly.
 *
 * @see advg_init_game()
 * @see advg_do_game()
 */
typedef struct
{
  advg_player_t  player;               /**< The player */
  uint8_t        scene;                /**< The number of the current scene */
  uint8_t        world_w;              /**< The width of the world (in scenes) */
  uint8_t        world_h;              /**< The height of the world (in scenes) */
  adv_scene_t    adv_scene;            /**< The current scene-struct */
  uint8_t        scene_buf[ADVG_SCENE_WIDTH*ADVG_SCENE_HEIGHT]; /**< The char-buffer for the scenes */
  advg_scene_event_t  scene_events[5]; /**< Events on the current scene */
  char           p_scene_filename[__MAX_STR_LEN]; /**< The filename of the file with the scene data.
						     This must not be larger than 20 characters. */
  char           p_messages_filename[__MAX_STR_LEN]; /**< The filename of the file with the scene messages */
  uint8_t        p_player_mask[ADVG_PLAYER_HEIGHT+2]; /**< The mask for the player */
  uint8_t        p_player_bitmap[ADVG_PLAYER_HEIGHT+2]; /**< The bitmap for the player */
  uint8_t        p_obj_bitmap[16*(ADVG_OBJECT_HEIGHT+2)]; /**< The bitmap for objects carried by the player. */
  uint16_t       action;               /**< Flags for events (things like "look pressed" etc) */
  uint8_t        state;                /**< The state of the game (if the player is alive etc) */
  uint8_t        obj_use;              /**< The object currently in a ACTION_USE-operation */
  advg_objects_t objects;              /**< The objects the player has access to. This is a bitfield. */
  advg_event_t   curr_event;           /**< The current event point (if any) */

  void          *p_user;               /**< A pointer to an user-defined structure */
} advg_game_t;

/**
 * Fill in a scene event. This should be called when a new scene is
 * entered. When the player enters, exits or does something in this
 * region, the associated event will be generated. The position of the
 * player is counted from the upper left corner of the player bitmap.
 *
 * @param p_event the event to fill.
 * @param x the x-position of the event region.
 * @param y the y-position of the event region.
 * @param w the width of the region.
 * @param h the height of the region.
 * @param event the number of the event.
 */
extern void advg_init_scene_event(advg_scene_event_t *p_event, sint16_t x, sint16_t y, uint8_t w, uint8_t h,
				  advg_event_t event);

/**
 * Clear a scene event. If the event was active, this will remove the
 * event.
 *
 * @param p_event A pointer to the event to clear.
 */
#if defined(__USE_DEFINES)
#define advg_clear_scene_event(p_event) ((p_event)->event = 0)
#else
void advg_clear_scene_event(advg_scene_event_t *p_event);
#endif

/**
 * Clear all scene events. This is typically done when entering a new
 * scene (so that no old events remain on the new scene).
 *
 * @param p_game a pointer to the current game.
 */
#if defined(__USE_DEFINES)
#define advg_clear_all_scene_events(p_game) memset((p_game)->scene_events, 0, sizeof(advg_scene_event_t)*5)
#else
void advg_clear_all_scene_events(advg_game_t *p_game);
#endif

/**
 * Display messages in the message area.
 *
 * @param p_game the game to display messages for.
 * @param nr the line number of the first message to show.
 * @param n the number of messages to show, starting at nr. This
 *  is either 1 or 2. 0 will cause an error.
 */
extern void advg_display_message(advg_game_t *p_game, sint16_t nr, uint8_t n);

/**
 * Clear the message area.
 */
#if defined(__USE_DEFINES)
#define advg_clear_messages() fe_clear_area(0, ADVG_GAME_SCREEN_HEIGHT+1, FE_PHYS_WIDTH, FE_PHYS_HEIGHT-ADVG_GAME_SCREEN_HEIGHT)
#else
void advg_clear_messages(void);
#endif

/**
 * Stop the player from walking. This should be called when the player
 * steps on an object which it cannot pass through, e.g. a rock or
 * something.
 *
 * @param p_player A pointer to the advg_player_t player to stop.
 */
#if defined(__USE_DEFINES)
#define advg_stop_player(p_player) (p_player)->dst_x = (p_player)->last_x; (p_player)->dst_y = (p_player)->last_y
#else
void advg_stop_player(advg_player_t *p_player);
#endif


/**
 * Draw the objects that the player currently have.
 *
 * @param p_game the game.
 */
extern void advg_draw_objects(advg_game_t *p_game);

/**
 * Go to a scene. This should be called after initialising the game
 * sturcture with advg_init_game(), to enter the start scene.
 *
 * @param p_game the current game.
 * @param scene the scene to go to.
 * @param x the x-position of the player on the new scene.
 * @param y the y-position of the player on the new scene.
 */
extern void advg_goto_scene(advg_game_t *p_game, sint8_t scene, sint16_t x, sint16_t y);
#if defined(TARGET_SDL) && !defined(ADVG_LIB)
#define advg_goto_scene(p_game, scene, x, y) advg_goto_scene(p_game, scene, x, y); \
                                             advg_handle_event(p_game, ADVG_EVENT_ENTER_SCENE)
#endif

/**
 * Draw the screen - show the buttons on the right side of the screen
 * etc.
 */
extern void advg_draw_screen(void);

/**
 * Initialize a game structure. This function has to be called before
 * any other function in advgame, or advlib. Failure to do so will
 * cause a crash.
 *
 * @param p_game the game-structure to initialize
 * @param nr_tiles_ the number of unique tiles (background images) in
 *  the game.
 * @param world_w_ the width of the world (in scenes)
 * @param world_h_ the height of the world
 * @param p_scene_objs_ the background tiles. These have to reside on
 *  the stack.
 * @param p_player_mask_ the mask-bitmap for the player.
 * @param p_player_bitmap_ the bitmap of the player.
 * @param nr_objs_ the number of objects in the game (max 16).
 * @param p_obj_bitmap_ the bitmap for objects carried by the player.
 * @param p_scene_filename_ the filename of the file containing the
 *  scene data.
 * @param p_messages_filename_ the filename of the file containing the
 *  messages.
 * @param p_user_ a pointer to a user-specified structure. This should
 *  be used to store the private game state (like if the player has
 *  taken a certain thing etc).
 */
#if defined(__USE_DEFINES)
#define advg_init_game(p_game, nr_tiles_,                                     \
		       world_w_, world_h_,                                    \
		       p_scene_objs_,                                         \
		       p_player_mask_, p_player_bitmap_,                      \
		       nr_objs_, p_obj_bitmap_,                               \
		       p_scene_filename_, p_messages_filename_,               \
		       p_user_)                                               \
{                                                                             \
  memset((p_game), 0, sizeof(advg_game_t));				      \
  (p_game)->world_w = world_w_;                                               \
  (p_game)->world_h = world_h_;                                               \
  (p_game)->p_user = p_user_;                                                 \
  memcpy((p_game)->p_player_mask, p_player_mask_, ADVG_PLAYER_HEIGHT+2);      \
  memcpy((p_game)->p_player_bitmap, p_player_bitmap_, ADVG_PLAYER_HEIGHT+2);  \
  strncpy((p_game)->p_scene_filename, p_scene_filename_, __MAX_STR_LEN);      \
  strncpy((p_game)->p_messages_filename, p_messages_filename_, __MAX_STR_LEN);\
  memcpy((p_game)->p_obj_bitmap, p_obj_bitmap_, (nr_objs_)*(ADVG_OBJECT_HEIGHT+2)); \
  /* Initialize the scene */                                                  \
  adv_init_scene(&(p_game)->adv_scene, (p_game)->scene_buf, ADVG_SCENE_WIDTH, ADVG_SCENE_HEIGHT, nr_tiles_, p_scene_objs_); \
}
#else
void advg_init_game(advg_game_t *p_game, uint8_t nr_tiles_,
		    uint8_t world_w_, uint8_t world_h_,
		    uint8_t *p_scene_objs_,
		    uint8_t *p_player_mask_, uint8_t *p_player_bitmap_,
		    uint8_t nr_objs_, uint8_t *p_obj_bitmap_,
		    char *p_scene_filename_, char *p_messages_filename_,
		    void *p_user_);
#endif

/**
 * The main game loop. This function implements the main loop of the
 * game and should only be called once. It returns when the game is over.
 * You have to call advg_init_game() before this function.
 *
 * @param p_game the game structure to play with.
 *
 * @see advg_init_game()
 */
extern void advg_do_game(advg_game_t *p_game);

#endif /* ADVGAME_H */
