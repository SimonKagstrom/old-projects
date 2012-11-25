/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      playfield.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Playfield implementation for tafl game
 *
 * $Id: playfield.c 8289 2006-05-26 07:58:55Z ska $
 *
 ********************************************************************/
#include <sysdeps/sysdeps.h>
#include <string.h>

#include "playfield.h"
#include "game.h"
#include "utils.h"

/* Old macros... Maybe fix some day */
bool_t IS_THRONE(uint8_t x, uint8_t y)
{
  return (x == PF_WIDTH/2 && y == PF_HEIGHT/2);
}

bool_t IS_CORNER(uint8_t x, uint8_t y)
{
  return ((x == 0          && y == 0) ||
	  (x == PF_WIDTH-1 && y == 0) ||
	  (x == 0          && y == PF_HEIGHT-1) ||
	  (x == PF_WIDTH-1 && y == PF_HEIGHT-1));
}

item_t COLOR(item_t item)
{
  if (item == PF_WHITE || item == PF_WHITE_KING)
    return PF_WHITE;
  if (item == PF_BLACK)
    return PF_BLACK;

  return PF_INVALID;
}

int dir_to_dx(dir_t dir)
{
  if (dir == LEFT || dir == UP_LEFT || dir == DOWN_LEFT)
    return -1;
  if (dir == RIGHT || dir == UP_RIGHT || dir == DOWN_RIGHT)
    return 1;
  return 0;
}

int dir_to_dy(dir_t dir)
{
  if (dir == UP || dir == UP_LEFT || dir == UP_RIGHT)
    return -1;
  if (dir == DOWN || dir == DOWN_LEFT || dir == DOWN_RIGHT)
    return 1;
  return 0;
}

dir_t dx_dy_to_dir(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy)
{
  if (dx - sx < 0)
    return LEFT;
  else if (dx - sx > 0)
    return RIGHT;
  else if (dy - sy < 0)
    return UP;
  else if (dy - sy > 0)
    return DOWN;

  /* Only get here if sx==sy && dx == dy */
  return -1;
}



bool_t playfield_item_is_valid(playfield_t *p_playfield, uint8_t x, uint8_t y)
{
  return !(x >= PF_WIDTH || y >= PF_HEIGHT);
}

/* These two functions should be optimized since they are called
 * basically all the time
 */
item_t playfield_get_item(playfield_t *p_playfield, uint8_t x, uint8_t y)
{
  if (playfield_item_is_valid(p_playfield, x, y))
    return p_playfield->field[y * PF_WIDTH_PWR_TWO + x];
  return PF_INVALID;
}

void playfield_set_item(playfield_t *p_playfield, item_t item, uint8_t x, uint8_t y)
{
  if (playfield_item_is_valid(p_playfield, x, y))
    p_playfield->field[y * PF_WIDTH_PWR_TWO + x] = item;
}

bool_t playfield_can_move(playfield_t *p_playfield, uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy)
{
  item_t item = playfield_get_item(p_playfield, sx, sy);
  int dir_offs_x, dir_offs_y;
  dir_t dir;

  /* Must be a piece, not empty */
  if (item == PF_EMPTY || item == PF_INVALID)
    return 0;

  /* Move like a rook from the current tile */
  if ( !((sy-dy == 0 && abs(sx - dx) >= 1) ||
	 (sx-dx == 0 && abs(sy - dy) >= 1) ))
    return 0;

  /* Not allowed to step onto the corner tile unless king */
  if ( (item != PF_WHITE_KING && IS_CORNER(dx, dy)) ||
       IS_THRONE(dx, dy))
    return 0;

  dir = dx_dy_to_dir(sx, sy, dx, dy);
  dir_offs_x = dir_to_dx(dir);
  dir_offs_y = dir_to_dy(dir);

  /* Check the path from sx,sy -> dx,dy */
  while (sx != dx || sy != dy)
    {
      item_t next;

      sx = sx + dir_offs_x;
      sy = sy + dir_offs_y;
      next = playfield_get_item(p_playfield, sx, sy);
      if (next != PF_EMPTY)
	return 0;
    }

  return 1;
}

/* Assumes that this has been checked with playfield_can_move */
void playfield_do_move(playfield_t *p_playfield, game_t *p_game, uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy)
{
  item_t src;
  dir_t i;

  /* Do the actual move */
  src = playfield_get_item(p_playfield, sx, sy);
  playfield_set_item(p_playfield, src, dx, dy);
  playfield_set_item(p_playfield, PF_EMPTY, sx, sy);

  if (p_game)
    {
      game_draw_one_pos(p_game, sx, sy, 0);
      game_draw_one_pos(p_game, dx, dy, 0);
    }
  /* Check if some neighbor should be removed */
  for (i = 0; i < N_NEIGHBORS/2; i++)
    {
      int dir_dx = dir_to_dx(i);
      int dir_dy = dir_to_dy(i);
      uint8_t n_x = dx + dir_dx;
      uint8_t n_y = dy + dir_dy;
      uint8_t opp_x = dx + dir_dx * 2 ; /* n_x + dir_to_dx(i) */
      uint8_t opp_y = dy + dir_dy * 2;
      item_t neighbor = playfield_get_item(p_playfield, n_x, n_y);
      item_t neighbor_opposite = playfield_get_item(p_playfield, opp_x, opp_y);

      /* Remove neighbor piece if the far pos is a piece and of the same color as this */
      if (IS_PIECE(neighbor) &&
	  COLOR(neighbor) != COLOR(src) &&
	  (COLOR(neighbor_opposite) == COLOR(src) ||
	   IS_CORNER(opp_x, opp_y) ||
	   (IS_THRONE(opp_x, opp_y) && neighbor_opposite == PF_EMPTY)))
	{
	  playfield_set_item(p_playfield, PF_EMPTY, n_x, n_y);
	  if (p_game)
	    {
	      game_add_captured_piece(p_game, neighbor);
	      game_draw_one_pos(p_game, n_x, n_y, 0);
	    }
	}
    }
}

#define W PF_WHITE
#define K PF_WHITE_KING
#define B PF_BLACK
#define e PF_EMPTY
#define i PF_INVALID

playfield_t playfield_initial =
{
  {
    e,e,e,B,B,B,B,B,e,e,e, i,i,i,i,i,
    e,e,e,e,e,B,e,e,e,e,e, i,i,i,i,i,
    e,e,e,e,e,e,e,e,e,e,e, i,i,i,i,i,
    B,e,e,e,e,W,e,e,e,e,B, i,i,i,i,i,
    B,e,e,e,W,W,W,e,e,e,B, i,i,i,i,i,
    B,B,e,W,W,K,W,W,e,B,B, i,i,i,i,i,
    B,e,e,e,W,W,W,e,e,e,B, i,i,i,i,i,
    B,e,e,e,e,W,e,e,e,e,B, i,i,i,i,i,
    e,e,e,e,e,e,e,e,e,e,e, i,i,i,i,i,
    e,e,e,e,e,B,e,e,e,e,e, i,i,i,i,i,
    e,e,e,B,B,B,B,B,e,e,e, i,i,i,i,i,
  },
};
#undef  W
#undef  K
#undef  B
#undef  e
#undef  i
