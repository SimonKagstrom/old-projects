/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      game.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Game defs
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __GAME_H__
#define __GAME_H__

#include <playfield.h>

#define SPELL_NONE        0
#define SPELL_TELEPORT    1
#define SPELL_HEAL        2
#define SPELL_SHIFT_TIME  3
#define SPELL_EXCHANGE    4
#define SPELL_ELEMENTAL   5
#define SPELL_REVIVE      6
#define SPELL_IMPRISION   7

#define N_SPELLS 7

typedef struct
{
  playfield_t playfield;
  int         light_cycle;
  int         turn;

  int         spell_used[ N_SPELLS ];
} game_t;

#endif /* !__GAME_H__ */
