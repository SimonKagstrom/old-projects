/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      types.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Typedefs
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __TYPES_H__
#define __TYPES_H__

#ifndef TARGET_ANSIC
# include <vmgp.h>
#endif

typedef uint16_t bool_t;
typedef uint16_t tile_t;

typedef struct
{
  uint8_t id;
  uint8_t attr;
} mask_tile_t;

#endif /* !__TYPES_H__ */
