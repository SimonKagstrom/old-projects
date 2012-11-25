/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      sprite.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:
 *
 * $Id: sprite.c 331 2004-08-05 06:21:45Z ska $
 *
 ********************************************************************/
#include <vmgp.h>
#include <vmgputil.h> /* WaitTicks */

#include <boulder.h>


/* Initialize a sprite */
void sprite_init(sprite_t *p_sprite, point_t p, uint8_t base_frame)
{
  p_sprite->pt = p;
  p_sprite->base_frame = base_frame;
  p_sprite->frame = 0;
}

void sprite_move(sprite_t *p_sprite, dir_t dir)
{
  p_sprite->pt.x += get_dx(dir)*TILE_W;
  p_sprite->pt.y += get_dy(dir)*TILE_H;
}
