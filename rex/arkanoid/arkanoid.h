/*********************************************************************
 *
 * Copyright (C) 2001-2003,  Simon Kagstrom <ska@bth.se>
 *
 * Filename:      arkanoid.h
 * Description:   This file contains the definitions for the
 *                arkanoid game.
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
 * $Id: arkanoid.h,v $
 *
 ********************************************************************/
#ifndef ARKANOID_H
#define ARKANOID_H

#include <sysdeps/typedefs.h>

#define SCREEN_WIDTH      200
#define SCREEN_HEIGHT     120
#define BLOCK_WIDTH       20  /* 20x10 pixel-blocks */
#define BLOCK_HEIGHT      10

/* Playfield */
#define FIELD_WIDTH       (SCREEN_WIDTH/BLOCK_WIDTH)
#define FIELD_HEIGHT      (SCREEN_HEIGHT/BLOCK_HEIGHT+1) /* +1 to have balls etc move off-field more nicely */

#define FLOOR_HEIGHT      3

/* Paddle definitions */
#define PADDLE_HEIGHT        6
#define PADDLE_WIDTH         24
#define PADDLE_LONG_WIDTH    32
#define PADDLE_SHORT_WIDTH   16
#define PADDLE_NORMAL        (uint8_t)0 /* The normal paddle */
#define PADDLE_Y             (SCREEN_HEIGHT-PADDLE_HEIGHT-FLOOR_HEIGHT)

/* Ball definitions */
#define BALL_STATE_DESTROYER  (uint8_t)16 /* When the ball goes through blocks without bouncing */
#define BALL_STATE_ACTIVE     (uint8_t)32
#define BALL_STATE_HOLDING    (uint8_t)64
#define BALL_WIDTH           (sint16_t)4
#define BALL_HEIGHT          (sint16_t)4
#define MAX_BALLS             (uint8_t)2

#define TYPE_EMPTY           0
#define TYPE_SIMPLE          1 /* One hit - gone */
#define TYPE_HARD            2 /* 2 hits needed */
#define TYPE_IMPOSSIBLE      3 /* Cannot (almost) be destroyed */
#define TYPE_SPECIAL         4 /* Something will fall down (or'ed with the other values) */

#define SPECIAL_NONE         (uint8_t)0
#define SPECIAL_EXTRA_BALL   (uint8_t)1   /* Two balls */
#define SPECIAL_LONG_PADDLE  (uint8_t)2   /* Long paddle */
#define SPECIAL_STICKY       (uint8_t)4   /* Paddle is sticky four times */
#define SPECIAL_FLOOR        (uint8_t)8   /* A floor saves the ball four times */
#define SPECIAL_DESTROYER    (uint8_t)16  /* The ball passes through 1-hit blocks */
#define SPECIAL_EXTRA_LIFE   (uint8_t)32  /* Extra life! */
#define SPECIAL_SHORT_PADDLE (uint8_t)64  /* Short paddle */
#define SPECIAL_GUN          (uint8_t)128 /* Not used yet */
#define SPECIAL_MAX          (uint8_t)7   /* The number of different specials */

#define SPECIAL_WIDTH       (sint16_t)6
#define SPECIAL_HEIGHT      (sint16_t)8

#define MAX_SPECIALS         (uint8_t)3 /* The maximum number of concurrent specials */

#define NR_LIVES             5
#define MAX_LIVES            8

#if defined(TARGET_REX)
#define DEBUG(x)
#define SLEEP_PERIOD         1    /* The REX 6000 is fairly slow ;-) */
#define LEVEL_SLEEP_PERIOD   (1000/64)
#else
#define DEBUG(x) x                /* Use debug printouts for SDL*/
#define SLEEP_PERIOD         1
#define LEVEL_SLEEP_PERIOD   (1000/64)
#endif /* TARGET_REX */


#define BALL_TYPE_NORMAL     0
#define BALL_TYPE_FAST       1
#define BALL_TYPE_ONE_WAY    2


/* The struct for the player paddle */
typedef struct
{
  uint8_t type;       /* Long, short etc */
  uint8_t width;
  sint16_t x;
  sint16_t lastx;
} paddle_t;

/* The struct for the ball */
typedef struct
{
  uint16_t state;     /* Destroyer etc */
  sint16_t dx;        /* Direction the ball is travelling in */
  sint16_t dy;
  sint16_t x;
  sint16_t y;
  sint16_t lastx;
  sint16_t lasty;
  sint16_t swap;
  sint16_t type;
  sint16_t step;
} ball_t;

/* The struct for the blocks */
typedef struct
{
  uint8_t type;        /* Type of block */
  uint8_t hits;        /* Hits left before blowup */
  uint8_t special;     /* What type of special this contains */
} block_t;

/* The structure for specials */
typedef struct
{
  uint8_t type;        /* Type of special */
  uint8_t x;
  uint8_t y;
  uint8_t lasty;       /* Only y is needed - never changes its X-pos */
} special_t;

/* The struct for the actual game */
typedef struct
{
  block_t   p_field[FIELD_WIDTH*FIELD_HEIGHT];     /* The playfield */
  ball_t    p_balls[MAX_BALLS];                    /* The ball(s) */
  special_t p_specials[MAX_SPECIALS];              /* The current specials */
  paddle_t  paddle;                                /* The player paddle */
  char      p_levels[FIELD_WIDTH*FIELD_HEIGHT+40]; /* A pointer to the levels */
  uint8_t   levels;                              /* The number of levels */
  uint8_t   state;                               /* Floor on or off */
  uint8_t   nr_balls;                            /* The current number of balls */
  uint8_t   nr_blocks;                           /* The number of blocks on this level */
  uint8_t   nr_specials;                         /* The current number of specials */
  sint8_t   free_ball;                           /* These two specify if there is */
  sint8_t   free_special;                        /* a free ball/special slot. */
  uint8_t   level;                               /* The current level */
  uint8_t   lives;                               /* The number of lives the player has */
} game_t;

#endif /* !ARKANOID_H */
