/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      game.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Game structs
 *
 * $Id: game.h 2678 2005-06-01 17:24:20Z ska $
 *
 ********************************************************************/
#ifndef __GAME_H__
#define __GAME_H__

#include <vmgp.h>
#include <vmgputil.h>

#include <utils.h>
#include <types.h>
#include <point.h>
#include <rect.h>
#include <dir.h>

#include "res.h"

#define LEVEL_MASK_EMPTY               0
#define LEVEL_MASK_PLATFORM            1
#define LEVEL_MASK_CLIMB_NET           2
#define LEVEL_MASK_CLIMB_NET_MOVE_UP   3
#define LEVEL_MASK_CLIMB_NET_MOVE_DOWN 4
#define LEVEL_MASK_CLIMB_STAIRS        5


#define FRAME_BRUCE_IDLE            0
#define FRAME_BRUCE_HIT            (FRAME_BRUCE_IDLE+1)
#define FRAME_BRUCE_CROUCH         (FRAME_BRUCE_HIT+1)
#define FRAME_BRUCE_RUN            (FRAME_BRUCE_CROUCH+1)
#define FRAME_BRUCE_RUN_JUMP       (FRAME_BRUCE_RUN+2)
#define FRAME_BRUCE_KICK           (FRAME_BRUCE_RUN_JUMP+2)
#define FRAME_BRUCE_JUMP           (FRAME_BRUCE_KICK+2)
#define FRAME_BRUCE_FALL           (FRAME_BRUCE_JUMP+1)  /* Same as jump */
#define FRAME_BRUCE_CLIMB          (FRAME_BRUCE_FALL+1)

#define FRAME_BRUCE_FIRST_RIGHT    (FRAME_BRUCE_CLIMB+2)

#define N_FRAMES                   (SPRITE_FRAMES_HEIGHT / 24)

#define LEVEL_WIDTH                (320)
#define N_LAMPS                      10

typedef enum
{
  SPRITE_IDLE     = 0,
  SPRITE_CROUCH   = 1,
  SPRITE_RUN      = 2,
  SPRITE_HIT      = 3,
  SPRITE_KICK     = 4,
  SPRITE_JUMP     = 5,
  SPRITE_JUMP_RUN = 6,
  SPRITE_KICKED   = 7,
  SPRITE_FALL     = 8,
  SPRITE_CLIMB    = 9,
} sprite_state_t;

typedef enum
{
  EMPTY           = 0,
  HARD            = 1,
  VINES           = 2,
} tile_type_t;

#define SPRITE_N_STATES (SPRITE_CLIMB+1)

typedef struct
{
  point_t pt_offset;
  uint8_t anim_frame;
  uint8_t n;
} jt_entry_t;

typedef struct
{
  int16_t n_entries;
  jt_entry_t entries[];
} anim_table_t;

typedef struct
{
  point_t   pt;
  dir_t     dir;
  uint8_t   frame;
  rect_t   *p_bounds;

  sprite_state_t state;
  anim_table_t  **p_anims;
  int            move_table_idx;
  int            table_n;
  int       offset_right; /* The first frame to the right */
} sprite_t;

typedef struct
{
  point_t  pt;
  uint8_t  id;
} lamp_store_t;

typedef struct
{
  sprite_t      sprite;
  int               id;
} lamp_t;

typedef struct
{
  sprite_t      sprite;
} player_t;

typedef struct s_level
{
  int16_t w;
  int16_t h;
  uint8_t *p_level_data;
  level_mask_t *p_level_mask;
  struct s_level *p_adjacent_screens[4];

  /* Spawn points */
  point_t *bruce_spawn_points;

  uint32_t     *p_lamp_mask;
  lamp_store_t *p_lamps;
  int           n_lamps;
} level_t;

typedef struct
{
  MAP_HEADER   bgmap;
  SPRITE     **pp_sprites;
  player_t     player;
  level_t     *p_cur_level;
  uint8_t     *p_tiles;
  uint8_t     *p_midi_data;
  rect_t      *p_cur_rects;
  SPRITE      *p_title_screen;
  int16_t      bg_x;
  int16_t      bg_y;
  lamp_t       lamps[N_LAMPS];
} game_t;


extern level_t levels[];
extern anim_table_t *bruce_anims[SPRITE_N_STATES];
extern rect_t bruce_bounding[];
extern rect_t tile_bounds[];
extern tile_type_t tile_type_map[];
void level_goto(game_t *p_game, level_t *p_level, point_t p);
void sprite_set_state(sprite_t *p_sprite, sprite_state_t state);


#endif /* !__GAME_H__ */
