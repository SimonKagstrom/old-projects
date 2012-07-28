/*********************************************************************
 *
 * Copyright (C) 2001-2003,  Simon Kagstrom <ska@bth.se>
 *
 * Filename:      frontend.h
 * Description:   This file contains the defined functions that
 *                each frontend (drawing, keyboard input etc)
 *                should implement.
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
 * $Id: draw.h,v 1.1 2003/02/09 08:14:49 ska Exp $
 *
 ********************************************************************/
#ifndef DRAW_H
#define DRAW_H

#include <sysdeps/typedefs.h>
#include "arkanoid.h"

/* Drawing functions */

/* Draw the game border */
extern void draw_screen(game_t *p_game);
/* Draw the number of lives left */
extern void draw_lives(game_t *p_game);
/* Draw the block p_block at x, y */
extern void draw_block(block_t *p_block, uint8_t x, uint8_t y);
/* Draw the paddle p_paddle */
extern void draw_paddle(paddle_t *p_paddle);
/* Draw the ball p_ball */
extern void draw_ball(ball_t *p_ball);
/* Draw the special p_special */
extern void draw_special(special_t *p_special);

#endif /* !DRAW_H */
