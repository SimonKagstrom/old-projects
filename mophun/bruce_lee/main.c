/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Bruce Lee game
 *
 * $Id: main.c 2678 2005-06-01 17:24:20Z ska $
 *
 ********************************************************************/
#include <game.h>
#include <menu.h>
#include "res.h"

int32_t screen_w, screen_h;

#define SLEEP_PERIOD 60

static level_mask_t check_bg_collision(game_t *p_game, sprite_t *p_sprite, dir_t dir, point_t new_pt, point_t *p_overlap);

void sprite_set_state(sprite_t *p_sprite, sprite_state_t state)
{
  p_sprite->state = state;
  p_sprite->move_table_idx = 0;
  p_sprite->table_n = 0;
}

#ifndef NDEBUG
static void draw_rect(game_t *p_game, rect_t rect, uint32_t color)
{
    vSetForeColor(color);
    vDrawLine(rect.top_left.x-p_game->bg_x,rect.top_left.y-p_game->bg_y, rect.bottom_right.x-p_game->bg_x,rect.top_left.y-p_game->bg_y);
    vDrawLine(rect.top_left.x-p_game->bg_x,rect.top_left.y-p_game->bg_y, rect.top_left.x-p_game->bg_x,rect.bottom_right.y-p_game->bg_y);
    vDrawLine(rect.bottom_right.x-p_game->bg_x,rect.top_left.y-p_game->bg_y,
	      rect.bottom_right.x-p_game->bg_x,rect.bottom_right.y-p_game->bg_y);
    vDrawLine(rect.top_left.x-p_game->bg_x,rect.bottom_right.y-p_game->bg_y,
	      rect.bottom_right.x-p_game->bg_x,rect.bottom_right.y-p_game->bg_y);
}
#else
#define draw_rect(x,y,z)
#endif /* NDEBUG */

static void center_map(game_t *p_game)
{
  p_game->bg_x = p_game->player.sprite.pt.x - screen_w / 2;
  p_game->bg_y = p_game->player.sprite.pt.y - screen_h / 2;

  if (p_game->bg_x < 0)
    p_game->bg_x = 0; /* Stop at (0,?) */
  else if (p_game->bg_x > p_game->p_cur_level->w*TILE_W - screen_w)
    p_game->bg_x = p_game->p_cur_level->w*TILE_W - screen_w; /* Stop at (screen_w, ?)*/

  if (p_game->bg_y < 0)
    p_game->bg_y = 0;
  else if (p_game->bg_y > p_game->p_cur_level->h*TILE_H - screen_h)
    p_game->bg_y = p_game->p_cur_level->h*TILE_H - screen_h;
}

static level_mask_t tile_at(game_t *p_game, point_t where)
{
  assert(where.x < p_game->p_cur_level->w &&
	 where.y < p_game->p_cur_level->h);

  /* FIXME! Scale this down */
  return p_game->p_cur_level->p_level_mask[where.y * p_game->p_cur_level->w + where.x];
}

static void draw_game(game_t *p_game)
{
  center_map(p_game);

  vMapSetXY(p_game->bg_x, p_game->bg_y);
  vUpdateMap();

  /* Draw everything */
  vDrawObject(p_game->player.sprite.pt.x-p_game->bg_x, p_game->player.sprite.pt.y-p_game->bg_y,
	      p_game->pp_sprites[p_game->player.sprite.frame]);
  vFlipScreen(1);
}

static point_t move_table(game_t *p_game, sprite_t *p_sprite, anim_table_t *p_table)
{
  point_t out;
  int cur = p_sprite->move_table_idx;

  if (p_sprite->dir == LEFT)
    {
      out = pt_add(p_sprite->pt, pt_flip(p_table->entries[cur].pt_offset));
      p_sprite->frame = p_table->entries[cur].anim_frame;
    }
  else
    {
      out = pt_add(p_sprite->pt, p_table->entries[cur].pt_offset);
      p_sprite->frame = p_table->entries[cur].anim_frame + p_sprite->offset_right;
    }

  if (++p_sprite->table_n >= p_table->entries[cur].n)
    {
      p_sprite->move_table_idx++;
      p_sprite->table_n = 0;
    }

  if (p_sprite->move_table_idx >= p_table->n_entries)
    {
      p_sprite->move_table_idx = 0;
      p_sprite->table_n = 0;
      p_sprite->state = SPRITE_IDLE;
    }

  return out;
}

static void move_sprite(game_t *p_game, sprite_t *p_sprite)
{
  level_mask_t left_tile = 0, right_tile = 0, up_tile, down_tile;
  point_t horiz_overlap, vert_overlap;
  point_t old_pt = p_sprite->pt;
  point_t new_pt;

  new_pt = move_table(p_game, p_sprite, p_sprite->p_anims[p_sprite->state]);

  /* Check collisions with walls */
  if (p_sprite->dir == LEFT &&
      p_sprite->state != SPRITE_CLIMB)
    left_tile = check_bg_collision(p_game, p_sprite, LEFT, new_pt, &horiz_overlap);

  if (p_sprite->dir == RIGHT &&
      p_sprite->state != SPRITE_CLIMB)
    right_tile = check_bg_collision(p_game, p_sprite, RIGHT, new_pt, &horiz_overlap);

  up_tile = check_bg_collision(p_game, p_sprite, UP, new_pt, &vert_overlap);

  if (tile_type_map[up_tile] == VINES ||
      tile_type_map[left_tile] == VINES ||
      tile_type_map[right_tile] == VINES)
    {
      if (p_sprite->state != SPRITE_CLIMB)
	sprite_set_state(p_sprite, SPRITE_CLIMB);
      p_sprite->pt = new_pt;
      return;
    }

  if (left_tile || right_tile)
    {
      /* Undo the movement */
      new_pt = old_pt;
      new_pt.x += right_tile ? -2 : 2;
      sprite_set_state(p_sprite, SPRITE_IDLE);
    }
  down_tile = check_bg_collision(p_game, p_sprite, DOWN, new_pt, &vert_overlap);

  /* Fall? */
  if (!down_tile)
    {
      if (p_sprite->state == SPRITE_RUN ||
	  p_sprite->state == SPRITE_IDLE ||
	  p_sprite->state == SPRITE_HIT)
	sprite_set_state(p_sprite, SPRITE_FALL);
    }
  else if (p_sprite->state == SPRITE_FALL ||
	   p_sprite->state == SPRITE_JUMP_RUN)
    {
      sprite_set_state(p_sprite, SPRITE_IDLE);
      new_pt.y -= vert_overlap.y;
    }

  p_sprite->pt = new_pt;
}

/* Can probably be made more efficient - like only checking one area */
static level_mask_t check_bg_collision(game_t *p_game, sprite_t *p_sprite, dir_t dir, point_t new_pt, point_t *p_overlap)
{
  rect_t rect;
  int16_t x,y;

  rect = rect_add_pt(p_sprite->p_bounds[p_sprite->frame], new_pt);

  switch (dir)
    {
    case LEFT:
      rect.bottom_right.x = rect.top_left.x + TILE_W;
      rect.bottom_right.y-=TILE_H;
      break;
    case RIGHT:
      rect.top_left.x = rect.bottom_right.x - TILE_W;
      rect.bottom_right.y-=TILE_H; /* FIXME! Better with allow left/right */
      break;
    case UP:
      rect.bottom_right.y = rect.top_left.y + TILE_W;
      break;
    case DOWN:
      rect.top_left.y = rect.bottom_right.y - TILE_W;
      break;
    }

#ifndef NDEBUG
  {
    rect_t rect2 = rect_add_pt(p_sprite->p_bounds[p_sprite->frame], p_sprite->pt);

    draw_rect(p_game, rect2, vRGB(255,255,255));
  }
#endif
  draw_rect(p_game, rect, vRGB(255,0,0));

  for (y = rect.top_left.y/TILE_H; y <= rect.bottom_right.y/TILE_H; y++)
    {
      for (x = rect.top_left.x/TILE_W; x <= rect.bottom_right.x/TILE_W; x++)
	{
	  level_mask_t tile = tile_at(p_game, pt(x,y));

	  if (tile > 1)
	    {
	      rect_t tile_rect = rect_add_pt(tile_bounds[tile-1], pt_to_sprite(pt(x,y)));

	      draw_rect(p_game, tile_rect, vRGB(0,255,0));
	      if (rect_overlap(tile_rect, rect, p_overlap))
		{
		  assert(tile != 0);

#ifndef NDEBUG
		  DbgPrintf("  %d:%d Tile %d rect %d:%d -> %d:%d (overlap %d,%d)\n\n", dir, p_sprite->state, tile,
			    tile_rect.top_left.x,tile_rect.top_left.y,
			    tile_rect.bottom_right.x, tile_rect.bottom_right.y,
			    p_overlap->x, p_overlap->y
			    );
		  vFlipScreen(0);
#endif
		  return tile;
		}
	    }
	}
    }
  /* TMP! */
  DbgPrintf("\n");

#ifndef NDEBUG
  vFlipScreen(0);
#endif

  return 0;
}

static void move_hero(game_t *p_game, player_t *p_player)
{
  move_sprite(p_game, &p_player->sprite);

  /* Check exit to the right/left */
  if (p_player->sprite.pt.x < 0)
    {
      if (p_game->p_cur_level->p_adjacent_screens[LEFT])
	level_goto(p_game, p_game->p_cur_level->p_adjacent_screens[LEFT], pt(300, p_player->sprite.pt.y) );
    }
  else if (p_player->sprite.pt.x + p_player->sprite.p_bounds[p_player->sprite.frame].bottom_right.x > LEVEL_WIDTH-4)
    {
      if (p_game->p_cur_level->p_adjacent_screens[RIGHT])
	level_goto(p_game, p_game->p_cur_level->p_adjacent_screens[RIGHT], pt(0, p_player->sprite.pt.y) );
    }
}

static void handle_input(game_t *p_game, uint32_t keys)
{
  sprite_t *p_sprite = &p_game->player.sprite;

  if (!keys)
    {
      if (p_sprite->state == SPRITE_CROUCH)
	sprite_set_state(p_sprite, SPRITE_IDLE);
    }

  /* Handle running and jumping */
  if (p_sprite->state == SPRITE_IDLE ||
      p_sprite->state == SPRITE_RUN)
    {
      if (keys & KEY_LEFT)
	{
	  p_sprite->state = SPRITE_RUN;
	  p_sprite->dir = LEFT;
	}
      else if (keys & KEY_RIGHT)
	{
	  p_sprite->state = SPRITE_RUN;
	  p_sprite->dir = RIGHT;
	}
      if (keys & KEY_DOWN)
	sprite_set_state(p_sprite, SPRITE_CROUCH);

      /* Handle jumping */
      if (p_sprite->state == SPRITE_IDLE &&
	  (keys & KEY_UP))
	sprite_set_state(p_sprite, SPRITE_JUMP);
      else if (p_sprite->state == SPRITE_RUN &&
	       (keys & KEY_UP))
	sprite_set_state(p_sprite, SPRITE_JUMP_RUN);
    }
  else if (p_sprite->state == SPRITE_CLIMB)
    {
      anim_table_t *p_cur = p_sprite->p_anims[SPRITE_CLIMB];

      if (!keys)
	p_cur->entries[0].pt_offset = pt(0, 0);
      else
	{
	  if (p_game->player.sprite.move_table_idx == 0)
	    {
	      if (p_cur->entries[0].anim_frame == FRAME_BRUCE_CLIMB)
		{
		  p_cur->entries[0].anim_frame = FRAME_BRUCE_CLIMB+1;
		  p_cur->entries[1].anim_frame = FRAME_BRUCE_CLIMB+1;
		}
	      else
		{
		  p_cur->entries[0].anim_frame = FRAME_BRUCE_CLIMB;
		  p_cur->entries[1].anim_frame = FRAME_BRUCE_CLIMB;
		}
	    }

	  p_sprite->dir = RIGHT;

	  if (keys & KEY_UP)
	    p_cur->entries[0].pt_offset = pt(0, -4);
	  else if (keys & KEY_DOWN)
	    p_cur->entries[0].pt_offset = pt(0, 4);
	  else if (keys & KEY_LEFT)
	    p_cur->entries[0].pt_offset = pt(-4, 0);
	  else if (keys & KEY_RIGHT)
	    p_cur->entries[0].pt_offset = pt(4, 0);
	}

      /* Cannot hit etc when climbing */
      return;
    }

  if (keys & KEY_FIRE)
    {
      if (p_sprite->state == SPRITE_RUN)
	sprite_set_state(p_sprite, SPRITE_KICK);
      else if (p_sprite->state == SPRITE_IDLE &&
	       p_sprite->state != SPRITE_HIT)
	sprite_set_state(p_sprite, SPRITE_HIT);
    }
}


static void do_game(game_t *p_game)
{
  while(1)
    {
      uint32_t before, after;
      uint32_t keys;

      before = vGetTickCount(); /* Get the current ms ticks */

      keys = vGetButtonData();
      handle_input(p_game, keys);
      move_hero(p_game, &p_game->player);

      if (keys & KEY_SELECT)
	return;
      draw_game(p_game);

      after = vGetTickCount(); /* Get the current ms ticks */
      /* Every loop iteration should take about SLEEP_PERIOD, see to that */
      if ((after - before) < SLEEP_PERIOD)
	{
	  msSleep( SLEEP_PERIOD - (after-before) );
	}
    }
}


static void game_init(game_t *p_game)
{
  void *p_buf;

  memset(p_game, 0, sizeof(game_t));

  if ( !(p_buf = (void *)get_resource(SPRITE_FRAMES, SPRITE_FRAMES_SIZE)) )
    error_msg("Alloc space for sprite frames\n");

  p_game->pp_sprites = unpack_sprite_frames(p_buf, SPRITE_FRAMES_UNPACKED_SIZE, N_FRAMES);
  vDisposePtr(p_buf);

  p_game->p_tiles = BG_TILES;
  p_game->player.sprite.dir = RIGHT;

  sprite_set_state(&p_game->player.sprite, SPRITE_IDLE);

  p_game->player.sprite.offset_right = FRAME_BRUCE_FIRST_RIGHT;
  p_game->player.sprite.p_bounds = bruce_bounding;
  p_game->player.sprite.p_anims = bruce_anims;

  if ( !(p_buf = get_resource(BRUCE_MIDI, BRUCE_MIDI_SIZE)) )
    error_msg("Alloc space for midi file failed!\n");
  if ( !(p_game->p_midi_data = unpack_data(p_buf, BRUCE_MIDI_UNPACKED_SIZE)) )
    error_msg("Unpack midi failed!\n");
  vDisposePtr(p_buf);

  store_midifile("bruce.mid", p_game->p_midi_data, BRUCE_MIDI_UNPACKED_SIZE);
  vDisposePtr(p_game->p_midi_data);

  play_midifile("bruce.mid");

  if ( !(p_buf = (void *)get_resource(TITLE_SCREEN, TITLE_SCREEN_SIZE)) )
    error_msg("Alloc space for sprite frames\n");
  if ( !(p_game->p_title_screen = (SPRITE*)unpack_data(p_buf, TITLE_SCREEN_UNPACKED_SIZE)) )
    error_msg("Unpack title screen failed!\n");
  vDisposePtr(p_buf);
}

static char *main_menu_msgs[] =
  {
    "START NEW GAME",  /* 0 */
    "OPTIONS",         /* 1 */
    "HELP",            /* 2 */
    "SELECT SCREEN",
    "^|1|2",
    " ",               /* 5 */
    "QUIT",            /* 6 */
    (char*)NULL,
  };

int main(void)
{
  game_t game;
  menu_t main_menu;
  VMGPFONT small_font;
  int submenus[1];
  bool_t done = FALSE;

  small_font.width = 4;
  small_font.height = 6;
  small_font.bpp = 1;
  small_font.palindex = 1;
  small_font.chartbl = SMALL_FONT+16; /* Which characters are present in the font? */
  small_font.fontdata = SMALL_FONT+16+256; /* The pixel data */

  get_screen_size(&screen_w, &screen_h);

  game_init(&game);

  menu_init(&main_menu, &small_font, main_menu_msgs,
	    0, game.p_title_screen->height, screen_w, screen_h);


  while (!done)
    {
      /* Display title screen */
      vSetTransferMode(MODE_BLOCK);
      vClearScreen(vRGB(0,0,0));
      vDrawObject(0,0, game.p_title_screen);

      switch(menu_select(&main_menu, ~0, submenus))
	{
	case 1:
	  break;
	case 6:
	  return 1;
	case -1:
	default:
	  break;
	}

      vSetTransferMode(MODE_TRANS);
      vPlayResource(NULL,0,SOUND_FLAG_STOP);

      level_goto(&game, &levels[submenus[0]], pt(0, 80));
      do_game(&game);
    }

  return 0;
}
