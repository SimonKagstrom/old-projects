/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      point.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Points and operations
 *
 * $Id: point.h 598 2004-09-11 16:40:19Z ska $
 *
 ********************************************************************/
#ifndef __POINT_H__
#define __POINT_H__

#ifndef TARGET_ANSIC
# include <vmgp.h>
#endif

#include "dir.h"
#include "types.h"
#include "utils.h"

#define TILE_W   8
#define TILE_H   8

typedef struct
{
  int16_t x;
  int16_t y;
} point_t;

static inline point_t pt(int16_t x, int16_t y)
{
  point_t out;

  out.x = x;
  out.y = y;

  return out;
}

static inline point_t pt_to_sprite(point_t p)
{
  point_t out;

  out.x = p.x * TILE_W;
  out.y = p.y * TILE_H;

  return out;
}

static inline point_t pt_to_tile(point_t pt)
{
  point_t out;

  out.x = pt.x / TILE_W;
  out.y = pt.y / TILE_H;

  return out;
}

static inline point_t pt_add(point_t a, point_t b)
{
  point_t out;

  out.x = a.x + b.x;
  out.y = a.y + b.y;

  return out;
}

static inline point_t pt_flip(const point_t a)
{
  point_t out;

  out = a;
  out.x = -a.x;

  return out;
}

static inline point_t pt_flop(const point_t a)
{
  point_t out;

  out = a;
  out.y = -a.y;

  return out;
}

static inline point_t pt_flip_flop(const point_t a)
{
  point_t out;

  out.x = -a.x;
  out.y = -a.y;

  return out;
}

static inline bool_t pt_eq(point_t a, point_t b)
{
  return (a.x == b.x &&
	  a.y == b.y);
}

static inline point_t pt_add_dir(point_t a, dir_t dir)
{
  point_t out;

  out.x = a.x + dir_dx(dir)*4; /* UGLY!! */
  out.y = a.y + dir_dy(dir)*4;

  return out;
}

#endif /* !__POINT_H__ */
