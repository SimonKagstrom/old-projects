/*********************************************************************
 * Copyright (C) 2001-2002,  Simon Kagstrom
 *
 * Filename:      advlib.h
 * Description:   This file contains the defined functions
 *                for the AdvLib library.
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
 * $Id: advlib.h,v 1.7 2002/10/21 17:41:44 simon Exp $
 *
 *********************************************************************/

/** @file
 *
 * Adventure (Zelda-style) support library. This library provides
 * some assistance for writing Zelda-style games. The main feature is
 * that it provides loadable background maps and that it can draw
 * objects on the map without destroying the background.
 *
 * This is mainly written for the REX 6000, but can also be compiled
 * and run on systems with libSDL.
 *
 * Advlib uses text-files (memos on the REX) for storing the
 * scenes in a simple format that is meant to be easy to use.
 *
 * The scenes are just strings of ascii-characters between 'A' and
 * 'z', allowing for at most 57 different tiles. Each string
 * make up one scene and constist of width*height characters. An
 * example can look as follows:
 *
 * <PRE>
 * AAACDAAAAA
 * AABCDBBAAA
 * AAACDABAAA
 * BAACDAABAA
 * AAACDBAABA
 * AAACDBAAAA
 * </PRE>
 *
 * Could be one scene, with A-, B-, C- and D-tiles in 10x6
 * configuration. You just have to remove the newlines to get a scene
 * that adv_load_scene can use. Therefore,
 *
 * <PRE>
 * AAACDAAAAAAABCDBBAAAAAACDABAAABAACDAABAAAAACDBAABAAAACDBAAAA
 * </PRE>
 *
 * would be a valid scene. Different scenes are separated by an extra
 * newline. The scenes are numbered by their order in the file,
 * starting at 0.
 */
#ifndef ADVLIB_H
#define ADVLIB_H

#include "typedefs.h"

/**
 * The scene structure. You will probably not want to access any of
 * the elements here. Use adv_init_scene() to initialize a new
 * adv_scene_t.
 */
typedef struct
{
  uint8_t *p_buf;        /**< a pointer to the map buffer */
  sint16_t w;            /**< the width of the scene */
  sint16_t h;            /**< the height of the scene */
  uint8_t nr_tiles;      /**< the number of background tiles (images) present. */
  uint8_t *p_tile_buf;   /**< a pointer to the tile buffer */
  uint8_t tw;            /**< the width of the tiles in the scene */
  uint8_t th;            /**< the height of the tiles in the scene */

  /* Private things */
  sint16_t scene_mult;
} adv_scene_t;

/**
 * Initialize a scene. This sets the default values for the entire
 * game and must be called before the use of any other function in
 * advlib.
 *
 * @note The width of the tile images must be a power of of 2,
 * i.e. 2,4,8,16,32 etc. You will probably only want 8, 16 or 32. The
 * scene-data and object buffers <I>must</I> be stored on the stack to
 * have the library function correctly.
 *
 * @param p_scene the scene to fill in.
 * @param p_buf the buffer for storing the scene data. This has to be
 *  allocated to (w*h) bytes on the stack.
 * @param w the width of the scene data. Must be a power of two.
 * @param h the height of the scene data.
 * @param nr_objs the number of background objects in the game (not
 *  nessessarily on the scene).
 * @param p_obj_buf a pointer to the background object data (the
 *  images). The images are stored in <I>one</I> structure one after
 *  another. All images <I>must</I> have the same size. If advlib is
 *  used on the REX as a library, this also has to be stack-allocated
 *  since addin space cannot be passed to libraries.
 */
extern void adv_init_scene(adv_scene_t *p_scene, uint8_t *p_buf, sint16_t w, sint16_t h,
			   uint8_t nr_objs, uint8_t *p_obj_buf);

/**
 * Load data from a memo or file into a scene. This should be called
 * when the player enters a scene. If the scene is not initialized
 * with adv_init_scene() first, this will give undefined results.
 *
 * @param p_dst_scene the scene to fill in.
 * @param p_filename the memo to load the scene from.
 * @param scene the number of the scene to load.
 */
extern void adv_load_scene(adv_scene_t *p_dst_scene, char *p_filename, uint8_t scene);

/**
 * Mask an object against the background. This uses a mask and an
 * image. The mask is an image that specifies (with black) the areas
 * where the background should <I>not</I> be visible. The mask and the
 * image <I>must</I> be smaller or equal to the tile size.
 *
 * @todo Specifying widths other than 8 here will cause the function
 * to behave incorrectly.
 *
 * @param p_scene the map to use as background.
 * @param p_dst the destination bitmap (to draw upon).
 * @param p_mask the mask to use.
 * @param p_obj the object to draw.
 * @param x the X-position of the object.
 * @param y the Y-position of the object.
 *
 * @see adv_mask_background()
 */
extern void adv_mask_object(adv_scene_t *p_scene, uint8_t *p_dst, uint8_t *p_mask, uint8_t *p_obj,
			    sint16_t x, sint16_t y);

/**
 * Draw a part of the background. This function is typically used to
 * redraw the path on which the player has moved (to not leave
 * "droppings" of the player graphics).
 *
 * @todo Specifying widths other than 8 here will cause the function
 * to behave incorrectly.
 *
 * @param p_scene the scene to use as background.
 * @param p_dst the destination bitmap (to draw upon).
 * @param x the X-position to start on.
 * @param y the Y-position to start on.
 * @param w the width to draw.
 * @param h the height to draw.
 *
 * @see adv_mask_object()
 */
extern void adv_mask_background(adv_scene_t *p_scene, uint8_t *p_dst,
				sint16_t x, sint16_t y, uint8_t w, uint8_t h);

/**
 * Draw a scene on the screen. This should be called once when a
 * player enters the scene. As the player (or enemies or friends)
 * move, adv_mask_background() and adv_mask_object() should be used to
 * redraw the place where the object moved.
 *
 * @param p_scene the map to draw. This has to be filled in first with
 *                adv_load_scene().
 *
 * @see adv_mask_object()
 * @see adv_mask_background()
 * @see adv_load_scene()
 */
extern void adv_draw_scene(adv_scene_t *p_scene);

/**
 * Fill in a char-vector with the surroundings of a position. This
 * function will fill p_dst with the tile-types (char's) of the
 * surroundings of the given position. p_dst is filled with position 0
 * being upper left, 1 upper right, 2 lower left and 3 lower right.
 *
 * @param p_scene the scene to check.
 * @param p_dst a vector of 4 characters. Don't pass anything else here!
 * @param x the X-position to check.
 * @param y the Y-position to check.
 * @param w the width to check.
 * @param h the height to check.
 */
extern void adv_get_surroundings(adv_scene_t *p_scene, unsigned char *p_dst,
				 sint16_t x, sint16_t y, uint8_t w, uint8_t h);

#endif /* ADVLIB_H */
