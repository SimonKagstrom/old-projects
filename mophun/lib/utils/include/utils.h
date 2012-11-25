/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      utils.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Util defs.
 *
 * $Id: utils.h 606 2004-09-14 18:14:38Z ska $
 *
 ********************************************************************/
#ifndef __UTILS_H__
#define __UTILS_H__

#ifndef abs
# define abs(x) ( (x) < 0 ? -(x) : (x) )
#endif

#ifndef max
# define max(x, y) ( (x) > (y) ? (x) : (y) )
#endif

#ifndef min
# define min(x, y) ( (x) < (y) ? (x) : (y) )
#endif

#define TRUE 1
#define FALSE 0

#define IS_SET(x, val) ( (x) & val )
#define SET_BITS(x, val) ( (x) |= (val) )
#define SET_BIT_NR(x, nr) ( (x) | (1<<(nr)) )
#define CLEAR_BIT_NR(x, nr) ( (x) | ~(1<<(nr)) )
#define CLEAR_BITS(x, val) ( (x) &= ~(val) )

#ifndef TARGET_ANSIC
# include <vmgp.h>
# include <vstream.h>
# include <vmgputil.h>
# ifndef NDEBUG
#  define assert(x) do { if (!(x)) error_msg("Assert failed %s:%d\n",__FUNCTION__, __LINE__); } while(0)
#  define debug_msg(x...) do {DbgPrintf(x); } while(0)
# else
#  define assert(x)
#  define debug_msg(x...) do {DbgPrintf(x); } while(0)
# endif /* NDEBUG */
# define error_msg(x...) do {debug_msg(x); DbgBreak(); vTerminateVMGP();} while(0)

static inline int sign(int x)
{
  if (x < 0)
    return -1;
  if (x > 0)
    return 1;
  return 0;
}

static inline int get_manhattan_dist(int x1, int y1, int x2, int y2)
{
  return abs(x2-x1) + abs(y2-y1);
}

int get_screen_size(int32_t *p_w, int32_t *p_h);
int bresenham(int x0, int y0, int x1, int y1, int (*callback)(int x, int y, void *p), void *p_callback_arg);
SPRITE **unpack_sprite_frames(uint8_t *p_packed_data, int unpacked_size, int n_sprites);
uint8_t *unpack_data(uint8_t *p_packed_data, int unpacked_size);
void *get_resource(int32_t id, uint32_t size);
void print_font(VMGPFONT *p_font, uint32_t color, int32_t x, int32_t y, const char *msg);
void store_midifile(const char *filename, uint8_t *p_data, uint32_t size);
void play_midifile(const char *filename);
#else
# define debug_msg(x...) do {printf(x); } while(0)
# define error_msg(x...) do {debug_msg(x); exit(1);} while(0)
#endif /* TARGET_ANSIC */

#endif /* !__UTILS_H__ */
