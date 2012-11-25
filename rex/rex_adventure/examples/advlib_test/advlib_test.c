/*********************************************************************
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      advlib_test.c
 * Description:   A test program for advlib.
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
 * $Id: advlib_test.c 6264 2006-01-21 09:46:06Z ska $
 *
 *********************************************************************/
#include <string.h>       /* memset */

#include "typedefs.h"
#include "sysdeps.h"
#include "advlib/advlib.h"

#include "../example_game/bitmaps.h"

#define SCENE_WIDTH  13
#define SCENE_HEIGHT  6

int main()
{
  uint8_t buf[SCENE_WIDTH*SCENE_HEIGHT];
  uint8_t tile_buf[NR_TILES*2*16 + NR_TILES*2];
  uint8_t obj_buf[2+8];
  adv_scene_t scene;
  char name[40];

  /* Initialise the system dependent things */
  fe_init();

  /* The tiles and the filename _need_ to be on the stack */
  memcpy(tile_buf, p_background_data, NR_TILES*2*16 + NR_TILES*2);
  strcpy(name, "test_scene");

  /* Initialise and load the scene */
  adv_init_scene(&scene, buf, SCENE_WIDTH, SCENE_HEIGHT, NR_TILES, tile_buf);
  adv_load_scene(&scene, name, 0);

  /* Draw the scene */
  adv_draw_scene(&scene);

  /* Mask an object against the background */
  adv_mask_object(&scene, obj_buf, p_player_mask_bitmap, p_player_bitmap, 27, 20);

  /* Draw the object */
  fe_draw_bitmap(obj_buf, 27, 20, 0);

  /* Wait for keypress */
  while (!fe_get_buttons())
    ;

  fe_finalize();

  return 0;
}
