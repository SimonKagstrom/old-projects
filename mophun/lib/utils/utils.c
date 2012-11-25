/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      utils.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Utils
 *
 * $Id: utils.c 607 2004-09-14 18:17:25Z ska $
 *
 ********************************************************************/
#include <vmgp.h>
#include <vstream.h>

#include <utils.h>

/* Get the screen size, from 2dbackground.c */
int get_screen_size(int32_t *p_w, int32_t *p_h)
{
  VIDEOCAPS videocaps;

  videocaps.size = sizeof(VIDEOCAPS);

  if(vGetCaps(CAPS_VIDEO, &videocaps))
    {
      *p_w  = videocaps.width;
      *p_h = videocaps.height;
    }
  else
    return -1;

  return 0;
}


/* Bresenhams algorithm, from http://www.fact-index.com/b/br/bresenham_s_line_algorithm_c_code.html */
int bresenham(int x0, int y0, int x1, int y1,
	      int (*callback)(int x, int y, void *p), void *p_callback_arg)
{
  int steep = 1;
  int sx, sy;  /* step positive or negative (1 or -1) */
  int dx, dy;  /* delta (difference in X and Y between points) */
  int e;
  int i;

  /*
   * inline swap. On some architectures, the XOR trick may be faster
   */
  int tmpswap;
#define SWAP(a,b) tmpswap = a; a = b; b = tmpswap;

  /*
   * optimize for vertical and horizontal lines here
   */
  dx = abs(x1 - x0);
  sx = ((x1 - x0) > 0) ? 1 : -1;
  dy = abs(y1 - y0);
  sy = ((y1 - y0) > 0) ? 1 : -1;
  if (dy > dx)
    {
      steep = 0;
      SWAP(x0, y0);
      SWAP(dx, dy);
      SWAP(sx, sy);
    }
  e = (dy << 1) - dx;
  for (i = 0; i < dx; i++)
    {
      if (steep)
	{
	  if (callback(x0,y0, p_callback_arg))
	    return 1;
	}
      else
	{
	  if (callback(y0,x0, p_callback_arg))
	    return 1;
	}
      while (e >= 0)
	{
	  y0 += sy;
	  e -= (dx << 1);
	}
      x0 += sx;
      e += (dy << 1);
    }
#undef SWAP

  return 0;
}

void print_font(VMGPFONT *p_font, uint32_t color, int32_t x, int32_t y, const char *msg)
{
  vSetForeColor(color);

  vSetActiveFont(p_font);
  vPrint(MODE_TRANS, x,y, msg);
}

/* One, general but more complex, way of doing this */
SPRITE **unpack_sprite_frames(uint8_t *p_packed_data, int unpacked_size, int n_sprites)
{
  SPRITE *p_src; /* The source sprites (i.e. one sprite with many frames) */
  SPRITE **pp_dst; /* The return vector of sprite-pointers */
  SPRITE *p_dst_data; /* The data for the new sprites */
  uint8_t *p_cur_data;
  uint16_t w;
  uint16_t h;
  int pad;
  int i;

  /* Allocate space for the source */
  if ( !(p_src = (SPRITE*)vNewPtr(unpacked_size)) )
    return NULL;

  /* Decompress the compressed data */
  if (vDecompress(p_packed_data, (uint8_t*)p_src, 0, 0) < 0)
    goto cleanup1;
  w = p_src->width; /* Get the width and the height */
  h = p_src->height/n_sprites;
  p_src->height = h; /* Quick fix */

  /* Padding, thanks Martin! */
  pad = (w*h + sizeof(SPRITE)) & 1;

  /* Allocate the pointer-vector and the SPRITE:s. The sprites come directly after the vector */
  if ( !(pp_dst = vNewPtr( ((sizeof(SPRITE*) + sizeof(SPRITE) + (w*h) + pad))*n_sprites)) )
    goto cleanup2; /* Out of memory! */
  p_dst_data = (SPRITE*)((uint8_t*)pp_dst + sizeof(SPRITE*)*n_sprites);

  /* Allocate all sprites */
  p_cur_data = (uint8_t*)(p_src + 1);
  for (i=0; i<n_sprites; i++)
    {
      memcpy(p_dst_data, p_src, sizeof(SPRITE)); /* Copy the sprite header */
      memcpy(p_dst_data+1, p_cur_data, (w*h));   /* ... and the data */
      /* Store this pointer */
      pp_dst[i] = p_dst_data;
      p_dst_data = (SPRITE*)((uint8_t*)p_dst_data + (w*h)+sizeof(SPRITE) + pad);
      p_cur_data += (w*h);
    }

  /* Voila! We have a vector of sprites. */
  vDisposePtr(p_src);
  return pp_dst;

  /* Oh no, an error! Free the allocated data */
 cleanup2:
  vDisposePtr(pp_dst);
 cleanup1:
  vDisposePtr(p_src);

  return NULL;
}

uint8_t *unpack_data(uint8_t *p_packed_data, int unpacked_size)
{
  uint8_t *p_dst;

  /* Allocate space for the source */
  if ( !(p_dst = (uint8_t*)vNewPtr(unpacked_size)) )
    return NULL;

  /* Decompress the compressed data */
  if (vDecompress(p_packed_data, (uint8_t*)p_dst, 0, 0) < 0)
    {
      vDisposePtr(p_dst);
      return NULL;
    }

  return p_dst;
}

void *get_resource(int32_t id, uint32_t size)
{
  uint8_t *p_out;
  int fd;
  int n_read;

  if ( !(p_out = (uint8_t*)vNewPtr(size)) )
    {
      debug_msg("Alloc space failed\n");
      return NULL;
    }

  if ( (fd = vStreamOpen((const char*)NULL, (id << 16) | STREAM_RESOURCE | STREAM_READ )) < 0)
    {
      debug_msg("vStreamOpen failed!\n");
      vDisposePtr(p_out);
      return NULL;
    }

  if ((n_read = vStreamRead(fd, p_out, size)) != size)
    {
      debug_msg("vStreamRead(): Could not read enough: %d vs %d!\n", n_read, size);
      vDisposePtr(p_out);
      return NULL;
    }
  vStreamClose(fd);

  return p_out;
}

#include <vmgpsonyericsson.h>

void store_midifile(const char *filename, uint8_t *p_data, uint32_t size)
{
  int fd;
  int n_read;

  fd = vStreamOpen(filename, STREAM_FILE | STREAM_READ);
  if (fd < 0)
    {
      fd = vStreamOpen(filename, STREAM_FILE | STREAM_WRITE | STREAM_BINARY | STREAM_CREATE);
      if (fd < 0)
	return; /* Error in opening! */
      vStreamWrite(fd, p_data, size);
      vStreamClose(fd);
    }
  else
    vStreamClose(fd);
}

void play_midifile(const char *filename)
{
  static SYSCTL_SONYERICSSON_SOUND_INFO_t midisound;

  midisound.SoundFormat = SYSCTL_SONYERICSSON_SOUND_MIDI;
  midisound.StartOffset = 0;
  midisound.RepeatSound = 0;
  midisound.RepeatDelay = 0;

  /* Play the tune! */
  vStrCpy(midisound.Filename, filename);
  if (vSysCtl(SYSCTL_SONYERICSSON_SOUND, (uint32_t)&midisound) != SYSCTL_SONYERICSSON_SOUND_OK)
    {
      error_msg("Could not play sound!\n");
    }
}
