/*********************************************************************
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      advg_rex_lib.c
 * Description:   The entry-point for the REX library.
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
 * $Id: advg_rex_lib.c 6264 2006-01-21 09:46:06Z ska $
 *
 *********************************************************************/
#include <rex/rex.h>
#include <rex/library.h>           /* LibMain etc */

#include "sysdeps.h"
#include "advlib/advlib.h"
#include "advglib/advgame.h"
#include "advg_rex_lib.h"
#include "advg_private.h"

static void adv_init_scene_wrap(adv_scene_t *p_scene, uint8_t *p_buf, sint16_t i_w, sint16_t i_h,
				uint8_t i_nr_tiles, uint8_t *p_tile_buf)
{
  adv_init_scene(p_scene, p_buf, i_w, i_h, i_nr_tiles, p_tile_buf);
}

static void adv_load_scene_wrap(adv_scene_t *p_scene, char *p_filename, uint8_t i_scene)
{
  adv_load_scene(p_scene, p_filename, i_scene);
}

static void adv_mask_object_wrap(adv_scene_t *p_scene, uint8_t *p_dst, uint8_t *p_mask, uint8_t *p_obj,
				 sint16_t i_x, sint16_t i_y)
{
  adv_mask_object(p_scene, p_dst, p_mask, p_obj, i_x, i_y);
}

static void adv_mask_background_wrap(adv_scene_t *p_scene, uint8_t *p_dst, sint16_t i_x, sint16_t i_y,
				     uint8_t i_w, uint8_t i_h)
{
  adv_mask_background(p_scene, p_dst, i_x, i_y, i_w, i_h);
}

static void adv_draw_scene_wrap(adv_scene_t *p_scene)
{
  adv_draw_scene(p_scene);
}

static void adv_get_surroundings_wrap(adv_scene_t *p_scene, unsigned char *p_dst,
				      sint16_t i_x, sint16_t i_y, uint8_t i_w, uint8_t i_h)
{
  adv_get_surroundings(p_scene, p_dst, i_x, i_y, i_w, i_h);
}



static void advg_display_message_wrap(advg_game_t *p_game, sint16_t i_nr, uint8_t i_n)
{
  advg_display_message(p_game, i_nr, i_n);
}

static void advg_draw_objects_wrap(advg_game_t *p_game)
{
  advg_draw_objects(p_game);
}

static void advg_goto_scene_wrap(advg_game_t *p_game, sint8_t i_scene, sint16_t i_x, sint16_t i_y)
{
  advg_goto_scene(p_game, i_scene, i_x, i_y);
}

static void advg_draw_screen_wrap(void)
{
  advg_draw_screen();
}

static advg_event_t _advg_handle_move_wrap(advg_game_t *p_game)
{
  return _advg_handle_move(p_game);
}

static advg_event_t _advg_handle_press_wrap(advg_game_t *p_game, fe_point_t *p_point)
{
  return _advg_handle_press(p_game, p_point);
}

static void _advg_reset_action_wrap(advg_game_t *p_game)
{
  _advg_reset_action(p_game);
}


void LibMain()
{
  /* FIXME! Reorder these! */
  switch(getFuncID())
    {
    case ADV_INIT_SCENE:
      adv_init_scene_wrap();
      break;
    case ADV_LOAD_SCENE:
      adv_load_scene_wrap();
      break;
    case ADV_MASK_OBJECT:
      adv_mask_object_wrap();
      break;
    case ADV_MASK_BACKGROUND:
      adv_mask_background_wrap();
      break;
    case ADV_DRAW_SCENE:
      adv_draw_scene_wrap();
      break;
    case ADV_GET_SURROUNDINGS:
      adv_get_surroundings_wrap();
      break;

    case ADVG_DISPLAY_MESSAGE:
      advg_display_message_wrap();
      break;
    case ADVG_DRAW_OBJECTS:
      advg_draw_objects_wrap();
      break;
    case ADVG_GOTO_SCENE:
      advg_goto_scene_wrap();
      break;
    case ADVG_DRAW_SCREEN:
      advg_draw_screen_wrap();
      break;
    case _ADVG_HANDLE_MOVE:
      _advg_handle_move_wrap();
      break;
    case _ADVG_HANDLE_PRESS:
      _advg_handle_press_wrap();
      break;
    case _ADVG_RESET_ACTION:
      _advg_reset_action_wrap();
      break;
    }
}


void main()
{
}

