/*********************************************************************
 *
 * Copyright (C) 2001-2003,  Simon Kagstrom <ska@bth.se>
 *
 * Filename:      sysdeps.h
 * Description:   This file contains the defined functions that
 *                each frontend (drawing, keyboard input etc)
 *                should implement.
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
 * $Id: sysdeps.h,v 1.1 2003/02/09 08:14:54 ska Exp $
 *
 *********************************************************************/

/** @file
 *
 * Platform-independent library for REX 6000 programs. The library is
 * optimized for the REX, so most of these functions have a directly
 * corresponding REX system call. Most calls can be defined as macros
 * in order to save execution time and space.
 *
 * The library has been created in order to make development of REX
 * programs easier, mostly for my own good. Why not just write a
 * wrapper around the REX system calls for some other architecture?
 * Well, there are a few calls here that don't have any equivavilent
 * in the REX library, like fe_load_data().
 *
 * The library can be compiled for zcc, sdcc (mostly) and gcc with
 * libSDL so far.
 */
#ifndef FRONTEND_H
#define FRONTEND_H

#include "typedefs.h"  /* uint8_t etc */

#if defined(TARGET_REX)
/* If set to 1, use #defines instead of functions where appropriate */
# define FE_REX_OPT 1
#else
/* Non-REX targets always use functions */
# define FE_REX_OPT 0
#endif /* TARGET_REX */


/* The size of a REX screen */
#define FE_PHYS_WIDTH      240 /**< The physical width of a REX screen, in pixels */
#define FE_PHYS_HEIGHT     120 /**< The physical height of a REX screen, in pixels */
#define FE_PHYS_BYTE_WIDTH (FE_PHYS_WIDTH/8)

/* The events. 16 possible (since a 16-bit number is used) */
#define FE_EVENT_NONE    0
#define FE_EVENT_EXIT    1  /* The home button was pressed */
#define FE_EVENT_BACK    2  /* The back button was pressed */
#define FE_EVENT_SELECT  4  /* The select button was pressed */
#define FE_EVENT_LEFT    8  /* The left/up button was pressed */
#define FE_EVENT_RIGHT   16 /* The right/down button was pressd */

#define FE_FONT_NORMAL   0  /* Normal font */
#define FE_FONT_INVERTED 1  /* Inverted font */
#define FE_FONT_BOLD     16 /* Bold text */
#define FE_FONT_SMALL    32 /* Small text */


#if defined(TARGET_REX)
# if defined(SDCC)
#  include <rex.h>            /* the REX API, SDCC version */
# else
#  include <rex/rex.h>        /* the REX API, z88dk */
# endif /* SDCC */
#endif /* TARGET_REX */

/**
 * Structure for representing points (for stylus-use). This is exactly
 * as found on the REX.
 */
typedef struct
{
  uint16_t x; /**< X position of the stylus. */
  uint16_t y; /**< Y position of the stylus. */
} fe_point_t;

/*
 * Structure for representing events, like key-presses and
 * stylus-pointing. This is exactly as found on the REX.
 *
 * @todo this is not yet used and contains bogus values.
 */
typedef struct
{
  uint8_t tmp;
} fe_event_t;

/**
 * Fill an area of the screen with black color.
 *
 * @param x the x-position to start on.
 * @param y the y-position to start on.
 * @param w the width to fill.
 * @param h the height to fill.
 *
 * @see fe_clear_area()
 * @see fe_draw_bitmap()
 */
#if FE_REX_OPT
# if defined(SDCC)
#  define fe_fill_area(x, y, w, h) RxDisplayBlock((x), (y), (w), (h), 1)
# else
#  define fe_fill_area(x, y, w, h) DsDisplayBlock((x), (y), (w), (h), 1)
# endif /* SDCC */
#else
extern void fe_fill_area(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
#endif /* FE_REX_OPT */

/**
 * Clear an area of the screen (white color)
 *
 * @param x the x-position to start on.
 * @param y the y-position to start on.
 * @param w the width to clear.
 * @param h the height to clear.
 *
 * @see fe_fill_area()
 */
#if FE_REX_OPT
# if defined(SDCC)
#  define fe_clear_area(x, y, w, h) RxDisplayBlockClear((x), (y), (w), (h))
# else
#  define fe_clear_area(x, y, w, h) DsDisplayBlockClear((x), (y), (w), (h))
# endif /* SDCC */
#else
extern void fe_clear_area(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
#endif /* FE_REX_OPT */

/**
 * Clear the entire screen.
 *
 * @see fe_clear_area()
 */
#if FE_REX_OPT
# define fe_clear_screen() (fe_clear_area(0,0, FE_PHYS_WIDTH, FE_PHYS_HEIGHT))
#else
extern void fe_clear_screen(void);
#endif /* FE_REX_OPT */

/**
 * Set a pixel to black color.
 *
 * @param x the x-position of the pixel.
 * @param y the y-position of the pixel.
 */
#if FE_REX_OPT
# if defined(SDCC) /* What to do here? */
#  define fe_set_pixel(x,y)   RxDisplayBlock((x), (y), 1, 1, 1)
# else
#  define fe_set_pixel(x,y)   DsSetPixel(x,y)
# endif /* SDCC */
#else
extern void fe_set_pixel(uint8_t x, uint8_t y);
#endif /* FE_REX_OPT */

/**
 * Clear a pixel to white/green color.
 *
 * @param x the x-position of the pixel.
 * @param y the y-position of the pixel.
 */
#if FE_REX_OPT
# if defined(SDCC) /* What to do here? */
#  define fe_clear_pixel(x,y)   RxDisplayBlockClear((x), (y), 1, 1)
# else
#  define fe_clear_pixel(x,y)   DsClearPixel(x,y)
# endif /* SDCC */
#else
extern void fe_clear_pixel(uint8_t x, uint8_t y);
#endif /* FE_REX_OPT */


/**
 * Draw a bitmap on the screen. Note that this currently only accepts
 * bitmaps with a width being a multiple of 8 (for speed
 * reasons). This function works regardless of if the bitmap is
 * located on the stack or in addin code.
 *
 * The function can draw bitmaps both inverted and non-inverted.
 *
 * @param p_bitmap the bitmap to draw.
 * @param x the x-position to draw the bitmap on.
 * @param y the y-position to draw the bitmap on.
 * @param inverted the mode to draw in (0 normal, 1 inverted).
 *
 * @todo the type of p_bitmap should be changed to fe_image_t so that
 *  we can easily provide other graphics for non-REX targets.
 * @warning the bitmap has to have a multiple-of-8 width.
 * @bug fe_draw_bitmap does not work with tiles of width 32.
 */
#if FE_REX_OPT
# if defined(SDCC) /* What to do here? */
#  define fe_draw_bitmap(p_bitmap, x, y, inverted) RxDisplayBitmapDraw(x,y,p_bitmap,inverted)
# else
#  define fe_draw_bitmap(p_bitmap, x, y, inverted) DsDisplayBitmapDraw(x,y,p_bitmap,inverted)
# endif /* SDCC */
#else
extern void fe_draw_bitmap(uint8_t *p_bitmap, uint8_t x, uint8_t y, uint8_t inverted);
#endif /* FE_REX_OPT */

/**
 * Get the stylus position, if used. The position is stored in a
 * fe_point_t structure.
 *
 * @param p_point the stylus position. This will contain bogus values
 *                if the screen wasn't touched.
 * @return TRUE if the screen was touched, FALSE otherwise.
 */
#if FE_REX_OPT
# if defined(SDCC)
#  define fe_get_stylus(p_point) RxTouchDataGet((p_point))
# else
#  define fe_get_stylus(p_point) DsTouchDataGet((p_point))
# endif /* SDCC */
#else
extern uint8_t fe_get_stylus(fe_point_t *p_point);
#endif /* FE_REX_OPT */

/**
 * Check input from the keys. Returns a bitmask of the touched keys. The
 * possible values are:
 *
 * - <TT>FE_EVENT_NONE</TT>:   No event (nothing pressed).
 * - <TT>FE_EVENT_EXIT</TT>:   Exit key pressed (Esc in SDL).
 * - <TT>FE_EVENT_BACK</TT>:   Back key pressed (Backspace in SDL).
 * - <TT>FE_EVENT_SELECT</TT>: Select/enter key pressed (Space/Ctrl in SDL).
 * - <TT>FE_EVENT_LEFT</TT>:   Left/down key pressed (Left in SDL).
 * - <TT>FE_EVENT_RIGHT</TT>:  Right/up key pressed (Right in SDL).
 *
 * Check events by AND:ing the result with the desired event:
 * <PRE>
 * uint16_t buttons = fe_get_buttons();
 * if (buttons & FE_EVENT_EXIT)
 *  ...
 * </PRE>
 *
 * @return a bitmask of the keys that were pressed, or 0 if no keys
 *         were pressed.
 */
#if FE_REX_OPT
# if defined(SDCC)
#  define fe_get_buttons() (IN(RX_REG_KI_DATA))
# else
#  define fe_get_buttons() (REGISTER_READ(REG_KI_DATA))
# endif /* SDCC */
#else
extern uint16_t fe_get_buttons(void);
#endif /* FE_REX_OPT */

/*
 * Add an event-point (like a button to press). The event is also made
 * active.
 *
 * @param x the x-position of the button
 * @param y the y-position of the button
 * @param w the width of the button
 * @param h the height of the button
 * @param id the identifier to associate with the event
 *
 * @todo This function is not yet implemented.
 */
extern void fe_add_event(uint8_t x, uint8_t y, uint8_t w, uint8_t h, sint16_t id);

/*
 * Delete an event-point (like a button to press).
 *
 * @param id the identifier of the event to be deleted
 *
 * @todo This function is not yet implemented.
 */
extern void fe_del_event(sint16_t id);

/*
 * Wait blocking for an event. When an event occurs, the passed event
 * structure is filled in. This function also returns a bitmask of the
 * currently pressed keys, see fe_get_buttons() for more information.
 *
 * @param p_event a pointer to the event structure to be filled in.
 *
 * @return a bitmask of the keypresses.
 *
 * @see fe_get_buttons()
 * @see fe_get_stylus()
 *
 * @todo This function is not yet implemented
 */
extern uint16_t fe_wait_event(fe_event_t *p_event);

/**
 * Print a string somewhere on the screen. A bitwise combination of
 * the following constants for mode is possible:
 *
 * - <TT>FE_FONT_NORMAL</TT>:    Normal.
 * - <TT>FE_FONT_INVERTED</TT>:  Inverted.
 * - <TT>FE_FONT_BOLD</TT>:      Bold.
 * - <TT>FE_FONT_SMALL</TT>:     Small.
 *
 * Do this by OR:ing the constants together, i.e.:
 * <PRE>
 * fe_print_xy(0,0, FE_FONT_NORMAL | FE_FONT_BOLD, "I print this!");
 * </PRE>
 *
 * Note however that it is not possible to combine @c FE_FONT_BOLD and
 * @c FE_FONT_SMALL.
 *
 * @param x the x-position of the string.
 * @param y the y-position of the string.
 * @param mode the character type to print in.
 * @param p_str the string to be printed.
 *
 * @bug The SDL-port does not fully emulate the behaviour of @c
 *  fe_print_xy(). The normal font is always shown, and the font does
 *  not look quite the same as on the REX (but has approximately the
 *  same size).
 */
#if FE_REX_OPT
# if defined(SDCC) /* What to do here? */
#  define fe_print_xy(x, y, mode, p_str) RxPrintf((x), (y), (mode), (p_str))
# else
#  define fe_print_xy(x, y, mode, p_str) DsPrintf((x), (y), (mode), (p_str))
# endif /* SDCC */
#else
extern void fe_print_xy(uint8_t x, uint8_t y, uint8_t mode, char *p_str);
#endif /* FE_REX_OPT */

/**
 * Load data from a memo. Memos can only, unfortunately, store text.
 *
 * @param p_dst a pointer to the buffer to store the data in.
 * @param offset the offset into the memo to begin reading from.
 * @param len the number of bytes to read
 * @param p_filename the memo name.
 *
 * @return 0 if everything was OK, non-zero value otherwise.
 *
 * @warning I've had problems with this before, and bugs might still
 *  be lurking around here. Seems to be working, though.
 */
extern uint8_t fe_load_data(char *p_dst, uint16_t offset, uint16_t len, char *p_filename);


/**
 * Initialize the frontend. This does things like set the graphics
 * mode etc and should be called at program start. It is very
 * frontend-dependent.
 */
#if FE_REX_OPT
# if defined(SDCC)
#  define fe_init() do {                                       \
	  	    RxEventClear(); /* Clear events */         \
		    fe_clear_screen(); /* Clear the display */ \
		  } while (0)
# else
#  define fe_init() do {                                       \
	  	    DsEventClear();    /* Clear events */      \
		    fe_clear_screen(); /* Clear the display */ \
		  } while (0)
# endif /* SDCC */
#else
extern void fe_init(void);
#endif /* FE_REX_OPT */

/**
 * Clean up after the frontend (clear the screen, exit the program
 * etc). This should be called when the program exits.
 */
#if FE_REX_OPT
# if defined(SDCC)
#  define fe_finalize() do {                \
                        fe_clear_screen();  \
			RxEventClear();     \
			RxAddinTerminate(); \
		      } while (0)
# else
#  define fe_finalize() do {                \
			DsClearScreen();    \
			DsEventClear();     \
			DsAddinTerminate(); \
		      } while (0)
# endif /* SDCC */
#else
extern void fe_finalize(void);
#endif /* FE_REX_OPT */

/**
 * Sleep for a number of ticks (each tick being 1/64s).
 *
 * @param ticks the number of ticks to sleep.
 *
 * @warning passing 0 here will cause the REX to lock (not emulated in
 *  the SDL-port).
 */
#if FE_REX_OPT
# if defined(SDCC) /* What to do here? */
#  define fe_sleep(ticks) RxSleep((ticks));
# else
#  define fe_sleep(ticks) DsSleep((ticks));
# endif /* SDCC */
#else
extern void fe_sleep(uint16_t ticks);
#endif /* FE_REX_OPT */

#endif /* FRONTEND_H */
