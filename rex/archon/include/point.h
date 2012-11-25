/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      point.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Point def
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __POINT_H__
#define __POINT_H__

#include <sysdeps/sysdeps.h>
#include <assert.h>

typedef struct
{
  int8_t x;
  int8_t y;
} point_t;

typedef enum
{
  NONE, /* Diagonal */
  UP,
  DOWN,
  LEFT,
  RIGHT,
} dir_t;

static inline point_t point(int x, int y)
{
  point_t out;

  out.x = x;
  out.y = y;

  return out;
}

extern dir_t point_get_dir(point_t a, point_t b);

extern int point_get_dx(point_t a, point_t b);

extern int point_get_dy(point_t a, point_t b);

extern point_t point_add_dx_dy(point_t pt, int dx, int dy);

extern point_t point_add_dir(point_t pt, dir_t dir);

#endif /* !__POINT_H__ */
