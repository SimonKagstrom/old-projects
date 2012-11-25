/*********************************************************************
 *
 * Copyright (C) 2003, Simon Kagstrom <ska@bth.se>
 *
 * Filename:      creator.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Main file for the REX adventure creator
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
 * $Id:$
 *
 ********************************************************************/
#include <SDL/SDL.h>

#ifndef __SRC__STRUCTS_H__
#define __SRC__STRUCTS_H__

/* A scene-event */
typedef struct
{
  int x;
  int y;
  int num;
} scene_event_t;

/* An object */
typedef struct
{
  int num;
  SDL_Surface *p_img;
} object_t;

/* A scene */
typedef struct
{
  char *p_level_buf;
  scene_event_t events[MAX_EVENTS];
  int w;
  int h;
} scene_t;

/* This struct describes a game */
typedef struct
{
  scene_t *p_scenes;
  object_t objects[16];
} game_t;

/* The struct for the app */
typedef struct
{
  SDL_Surface *p_screen;
  SDL_Surface *p_bg; /* Current background (i.e. the current scene) */


  char default_block;
  SDL_Surface **pp_block_imgs;
  game_t *p_game;
  int x; /* cursor position */
  int y;
  int cur_block;
  int cur_scene;
} creator_t;

#endif /* !__SRC__STRUCTS_H__ */
