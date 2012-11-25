/*********************************************************************
 *
 * Copyright (C) 2003, Simon Kagstrom <ska@bth.se>
 *
 * Filename:      sdl_ui.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   User interface for the creator.
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
 * $Id:$
 *
 ********************************************************************/
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

static char ui_ask(creator_t *p_c, char *p_prompt, char *p_opts)
{
  if (!p_opts)
    ; /* FIXME: Return any keypress */

  return 'y';
}

static int ui_ask_yesno(creator_t *p_c, char *p_prompt)
{
  switch (ui_ask(p_c, p_prompt, "ynYN"))
    {
    case 'y':
    case 'Y':
      return 1;
    case 'n':
    case 'N':
    default:
      return 0;
    }
}

static void ui_move_ptr(creator_t *p_c, int dir)
{
  /* Move pointer somewhere. Also switch scene if needed */

  switch(dir)
    {
    case 0: /* up */
      break;
    case 1: /* down */
      break;
    case 2: /* left */
      break;
    case 3: /* right */
      break;
    default:
      break;
    }
}


void ui_add_block(creator_t *p_c)
{
  /* Browse images, select from them */
}

void ui_move_block(creator_t *p_c)
{
  char cur_block;
  int i;

  cur_block = p_c->cur_block+'A';
  /* Move a block down/up, stop when enter is pressed */

  /* Switch the two for every scene, redraw */
}

void ui_next_block(creator_t *p_c)
{
  /* Select next block */
}

void ui_prev_block(creator_t *p_c)
{
  /* Select previous block */
}



void ui_add_object(creator_t *p_c)
{
  /* Browse images, select from them */
}

void ui_move_object(creator_t *p_c)
{
  /* Move a object stop when enter is pressed */

  /* Switch the two for every scene, redraw */
}

void ui_next_object(creator_t *p_c)
{
  /* Select next object */
}

void ui_prev_object(creator_t *p_c)
{
  /* Select previous object */
}



void ui_up(creator_t *p_c)
{
  /* Move pointer up */
  ui_move_ptr(p_c, 0);
}

void ui_down(creator_t *p_c)
{
  /* Move pointer down */
  ui_move_ptr(p_c, 1);
}

void ui_left(creator_t *p_c)
{
  /* Move pointer left */
  ui_move_ptr(p_c, 2);
}

void ui_right(creator_t *p_c)
{
  /* Move pointer right */
  ui_move_ptr(p_c, 3);
}



void ui_draw_scene(creator_t *p_c)
{
  scene_t *p_scene = &p_c->p_game->p_scenes[p_c->cur_scene];

  /* Draw p_scene */
  SDL_Flip(p_c->p_screen);
}

void ui_clear_scene(creator_t *p_c)
{
  /* Ask: really clear scene?
   *
   */
  if (ui_ask_yesno(p_c, "Really clear scene?"))
    ; /* Clear scene */
}



void ui_set_default(creator_t *p_c)
{
  char def;

  def = ui_ask(p_c, "Select default block ('A'..'z')", NULL);

  if (def >= 'A' && def <= 'z')
    p_c->default_block = def;
}



void ui_add_event(creator_t *p_c)
{
  /* Add an event-point, give it a number */
}

void ui_move_event(creator_t *p_c)
{
}

void ui_resize_event(creator_t *p_c, int dx, int dy)
{
}



void ui_init(creator_t *p_c)
{
  /* Create a window */
}
