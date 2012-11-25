/*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      advg_client_lib.c
 * Description:   Client-library for libadvg.
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
 * $Id: advg_client_lib.c 6264 2006-01-21 09:46:06Z ska $
 *
 ********************************************************************/
#include <rex/rex.h>

#define ADVG_LIB

#include "sysdeps.h"                  /* fe_?    */
#include "advlib/advlib.h"            /* adv_?   */
#include "advglib/advgame.h"          /* advg_?  */
#include "advg_private.h"             /* _advg_? */
#include "advg_rex_lib.h"             /* ADV_INIT_SCENE etc */

#define FARCALL_OFFSET 0x8006

static int advg_page; /* The page of advglib. */

/* FIXME: The functions here can be #define's instead (save space...) */

/* Called to find the library. This _has_ to be called before anything else is done. */
static void setup_lib(void)
{
  if ( (advg_page = FindLibrary(ADVG_SIG)) == -1)
    {
      /* Cannot find the library! */
      fe_print_xy(0, 0, 0, "Could not find libadvg ("ADVG_SIG")\n");
      fe_sleep(100);
      fe_finalize();
    }
}


/* Functions in libadv */
void adv_init_scene(adv_scene_t *p_scene, uint8_t *p_buf, sint16_t i_w, sint16_t i_h,
		    uint8_t i_nr_tiles, uint8_t *p_tile_buf)
{
  setup_lib();
  farcall(p_scene, p_buf, i_w, i_h, i_nr_tiles, p_tile_buf, ADV_INIT_SCENE, FARCALL_OFFSET, advg_page);
}

void adv_load_scene(adv_scene_t *p_scene, char *p_filename, uint8_t i_scene)
{
  farcall(p_scene, p_filename, i_scene, ADV_LOAD_SCENE, FARCALL_OFFSET, advg_page);
}

void adv_mask_object(adv_scene_t *p_scene, uint8_t *p_dst, uint8_t *p_mask, uint8_t *p_obj,
		     sint16_t i_x, sint16_t i_y)
{
  farcall(p_scene, p_dst, p_mask, p_obj, i_x, i_y, ADV_MASK_OBJECT, FARCALL_OFFSET, advg_page);
}

void adv_mask_background(adv_scene_t *p_scene, uint8_t *p_dst, sint16_t i_x, sint16_t i_y,
			 uint8_t i_w, uint8_t i_h)
{
  farcall(p_scene, p_dst, i_x, i_y, i_w, i_h, ADV_MASK_BACKGROUND, FARCALL_OFFSET, advg_page);
}

void adv_draw_scene(adv_scene_t *p_scene)
{
  farcall(p_scene, ADV_DRAW_SCENE, FARCALL_OFFSET, p_scene);
}

void adv_get_surroundings(adv_scene_t *p_scene, unsigned char *p_dst,
			  sint16_t i_x, sint16_t i_y, uint8_t i_w, uint8_t i_h)
{
  farcall(p_scene, p_dst, i_x, i_y, i_w, i_h, ADV_GET_SURROUNDINGS, FARCALL_OFFSET, advg_page);
}


#if !defined(ADVLIB_ONLY)
/* Functions in libadvg */
void advg_display_message(advg_game_t *p_game, sint16_t i_nr, uint8_t i_n)
{
  farcall(p_game, i_nr, i_n, ADVG_DISPLAY_MESSAGE, FARCALL_OFFSET, advg_page);
}

void advg_draw_objects(advg_game_t *p_game)
{
  farcall(p_game, ADVG_DRAW_OBJECTS, FARCALL_OFFSET, advg_page);
}

void advg_goto_scene(advg_game_t *p_game, sint8_t i_scene, sint16_t i_x, sint16_t i_y)
{
  farcall(p_game, i_scene, i_x, i_y, ADVG_GOTO_SCENE, FARCALL_OFFSET, advg_page);
  advg_handle_event(p_game, ADVG_EVENT_ENTER_SCENE);
}

void advg_draw_screen(void)
{
  farcall(ADVG_DRAW_SCREEN, FARCALL_OFFSET, advg_page);
}


/* Non-public libadvg functions */
advg_event_t _advg_handle_move(advg_game_t *p_game)
{
  return farcall(p_game, _ADVG_HANDLE_MOVE, FARCALL_OFFSET, advg_page);
}

advg_event_t _advg_handle_press(advg_game_t *p_game, fe_point_t *p_point)
{
  return farcall(p_game, p_point, _ADVG_HANDLE_PRESS, FARCALL_OFFSET, advg_page);
}

void _advg_reset_action(advg_game_t *p_game)
{
  farcall(p_game, _ADVG_RESET_ACTION, FARCALL_OFFSET, advg_page);
}
#endif /* ADVLIB_ONLY */
