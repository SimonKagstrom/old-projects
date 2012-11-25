/*********************************************************************
 *
 * Copyright (C) 2001-2003,  Simon Kagstrom <ska@bth.se>
 *
 * Filename:      typedefs.h
 * Description:   General typedefs for REX projects.
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
 * $Id: typedefs.h $
 *
 *********************************************************************/
#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#if defined(TARGET_SDL)
#include <SDL/SDL.h>
#endif /* TARGET_SDL */

#if defined(TARGET_REX)
typedef unsigned char bool_t;

typedef unsigned char uint8_t;     /*  0..255       */
typedef signed char   int8_t;      /* -128..127     */
typedef unsigned int  uint16_t;    /*  0..65535     */
typedef signed int    int16_t;     /* -32768..32767 */
typedef signed long   int32_t;     /* -2M..2M       */
typedef unsigned long uint32_t;    /*  0..4M        */

#elif defined(TARGET_SDL)
typedef Uint8 bool_t;

/* These use the SDL defs for portability */
typedef Uint8  uint8_t;
typedef Sint8  int8_t;
typedef Uint16 uint16_t;
typedef Sint16 int16_t;
typedef Sint32 int32_t;
typedef Uint32 uint32_t;

#else
/* Add your platform here */
#error No target specified! Compile with -DTARGET_REX or -DTARGET_SDL
#endif

/* Some convenient definitions */
#undef TRUE
#define TRUE  1

#undef FALSE
#define FALSE 0

#ifndef NULL
#define NULL ((void*)0)
#endif /* NULL */

#endif
