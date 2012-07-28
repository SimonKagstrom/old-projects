/*********************************************************************
 *
 * Copyright (C) 2001-2003,  Simon Kagstrom <ska@bth.se>
 *
 * Filename:      draw.c
 * Description:   This file contains the implementation of the
 *                drawing functions for the REX game.
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
 * $Id: draw.c,v 1.2 2003/02/09 10:45:20 ska Exp $
 *
 ********************************************************************/
#include <stdlib.h>         /* abs, rand */
#include <string.h>         /* strcpy */

#include <sysdeps/sysdeps.h>
#include "draw.h"
#include "bitmaps.h"

#if defined(REX_EMULATOR)
#include "levels.h"
#endif

void draw_lives(game_t *p_game)
{
  int i;

  /* Display the number of lives left */
  for (i=0; i<p_game->lives; i++)
    fe_draw_bitmap(p_ball_bitmap, SCREEN_WIDTH+10+i*6, SCREEN_HEIGHT-PADDLE_HEIGHT-10, 0);
  fe_clear_area(SCREEN_WIDTH+10+i*6, SCREEN_HEIGHT-PADDLE_HEIGHT-10, FE_PHYS_WIDTH-(SCREEN_WIDTH+7+i*6), 5);
}

void draw_screen(game_t *p_game)
{
  /* Clear the screen, draw the number of lives left */
  fe_fill_area(SCREEN_WIDTH+4, 0, 2, SCREEN_HEIGHT-PADDLE_HEIGHT);
  draw_lives(p_game);
}


void draw_block(block_t *p_block, uint8_t ix, uint8_t iy)
{
  sint16_t y = iy*BLOCK_HEIGHT;
  sint16_t x = ix*BLOCK_WIDTH;

  switch(p_block->type & (0xff ^ TYPE_SPECIAL))
    {
    case TYPE_SIMPLE:
      fe_draw_bitmap(p_block_normal_bitmap, x, y, 0);
      break;
    case TYPE_HARD:
      fe_draw_bitmap(p_block_hard_bitmap, x, y, 0);
      break;
    case TYPE_IMPOSSIBLE:
      fe_draw_bitmap(p_block_impossible_bitmap, x, y, 0);
      break;
    default:
      /* TYPE_EMPTY */
      fe_clear_area(x, y, BLOCK_WIDTH, BLOCK_HEIGHT);
      break;
    }
}

void draw_paddle(paddle_t *p_paddle)
{
  uint8_t *p_bitmap;

  fe_clear_area(p_paddle->lastx, PADDLE_Y, PADDLE_LONG_WIDTH, PADDLE_HEIGHT);

  switch(p_paddle->width)
    {
    case PADDLE_LONG_WIDTH:
      p_bitmap = p_paddle_long_bitmap;
      break;
    case PADDLE_SHORT_WIDTH:
      p_bitmap = p_paddle_short_bitmap;
      break;
    default:
      p_bitmap = p_paddle_normal_bitmap;
      break;
    }
  fe_draw_bitmap(p_bitmap, p_paddle->x, PADDLE_Y, 0);
}

void draw_ball(ball_t *p_ball)
{
  /* Always draw the ball to get comparable speed */
  if (!(p_ball->state & BALL_STATE_ACTIVE))
    {
      fe_draw_bitmap(p_ball_bitmap, SCREEN_WIDTH+10, 0, 0);
      fe_clear_area(SCREEN_WIDTH+10, 0, BALL_WIDTH, BALL_HEIGHT);
      return;
    }

  fe_clear_area(p_ball->lastx, p_ball->lasty, BALL_WIDTH, BALL_HEIGHT);
  fe_draw_bitmap(p_ball_bitmap, p_ball->x, p_ball->y, 0);
}

void draw_special(special_t *p_special)
{
  /* Always draw the special to get comparable speed */
  if (!p_special->type)
    {
      fe_draw_bitmap(p_special_bitmap, SCREEN_WIDTH+10, 0, 0);
      fe_clear_area(SCREEN_WIDTH+10, 0, SPECIAL_WIDTH, SPECIAL_HEIGHT);
      return;
    }

  fe_clear_area(p_special->x, p_special->lasty, SPECIAL_WIDTH, SPECIAL_HEIGHT);
  fe_draw_bitmap(p_special_bitmap, p_special->x, p_special->y, 0);
}
