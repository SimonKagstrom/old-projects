/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      rect.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Rectangle stuff
 *
 * $Id: rect.h 648 2004-09-19 19:12:55Z ska $
 *
 ********************************************************************/
#include <types.h>
#include <point.h>

typedef struct
{
  point_t top_left;
  point_t bottom_right;
} rect_t;


static inline bool_t rect_overlap(rect_t a, rect_t b, point_t *p_overlap)
{
#if 0
  DbgPrintf("Overlap (%d:%d, %d:%d), (%d:%d, %d:%d)?\n", a.top_left.x, a.top_left.y, a.bottom_right.x, a.bottom_right.y,
	    b.top_left.x, b.top_left.y, b.bottom_right.x, b.bottom_right.y);
#endif

  if (a.bottom_right.x < b.top_left.x) return FALSE;
  if (a.top_left.x > b.bottom_right.x) return FALSE;
  if (a.bottom_right.y < b.top_left.y) return FALSE;
  if (a.top_left.y > b.bottom_right.y) return FALSE;

  p_overlap->y = b.bottom_right.y - a.top_left.y;
  p_overlap->x = a.top_left.y - b.bottom_right.y;

  return TRUE;
}

static inline rect_t rect_add(rect_t a, rect_t b)
{
  rect_t out;

  out.top_left = pt_add(a.top_left, b.top_left);
  out.bottom_right = pt_add(a.bottom_right, b.bottom_right);

  return out;
}

static inline rect_t rect_add_pt(rect_t a, point_t p)
{
  rect_t out;

  out.top_left = pt_add(a.top_left, p);
  out.bottom_right = pt_add(a.bottom_right, p);

  return out;
}
