/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      main.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Tafl game
 *
 * $Id: main.c 8081 2006-05-10 20:05:53Z ska $
 *
 ********************************************************************/
#include <string.h>
#include <sysdeps/sysdeps.h>

#include "bitmaps.h"
#include "playfield.h"
#include "game.h"
#include "utils.h"

#define BRICK_WIDTH   13
#define BRICK_HEIGHT  11

static int rnd = 0; /* Randomness :-) */

static void game_draw(game_t *p_game, item_t turn);
static void game_init(game_t *p_game, item_t computer_color);

static int IN_RECT(uint8_t x, uint8_t y,
		   uint8_t rx, uint8_t ry,
		   uint8_t w, uint8_t h)
{
  return (x >= rx && x < rx + w &&
	  y >= ry && y < ry + h);
}


static int16_t game_eval_one_pos_black(game_t *p_game, playfield_t *p_playfield,
				       item_t turn, item_t cur, uint8_t x, uint8_t y)
{
  int16_t ret = 5; /* More important than the neighbors */
  dir_t i;

  for (i = 0; i < N_NEIGHBORS/2; i++)
    {
      item_t neighbor = playfield_get_item(p_playfield, x + dir_to_dx(i), y + dir_to_dy(i));

      /* Good to have a friend near */
      if (COLOR(neighbor) == COLOR(cur))
	ret++;
      else if (IS_PIECE(neighbor))
	{
	  if (turn == PF_WHITE)
	    ret--; /* ... But bad to have the enemy up close */
	  else
	    ret++;
	}
    }

  return ret;
}

static int16_t game_eval_one_pos_white_king(game_t *p_game, playfield_t *p_playfield,
					    item_t turn, item_t cur, uint8_t x, uint8_t y)
{
  int16_t ret = -SCORE_WIN; /* The king is alive! */
  dir_t i;

  if (IS_CORNER(x, y))
    return -SCORE_WIN*2;
  /* Good to be close to the corners */
  ret += abs(x - PF_WIDTH/2) + abs(y - PF_HEIGHT/2);

  if ((x == 0 || x == PF_WIDTH-1) ||
      (y == 0 || y == PF_HEIGHT-1))
    ret -= 30; /* On a line towards the corners */
  for (i = 0; i < N_NEIGHBORS/2; i++)
    {
      item_t neighbor = playfield_get_item(p_playfield, x + dir_to_dx(i), y + dir_to_dy(i));

      if (IS_PIECE(neighbor))
	{
	  ret++;
	  if (COLOR(neighbor) == PF_BLACK)
	    ret++;
	}
    }

  return ret;
}

static int16_t game_eval_one_pos_white(game_t *p_game, playfield_t *p_playfield,
				       item_t turn, item_t cur, uint8_t x, uint8_t y)
{
  int16_t ret = -5; /* The value of one piece */
  dir_t i;

  for (i = 0; i < N_NEIGHBORS/2; i++)
    {
      item_t neighbor = playfield_get_item(p_playfield, x + dir_to_dx(i), y + dir_to_dy(i));

      /* White doesn't need to stay close to friends */
      if (COLOR(neighbor) == PF_BLACK)
	ret++; /* ... But bad to have the enemy up close */
    }

  return ret;
}

/* Black is positive, white negative */
static int16_t game_eval_playfield(game_t *p_game, playfield_t *p_playfield, item_t turn)
{
  uint8_t x,y;
  int16_t ret = SCORE_WIN; /* Assume king is taken */

  for (y = 0; y < PF_HEIGHT; y++)
    {
      for (x = 0; x < PF_WIDTH; x++)
	{
	  item_t cur = playfield_get_item(p_playfield, x, y);

	  switch(cur)
	    {
	    case PF_BLACK:
	      ret += game_eval_one_pos_black(p_game, p_playfield, turn, cur, x, y);
	      break;
	    case PF_WHITE:
	      ret += game_eval_one_pos_white(p_game, p_playfield, turn, cur, x, y);
	      break;
	    case PF_WHITE_KING:
	      ret += game_eval_one_pos_white_king(p_game, p_playfield, turn, cur, x, y);
	      break;
	    default:
	      break;
	    }
	}
    }

  return ret;
}

/* Implementation of the minimax algorithm. Once was a alpha-beta
 * implementation, but since the REX only evaluates one ply down I
 * think this can be skipped. It's available in subversion.
 */
static int16_t game_minimax(game_t *p_game, playfield_t *p_playfield, item_t turn,
			    int depth, int maxdepth, move_t *p_out)
{
  item_t next_turn;
  uint8_t x,y;
  int16_t best_eval;

  /* End the recursion */
  if (depth >= maxdepth)
    return game_eval_playfield(p_game, p_playfield, turn);
  best_eval = (turn == PF_BLACK) ? -PF_INF : PF_INF;
  next_turn = (turn == PF_WHITE) ? PF_BLACK : PF_WHITE;

  for (y = 0; y < PF_HEIGHT; y++)
    {
      for (x = 0; x < PF_WIDTH; x++)
	{
	  item_t cur = playfield_get_item(p_playfield, x, y);
	  dir_t i;

	  if (COLOR(cur) != turn)
	    continue;

	  for (i = 0; i < N_NEIGHBORS/2; i++)
	    {
	      move_t move;
	      uint8_t dist;

	      /* Generate all possible moves in the current dir */
	      for (dist = 1; dist < PF_WIDTH; dist++)
		{
		  playfield_t next_playfield;
		  int16_t eval;
		  int dx = x + dir_to_dx(i)*dist;
		  int dy = y + dir_to_dy(i)*dist;

		  /* Hit an obstacle, no need to check the rest */
		  if (!playfield_can_move(p_playfield, x, y, dx, dy))
		    break;

		  /* Execute move on a new playfield */
		  memcpy(&next_playfield, p_playfield, sizeof(playfield_t));
		  playfield_do_move(&next_playfield, NULL, x, y, dx, dy);

		  /* Evaluate recursively */
		  eval = game_minimax(p_game, &next_playfield, next_turn,
				      depth + 1, maxdepth, &move);
		  if ( (turn == PF_BLACK && eval >= best_eval) ||
		       (turn == PF_WHITE && eval <= best_eval) )
		    {
		      if (eval == best_eval &&
			  (rnd & 4) != 0)
			continue;
		      p_out->sx = x;
		      p_out->sy = y;
		      p_out->dx = dx;
		      p_out->dy = dy;
		      best_eval = eval;
		    }
		}
	    }
	}
    }

  return best_eval;
}

void game_add_captured_piece(game_t *p_game, item_t item)
{
  const char *p_image = NULL;
  int nr = p_game->captured_pieces[item];
  int x = ( (nr >= 10) ? (nr - 10) : nr) * 8;
  int y = ( (nr >= 10) ? 8 : 0) + item * 16;

  switch(item)
    {
    case PF_BLACK:
      p_image = black_image; break;
    case PF_WHITE:
      p_image = white_image; break;
    default:
      break;
    }

  if (p_image)
    fe_draw_bitmap(p_image, PF_WIDTH * BRICK_WIDTH + 6 + x, 62 + y, 0);
  p_game->captured_pieces[item]++;
}

static void game_won(game_t *p_game, int val)
{
  char *msg = "Restarting...";

  fe_fill_area(FE_PHYS_WIDTH / 2 - 90, FE_PHYS_HEIGHT / 2 - 18,
	       84, 36);
  fe_clear_area(FE_PHYS_WIDTH / 2 - 88, FE_PHYS_HEIGHT / 2 - 16,
		80, 32);
  if (val < 0)
    msg = "White won!";
  else if (val > 0)
    msg = "Black won!";
  fe_print_xy(FE_PHYS_WIDTH / 2 - 76, FE_PHYS_HEIGHT / 2 - 8, FE_FONT_NORMAL, msg);
  fe_sleep(128);
  game_init(p_game, p_game->computer_color);
  fe_clear_area(PF_WIDTH * BRICK_WIDTH + 4, 59,
		FE_PHYS_WIDTH - PF_WIDTH * BRICK_WIDTH - 8, 3*8 + 4);
  game_draw(p_game, p_game->starting_color);
}

void game_draw_one_pos(game_t *p_game, uint8_t x, uint8_t y, int inverted)
{
  item_t item;
  const char *p_image = NULL;
  const char *p_board_image = (y == PF_HEIGHT-1) ? board_image_last_row : board_image;

  if (IS_CORNER(x,y) || IS_THRONE(x,y))
    p_board_image = (y == PF_HEIGHT-1) ? board_corner_image_last_row : board_corner_image;
  fe_draw_bitmap(p_board_image, x*BRICK_WIDTH, y*BRICK_HEIGHT, inverted);
  item = playfield_get_item(p_game->p_playfield, x, y);

  switch(item)
    {
    case PF_BLACK:
      p_image = black_image; break;
    case PF_WHITE:
      p_image = white_image; break;
    case PF_WHITE_KING:
      p_image = white_king_image; break;
    default:
      break;
    }
  if (p_image)
    fe_draw_bitmap(p_image, x*BRICK_WIDTH+2, y*BRICK_HEIGHT+1, 0);
}

#define RESTART_BUTTON_X (FE_PHYS_WIDTH - 34)
#define RESTART_BUTTON_Y (FE_PHYS_HEIGHT - 16)
#define HUMAN_BUTTON_X   (FE_PHYS_WIDTH - 13)
#define HUMAN_BUTTON_Y   (RESTART_BUTTON_Y - 16)
#define WHITE_BUTTON_X   (HUMAN_BUTTON_X - 16)
#define WHITE_BUTTON_Y   (HUMAN_BUTTON_Y)
#define BLACK_BUTTON_X   (WHITE_BUTTON_X - 16)
#define BLACK_BUTTON_Y   (HUMAN_BUTTON_Y)

static void game_draw_status(game_t *p_game, item_t turn)
{
  char *msg_turn = "Black plays...";
  char *msg_computer = "(Computer)";
  fe_clear_area(PF_WIDTH * BRICK_WIDTH + 4, 14,
		FE_PHYS_WIDTH - PF_WIDTH * BRICK_WIDTH, 33);

  if (turn == PF_WHITE)
    msg_turn = "White plays...";
  if (turn != p_game->computer_color)
    msg_computer = "(Player)";

  fe_print_xy(PF_WIDTH * BRICK_WIDTH + 50, 0, FE_FONT_BOLD, "REX Tafl");
  fe_print_xy(PF_WIDTH * BRICK_WIDTH + 4, 14, FE_FONT_NORMAL, msg_turn);
  fe_print_xy(PF_WIDTH * BRICK_WIDTH + 4, 26, FE_FONT_NORMAL, msg_computer);

  /* Draw the buttons */
  fe_print_xy(PF_WIDTH * BRICK_WIDTH + 4, HUMAN_BUTTON_Y+1, FE_FONT_SMALL, "Computer:");
  fe_draw_bitmap(restart_button_image, RESTART_BUTTON_X, RESTART_BUTTON_Y, p_game->restart);
  fe_draw_bitmap(human_button_image, HUMAN_BUTTON_X, HUMAN_BUTTON_Y, p_game->computer_color == PF_INVALID);
  fe_draw_bitmap(white_button_image, WHITE_BUTTON_X, WHITE_BUTTON_Y, p_game->computer_color == PF_WHITE);
  fe_draw_bitmap(black_button_image, BLACK_BUTTON_X, BLACK_BUTTON_Y, p_game->computer_color == PF_BLACK);
}

static void game_draw(game_t *p_game, item_t turn)
{
  uint8_t x,y;

  /* fe_draw_bitmap(title_image, FE_PHYS_WIDTH - 64, 2, 0); */

  game_draw_status(p_game, turn);

  fe_print_xy(PF_WIDTH * BRICK_WIDTH + 4, 46, FE_FONT_NORMAL, "Status");
  /* Draw a line (captured pieces below) */
  fe_fill_area(PF_WIDTH * BRICK_WIDTH + 4, 58,
	       FE_PHYS_WIDTH - PF_WIDTH * BRICK_WIDTH - 8, 1);

  /* Draw the board */
  for (y = 0; y < PF_HEIGHT; y++)
    {
      for (x = 0; x < PF_WIDTH; x++)
	game_draw_one_pos(p_game, x, y, 0);
    }
}

static void game_do(game_t *p_game)
{
  item_t turn = p_game->starting_color;
  int eval = 0;
  bool_t selected = FALSE;

  p_game->selected_x = -1;
  p_game->selected_y = -1;

  while (1)
    {
      fe_point_t where;
      move_t move;

      if (abs(eval) > 1000)
	{
	  game_won(p_game, eval);
	  eval = 0;
	}

      if (turn == p_game->computer_color)
	{
	  game_minimax(p_game, p_game->p_playfield, turn,
		       0, 1, &move);
	  playfield_do_move(p_game->p_playfield, p_game, move.sx, move.sy, move.dx, move.dy);
	  turn = (turn == PF_BLACK) ? PF_WHITE : PF_BLACK;
	  eval = game_eval_playfield(p_game, p_game->p_playfield, turn);
	  game_draw_status(p_game, turn);
	  continue;
	}

      if (fe_get_stylus(&where) == TRUE)
	{
	  uint8_t field_x = where.x / BRICK_WIDTH;
	  uint8_t field_y = where.y / BRICK_HEIGHT;
	  item_t item = playfield_get_item(p_game->p_playfield, field_x, field_y);

	  rnd += field_x ^ rnd;
	  /* Select something */
	  if (COLOR(item) == turn)
	    {
	      /* Within field, correct color */
	      game_draw_one_pos(p_game, p_game->selected_x, p_game->selected_y, 0);
	      game_draw_one_pos(p_game, field_x, field_y, 1);
	      selected = TRUE;
	      p_game->selected_x = field_x;
	      p_game->selected_y = field_y;
	    }
	  else if (selected && item == PF_EMPTY)
	    {
	      /* Destination - move to! */
	      if (playfield_can_move(p_game->p_playfield, p_game->selected_x, p_game->selected_y, field_x, field_y))
		{
		  playfield_do_move(p_game->p_playfield, p_game, p_game->selected_x, p_game->selected_y, field_x, field_y);
		  turn = (turn == PF_BLACK) ? PF_WHITE : PF_BLACK;
		}
	      game_draw_status(p_game, turn);
	      selected = FALSE;
	      eval = game_eval_playfield(p_game, p_game->p_playfield, turn);
	    }
	  else
	    {
	      /* Pressed outside the playfield */
	      game_draw_one_pos(p_game, p_game->selected_x, p_game->selected_y, 0);
	      selected = FALSE;

	      /* Handle the buttons */
	      if (IN_RECT(where.x, where.y,
			  RESTART_BUTTON_X, RESTART_BUTTON_Y, 34, 13))
		{
		  p_game->restart = 1;
		  game_won(p_game, 0);
		  p_game->restart = 0;
		}
	      if (IN_RECT(where.x, where.y,
			  HUMAN_BUTTON_X, HUMAN_BUTTON_Y, 13, 13))
		p_game->computer_color = PF_INVALID;
	      else if (IN_RECT(where.x, where.y,
			       BLACK_BUTTON_X, BLACK_BUTTON_Y, 13, 13))
		p_game->computer_color = PF_BLACK;
	      else if (IN_RECT(where.x, where.y,
			       WHITE_BUTTON_X, WHITE_BUTTON_Y, 13, 13))
		p_game->computer_color = PF_WHITE;

	      game_draw_status(p_game, turn);
	    }
	}
      if ( fe_get_buttons() == FE_EVENT_EXIT)
	break;
    }
}

static void game_init(game_t *p_game, item_t computer_color)
{
  memset(p_game, 0, sizeof(game_t));

  memcpy(&p_game->initial_playfield, &playfield_initial, sizeof(playfield_t));
  p_game->p_playfield = &p_game->initial_playfield;

  p_game->starting_color = PF_BLACK;
  p_game->computer_color = computer_color;
}

int main(int argc, char *argv[])
{
  game_t game;

  fe_init();
  fe_clear_screen();

  game_init(&game, PF_WHITE);
  game_draw(&game, game.starting_color);
  game_do(&game);

  return 0;
}
