/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      game.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Game defs
 *
 * $Id: game.h 8020 2006-05-07 08:40:10Z ska $
 *
 ********************************************************************/
#ifndef __GAME_H__
#define __GAME_H__

#include "playfield.h"

typedef struct s_game
{
  playfield_t initial_playfield;
  playfield_t *p_playfield;
  item_t starting_color;
  item_t computer_color;          /* PF_INVALID for two-player games */
  item_t captured_pieces[PF_INVALID];
  int restart;
  int selected_x;
  int selected_y;
} game_t;

extern void game_add_captured_piece(game_t *p_game, item_t item);
extern void game_draw_one_pos(game_t *p_game, uint8_t x, uint8_t y, int inverted);

#endif /* !__GAME_H__ */
