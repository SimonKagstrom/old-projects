/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      playfield.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Playfield defs
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __PLAYFIELD_H__
#define __PLAYFIELD_H__

#include <sysdeps/sysdeps.h>
#include <point.h>

#define PF_W 9
#define PF_H 9

/* All pieces - light and dark */
#define N_PIECES   (18 * 2)

#define NOK    0
#define OK     1
#define BATTLE 2

/* FIXME: Maybe combine this together with the pieces Pieces can be
 * identified by number + color (index into array)
 */
#define PF_DARK    0
#define PF_LIGHT   1
#define PF_MAGENTA 2

/* Light side */
#define CR_VALKYIRE  0
#define CR_GOLEM     1
#define CR_UNICORN   2
#define CR_DINJI     3
#define CR_WIZARD    4
#define CR_PHOENIX   5
#define CR_ARCHER    6
#define CR_KNIGHT    7

/* Dark side */
#define CR_BANSHEE      0
#define CR_TROLL        1
#define CR_BASILISK     2
#define CR_SHAPESHIFTER 3
#define CR_SORCERESS    4
#define CR_DRAGON       5
#define CR_MANTICORE    6
#define CR_GOBLIN       7

typedef int move_res_t;

typedef struct
{
  uint8_t power;
  uint8_t movement;
  uint8_t type;
  uint8_t dark;
  uint8_t flying;
  point_t pos;
} creature_t;

typedef struct
{
  creature_t *piece; /* NULL if no */
  uint8_t     type;  /* Light/dark */
} tile_t;

typedef struct
{
  tile_t field[ PF_W * PF_H ];
  creature_t pieces[N_PIECES]; /* both light and dark */

  /* private */
  int n;
} playfield_t;

extern void playfield_init(playfield_t *pf);

/**
 * Return if a move from (sx,sy) -> (dx,dy) can be made.
 *
 * @param pf the playfield
 * @param out how far it actually got (for battling)
 * @param start the start position to move from
 * @param dst the position to move to
 *
 * @return OK if the move is possible, BATTLE if the move ends up on
 * an enemy tile, NOK if the move is not possible
 */
extern move_res_t playfield_can_move(playfield_t *pf, point_t *out,
				     point_t start, point_t dst);

extern void playfield_do_move(playfield_t *pf,
			      point_t start, point_t end);

extern void playfield_remove_creature(playfield_t *pf,
				      point_t where);

extern void playfield_set_creature(playfield_t *pf, creature_t *cr,
				   point_t where);

extern creature_t *playfield_get_creature(playfield_t *pf,
					  point_t where);

#define playfield_get_tile(pf, pt) ((pf)->field[ (pt.y) * PF_W + (pt.x)])

extern int playfield_is_powerpoint(point_t where);

extern void creature_init(creature_t *cr, int dark,
			  uint8_t type);

#endif /* !__PLAYFIELD_H__ */
