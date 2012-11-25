/*********************************************************************
 *
 * Copyright (C) 2003, Simon Kagstrom <ska@bth.se>
 *
 * Filename:      creator.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Creator-related functions.
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
#define CHK(x) do { if (!(x))                              \
                 {                                         \
		   fprintf(stderr, "%s failed!\n",##x);    \
		   exit(1);                                \
		 } } while(0)

#define MAX_EVENTS 5

void scene_event_init(creator_t *p_c, scene_event_t *p_ev, int x, int y, int w, int h, int num)
{
  p_ev->x = x;
  p_ev->y = y;
  p_ev->w = w;
  p_ev->h = h;
  p_ev->num = num;
}

void scene_init(creator_t *p_c, scene_t *p_sc, int w, int h)
{
  CHK(p_sc->p_level_buf = malloc(w*h));

  memset(p_sc->p_level_buf, p_c->default_block, w*h);
}

void scene_finalize(creator_t *p_c, scene_t *p_sc)
{
  free (p_sc->p_level_buf);
}

#define DEFAULT_W 16
#define DEFAULT_H 12
void game_init(creator_t *p_c, game_t *p_g, int w, int h)
{
  int i;

  CHK(p_g->p_scenes = malloc(sizeof(scene_t) * (w*h)) );

  for (i=0; i<w*h; i++)
    scene_init(p_c, p_g->p_scenes[i], DEFAULT_W, DEFAULT_H);
}

void creator_init(creator_t *p_c)
{
  p_c->default_block = 'A';
  p_c->x = 0;
  p_c->y = 0;
  p_c->cur_block = 0;
  p_c->cur_scene = 0;

  CHK(p_c->pp_block_imgs = malloc(sizeof(SDL_Surface*)*('z'-'A')));
  memset(p_c->pp_block_imgs, 0, sizeof(SDL_Surface*)*('z'-'A'));
}
