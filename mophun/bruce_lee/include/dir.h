/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      dir.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Direction + handling
 *
 * $Id: dir.h 598 2004-09-11 16:40:19Z ska $
 *
 ********************************************************************/
#ifndef __DIR_H__
#define __DIR_H__

#ifndef TARGET_ANSIC
# include <vmgp.h>
#endif

typedef enum
{
  UP    = 0,
  DOWN  = 1,
  LEFT  = 2,
  RIGHT = 3,
  IDLE  = 4,
} dir_t;

static inline int16_t dir_dx(const dir_t dir)
{
  if (dir == LEFT)
    return -1;
  if (dir == RIGHT)
    return 1;
  return 0;
}

static inline int16_t dir_dy(const dir_t dir)
{
  if (dir == UP)
    return -1;
  if (dir == DOWN)
    return 1;
  return 0;
}


#endif /* !__DIR_H__ */
