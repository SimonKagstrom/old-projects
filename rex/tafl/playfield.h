/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      playfield.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Playfield defs
 *
 * $Id: playfield.h 8080 2006-05-10 20:05:40Z ska $
 *
 ********************************************************************/
#ifndef __PLAYFIELD_H__
#define __PLAYFIELD_H__

#define PF_BLACK      0
#define PF_WHITE      1
#define PF_WHITE_KING 2
#define PF_EMPTY      3
#define PF_INVALID    4 /* Not really needed in playfield_t::field */

#define PF_WIDTH      11
#define PF_HEIGHT     11

#define PF_WIDTH_PWR_TWO 16

#define PF_INF     30000 /* Infinity in 16 bits! */

#define LEFT           0
#define RIGHT          1
#define UP             2
#define DOWN           3
#define UP_LEFT        4
#define UP_RIGHT       5
#define DOWN_LEFT      6
#define DOWN_RIGHT     7

#define IS_PIECE(x)    ( (x) != PF_EMPTY && (x) != PF_INVALID )

#define N_NEIGHBORS    8

/* Scores for various things in the game */
#define SCORE_WIN      (PF_INF/2)

typedef uint8_t item_t;
typedef uint8_t dir_t;

typedef struct
{
  item_t field[PF_WIDTH_PWR_TWO * PF_HEIGHT];
} playfield_t;

typedef struct
{
  int sx, sy;
  int dx, dy;
} move_t;

struct s_game;

/* Global variables */
extern playfield_t playfield_initial;

/* Function prototypes */
extern bool_t IS_THRONE(uint8_t x, uint8_t y);
extern bool_t IS_CORNER(uint8_t x, uint8_t y);
extern item_t COLOR(item_t item);
extern int dir_to_dx(dir_t dir);
extern int dir_to_dy(dir_t dir);
extern dir_t dx_dy_to_dir(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy);
extern bool_t playfield_item_is_valid(playfield_t *p_playfield, uint8_t x, uint8_t y);
extern item_t playfield_get_item(playfield_t *p_playfield, uint8_t x, uint8_t y);
extern void playfield_set_item(playfield_t *p_playfield, item_t item, uint8_t x, uint8_t y);

extern bool_t playfield_can_move(playfield_t *p_playfield, uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy);
/* Assumes that this has been checked with playfield_can_move */
extern void playfield_do_move(playfield_t *p_playfield, struct s_game *p_game, uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy);

#endif /* !__PLAYFIELD_H__ */
