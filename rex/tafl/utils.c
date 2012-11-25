/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      utils.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Misc utils to have it compile on all platforms
 *
 * $Id: utils.c 8020 2006-05-07 08:40:10Z ska $
 *
 ********************************************************************/
#include "utils.h"

#ifndef SDCC
/* Not present in z88dk stdlib.h */
int abs(int x)
{
  if (x > 0)
    return x;
  return -x;
}

int min(int x, int y)
{
  if (x < y)
    return x;
  return y;
}

int max(int x, int y)
{
  if (x > y)
    return x;
  return y;
}
#endif
