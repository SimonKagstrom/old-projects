/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      anims.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Animations
 *
 * $Id: anims.c 11414 2006-10-02 10:44:20Z ska $
 *
 ********************************************************************/
#include <game.h>


#define JT_ENTRY(x_in,y_in, n_in, frame_in) (jt_entry_t){ .pt_offset.x = x_in, .pt_offset.y = y_in, \
							  .anim_frame = frame_in, .n = n_in}

static anim_table_t bruce_idle_table =
{
  .entries =
  {
    JT_ENTRY(0, 0, 1, FRAME_BRUCE_IDLE),
  },
  .n_entries = 1,
};

static anim_table_t bruce_fall_table =
{
  .entries =
  {
    JT_ENTRY(0, 4, 1, FRAME_BRUCE_FALL),
  },
  .n_entries = 1,
};

static anim_table_t bruce_crouch_table =
{
  .entries =
  {
    JT_ENTRY(0, 0, 5, FRAME_BRUCE_CROUCH),
  },
  .n_entries = 1,
};

static anim_table_t bruce_kicked_table =
{
  .entries =
  {
    JT_ENTRY(0, 0, 5, FRAME_BRUCE_KICK), /* FIXME! */
  },
  .n_entries = 1,
};

static anim_table_t bruce_climb_table =
{
  .entries =
  {
    JT_ENTRY(4, 4, 1, FRAME_BRUCE_CLIMB),
    JT_ENTRY(0, 0, 2, FRAME_BRUCE_CLIMB),
  },
  .n_entries = 2,
};


static anim_table_t bruce_hit_table =
{
  .entries =
  {
    JT_ENTRY(0, 0, 5, FRAME_BRUCE_HIT),
    JT_ENTRY(0, 0, 5, FRAME_BRUCE_IDLE),
  },
  .n_entries = 2,
};

static anim_table_t bruce_jump_table =
{
  .entries =
  {
    JT_ENTRY(0,  0, 3, FRAME_BRUCE_JUMP),
    JT_ENTRY(0, -4, 4, FRAME_BRUCE_JUMP+1),
  },
  .n_entries = 2,
};

static anim_table_t bruce_run_table =
{
  .entries =
  {
    JT_ENTRY(4, 0, 1, FRAME_BRUCE_RUN),
    JT_ENTRY(4, 0, 1, FRAME_BRUCE_RUN+1),
  },
  .n_entries = 2,
};

static anim_table_t bruce_kick_table =
{
  .entries =
  {
    JT_ENTRY(2, -4, 1, FRAME_BRUCE_KICK),
    JT_ENTRY(2, -2, 2, FRAME_BRUCE_KICK + 1),
    JT_ENTRY(2,  0, 6, FRAME_BRUCE_KICK + 1),
    JT_ENTRY(2,  2, 2, FRAME_BRUCE_KICK + 1),
    JT_ENTRY(2,  4, 1, FRAME_BRUCE_KICK),
  },
  .n_entries = 5
};

static anim_table_t bruce_jump_run_table =
{
  .entries =
  {
    JT_ENTRY(4, -4, 2, FRAME_BRUCE_RUN_JUMP),
    JT_ENTRY(4, -2, 2, FRAME_BRUCE_RUN_JUMP + 1),
    JT_ENTRY(4,  0, 4, FRAME_BRUCE_RUN_JUMP + 1),
    JT_ENTRY(4,  2, 2, FRAME_BRUCE_RUN_JUMP + 1),
    JT_ENTRY(4,  4, 2, FRAME_BRUCE_RUN_JUMP),
  },
  .n_entries = 5,
};

#include "gfx/bruce_bounding.h"

anim_table_t *bruce_anims[SPRITE_N_STATES] =
{
  &bruce_idle_table, /* Idle */
  &bruce_crouch_table,
  &bruce_run_table,
  &bruce_hit_table,
  &bruce_kick_table,
  &bruce_jump_table,
  &bruce_jump_run_table,
  &bruce_kicked_table, /* Kicked */
  &bruce_fall_table, /* Fall */
  &bruce_climb_table,
};
