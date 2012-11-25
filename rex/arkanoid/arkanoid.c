/*********************************************************************
 *
 * Copyright (C) 2001-2003,  Simon Kagstrom <ska@bth.se>
 *
 * Filename:      arkanoid.c
 * Description:   This file contains the (hopefully) platform
 *                independent implementation of a arkanoid game.
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
 * $Id: arkanoid.c $
 *
 ********************************************************************/
#include <stdlib.h>       /* abs */
#include <string.h>       /* memset */

#include <sysdeps/typedefs.h> /* uint8_t etc */
#include <sysdeps/sysdeps.h>
#include "arkanoid.h"
#include "draw.h"

static sint16_t dir_chgs[] =
  {/*dx, dy*/
    /* Normal */
    1,  0,
    0,  1,
    1,  0,
    0,  1,

    /* Fast */
    1,  0,
    1,  0,
    1,  0,
    0,  1,

    /* One-dir */
    1,  0,
    1,  0,
    1,  0,
    1,  0,
  };

int rand_nr;
#define SRAND() (rand_nr++) /* Used to get "random" numbers ;-) */

static uint16_t get_input(game_t *p_game);

/* ---------- INITIALIZATION RELATED ---------- */
/* Initialize a block */
static void init_block(block_t *p_block, uint8_t type)
{
  p_block->type = type;

  /* Set the number of hits for the block (ignore the special-flag) */
  switch(p_block->type & (0xff ^ TYPE_SPECIAL))
    {
    case TYPE_SIMPLE:
      p_block->hits = 1;
      break;
    case TYPE_HARD:
      p_block->hits = 2;
      break;
    case TYPE_IMPOSSIBLE:
      p_block->hits = 255;
      break;
    }

  if (p_block->type & TYPE_SPECIAL)
    {
      /* This block has a random special, 1/4 can be bad specials, 5/6 are only good.
       * 1/6 of the "only goods" can be very good (extra life, "destroyer" balls etc)
       */
      if (rand_nr & 1)
	p_block->special = 1<<(SRAND()%(SPECIAL_MAX-4)); /* Good specials */
      else
	p_block->special = 1<<(SRAND()%SPECIAL_MAX); /* Can be very good and bad specials */
      DEBUG(printf("Init special %d\n", p_block->special));
    }
}

/* Initialize paddle values */
static void init_paddle(paddle_t *p_paddle, uint8_t type, sint16_t x)
{
  p_paddle->type = type;
  p_paddle->x = x;
  p_paddle->width = PADDLE_WIDTH;
}

/* Initialize ball values */
static void init_ball(ball_t *p_ball, sint16_t x, sint16_t y, uint8_t state)
{
  p_ball->x = x;
  p_ball->y = y;
  p_ball->dx = 0; /* The ball is not moving at the start */
  p_ball->dy = 0;
  p_ball->type = BALL_TYPE_NORMAL;
  p_ball->swap = 0;
  p_ball->step = 0;
  p_ball->state = state;
}

static void init_special(special_t *p_special, sint16_t x, sint16_t y, uint8_t type)
{
  p_special->x = x;
  p_special->y = y;
  p_special->type = type;
}

/* ---------- FIELD RELATED ---------- */
/* "Load" a plaayfield and set the game to that one */
static int goto_level(game_t *p_game)
{
  int i,j;

  if (p_game->level >= p_game->levels)
    return -1;

  rand_nr = 0;
  p_game->nr_blocks=0;
  fe_load_data(p_game->p_levels, p_game->level*((FIELD_WIDTH+1)*(FIELD_HEIGHT-4)+1)+6, (FIELD_WIDTH+1)*FIELD_HEIGHT, "Zarkanoid");

  /* Set the data for the level */
  for(j=0; j<FIELD_HEIGHT-4; j++)
    {
      for(i=0; i<FIELD_WIDTH; i++)
	{
	  uint8_t curr = p_game->p_levels[j*(FIELD_WIDTH+1)+i]-'0';
	  block_t *p_block = &p_game->p_field[j*FIELD_WIDTH+i];

	  /* If field[x,y] is not empty */
	  if (curr && !((curr & TYPE_IMPOSSIBLE) == TYPE_IMPOSSIBLE))
	    p_game->nr_blocks++;
	  init_block(p_block, curr);
	  draw_block(p_block, i, j);
	}
    }

  return 0;
}

/* ---------- PADDLE RELATED ---------- */
/* Move the paddle */
static void update_paddle(paddle_t *p_paddle, sint16_t left, sint16_t right)
{
  p_paddle->lastx = p_paddle->x;

  /* Update paddle position */
  p_paddle->x += right-left;

  /* If the paddle is off-field - move it within again. */
  if (p_paddle->x < 0)
    p_paddle->x = 0;
  else if (p_paddle->x > SCREEN_WIDTH-p_paddle->width)
    p_paddle->x = SCREEN_WIDTH-p_paddle->width;
}

/* ---------- BALL RELATED ---------- */
/* Return true if the ball and the paddle collides */
#define paddle_ball_collide(p_paddle, p_ball) ((p_ball)->x+BALL_WIDTH > (p_paddle)->x        && \
					       (p_ball)->x < (p_paddle)->x+(p_paddle)->width && \
					       (p_ball)->y >= PADDLE_Y-PADDLE_HEIGHT)

/* Move the ball. Returns 1 if the ball exists the screen */
static int update_ball(game_t *p_game, ball_t *p_ball)
{
  /* Update each ball according to its speed */
  p_ball->lastx = p_ball->x;
  p_ball->lasty = p_ball->y;

  /* Bounce against the walls */
  if (p_ball->x+BALL_WIDTH >= SCREEN_WIDTH)
    {
      p_ball->dx = -p_ball->dx;
      p_ball->x = SCREEN_WIDTH-BALL_WIDTH-1; /* FIXME: Should not be needed */
    }
  else if (p_ball->x <= 0)
    {
      p_ball->dx = -p_ball->dx;
      p_ball->x = 1;
    }
  if (p_ball->y <= 0)
    {
      p_ball->dy = -p_ball->dy;
      p_ball->y = 1;
    }

  /* Move the ball */
  p_ball->x += p_ball->dx*dir_chgs[p_ball->type * 8 + p_ball->step + p_ball->swap];
  p_ball->y += p_ball->dy*dir_chgs[p_ball->type * 8 + p_ball->step + 1 - p_ball->swap];
  p_ball->step = (p_ball->step + 1) & 3; /* Next step */

  /* If the ball hits the floor, bounce it and weaken the floor */
  if ((p_game->state & SPECIAL_FLOOR) &&
       p_ball->y > (SCREEN_HEIGHT-FLOOR_HEIGHT-BALL_HEIGHT))
    {
      /* If the floor is removed - clear the floor area. */
      if (!(--p_game->state & SPECIAL_FLOOR))
	fe_clear_area(0, (SCREEN_HEIGHT-FLOOR_HEIGHT), SCREEN_WIDTH, FLOOR_HEIGHT);
      p_ball->y = (SCREEN_HEIGHT-FLOOR_HEIGHT-BALL_HEIGHT)-1;
      p_ball->dy = -p_ball->dy;
    }

  /* Return if the ball is on the field */
  return p_ball->y > (SCREEN_HEIGHT+1);
}

static void bounce_ball_paddle(game_t *p_game, ball_t *p_ball)
{
  if (p_ball->x < p_game->paddle.x+BALL_WIDTH)
    {
      /* The ball hits the paddle on the left side */
      p_ball->type = BALL_TYPE_FAST;
      p_ball->dx = -1;
      DEBUG(printf("Fast left!\n"));
    }
  else if (p_ball->x > p_game->paddle.x+p_game->paddle.width-BALL_WIDTH)
    {
      /* The ball hits the paddle on the right side */
      p_ball->type = BALL_TYPE_FAST;
      p_ball->dx =  1;
      DEBUG(printf("Fast right!\n"));
    }
  else
    {
      /* The ball hits the paddle in the middle, dont change */
    }
  p_ball->dy = -1;
  p_ball->y = PADDLE_Y-PADDLE_HEIGHT-1; /* next iteration shouldn't cause an immediate collision */
}

/* Check if the ball collides with some block (and handle that) */
static void block_ball_collide(game_t *p_game, ball_t *p_ball)
{
  sint16_t x = (p_ball->x+BALL_WIDTH/2)/BLOCK_WIDTH;
  sint16_t y = (p_ball->y+BALL_HEIGHT/2)/BLOCK_HEIGHT;
  sint16_t rest_x = (p_ball->x+BALL_WIDTH/2)%BLOCK_WIDTH;
  block_t *p_block;

  /* Off field - ignore! */
  if (y > FIELD_HEIGHT-1 ||
      x < 0              ||
      x > FIELD_WIDTH-1)
    return;
  p_block = &p_game->p_field[y*FIELD_WIDTH+x];

  /* If the block is not empty -> collision */
  if (p_block->type != TYPE_EMPTY)
    {
      int block_above = (y>0)?(p_block-FIELD_WIDTH)->type:FALSE;
      int block_below = (y<FIELD_HEIGHT)?(p_block+FIELD_WIDTH)->type:FALSE;
      int block_left  = (x>0)?(p_block-1)->type:FALSE;
      int block_right = (x<FIELD_WIDTH)?(p_block+1)->type:FALSE;
      int other_vertical_block = (p_ball->y-(y*BLOCK_HEIGHT) > BLOCK_HEIGHT/2)?block_below:block_above;
      int other_horizontal_block = (p_ball->x-(x*BLOCK_WIDTH) > BLOCK_WIDTH/2)?block_left:block_right;
      /* We save space above by using (p_block-1) instead of p_game->p_field[y*FIELD_WIDTH+(x-1)] etc */

      /* Lower the number of hits the field can take */
      if (--p_block->hits == 0)
	{
	  /* Remove block */
	  p_game->nr_blocks--;
	  if (p_block->type & TYPE_SPECIAL && p_game->free_special != -1)
	    {
	      init_special(&p_game->p_specials[p_game->free_special], x*BLOCK_WIDTH, y*BLOCK_HEIGHT, p_block->special);
	      DEBUG(printf("Special %d\n", p_game->p_specials[p_game->free_special].type));
	    }
	  init_block(p_block, TYPE_EMPTY);

	  /* For the "destroyer", invert the speed if the block is
	     removed (it will be inverted once more afterwards,
	     causing it to just keep on.) */
	  if (p_ball->state & BALL_STATE_DESTROYER)
	    {
	      draw_block(p_block, x, y);
	      p_ball->state--; /* This implements a timeout for the destroyer (max number of blocks) */
	      return;
	    }
	}
      /* Check if the ball is bouncing against the short side of the block or the long side
       * Since we're here we know that the ball is within the block borders, so there are two
       * possible cases (and two subcases):
       *
       * 1:  \  ____       ____  /  Here, the ball strikes a block which has a neighbor
       *      o|____|  or |____|o   It should then bounce off x-wise
       *     / |____|     |____| \
       *             OR
       * 2:  \  ____       ____  /  Here, the ball strikes a single block, in which case it
       *      o|____|  or |____|o   should bounce off if it is within the block y-borders
       *     /                   \
       *        ____ ____
       * 3:    |____|____|          This illustrates a case where the ball shouldn't bounce
       *            o               to the right regardless of if case 2 is true
       *           / \
       *
       * Alas, this does not work perfect. Perhaps a lookup-table instead?
       */
      if (((p_ball->lastx < p_ball->x && p_ball->x+BALL_WIDTH/2 < x*BLOCK_WIDTH+3) ||
	   (p_ball->lastx > p_ball->x && p_ball->x+BALL_WIDTH/2 > (x+1)*BLOCK_WIDTH-3)) &&
	  (other_vertical_block || /* Another block above or below exists (case 1) */
	   (p_ball->y+BALL_HEIGHT/2 > y*BLOCK_HEIGHT && /* Case 2 */
	    p_ball->y+BALL_HEIGHT/2 < (y+1)*BLOCK_HEIGHT &&
	    !other_horizontal_block))) /* Case 3 */
	{
	  /* Short side - change x */
	  p_ball->dx = -p_ball->dx;
	}
      else
	{
	  /* Long side - change y.
	   *
	   * Three cases:
	   * 1:    ____   2:  ____    3:  ____
	   *      |____|     |____|	 |____|
	   *     /|            /\	     /`o
	   *      o              o
	   *
	   * Plus three corresponding cases where the ball is
	   * travelling from right to left. The same is valid if the
	   * ball comes from above.
	   *
	   * If the ball hits the first third of the block, case 1
	   * applies. For the last third, case 3 applies and otherwise
	   * case 2.
	   */
	  /* Ball is travelling to the right */
	  if (rest_x < (BLOCK_WIDTH/3)+BALL_WIDTH/2)
	    {
	      /* Case 1, one-way dy */
	      p_ball->type = BALL_TYPE_ONE_WAY;
	      p_ball->swap = 1; /* dy instead of dx */
	      p_ball->dy = -p_ball->dy;
	      p_ball->dx = (p_ball->dx > 0) ? 0 : -1;
	      DEBUG(printf("Case 1\n"));
	    }
	  else if (rest_x > 2*(BLOCK_WIDTH/3)-BALL_WIDTH/2)
	    {
	      /* Case 3, fast dx */
	      p_ball->type = BALL_TYPE_FAST;
	      p_ball->swap = 0; /* Don't swap */
	      p_ball->dy = -p_ball->dy;
	      p_ball->dx += (p_ball->dx == 0) ? 1 : 0;
	      DEBUG(printf("Case 3\n"));
	    }
	  else /* Case 2, just bounce */
	    {
	      p_ball->dy = -p_ball->dy;
	      p_ball->swap = 0; /* Don't swap */
	      DEBUG(printf("Case 2\n"));
	    }
	}
      /* Always draw the block if it's hit (in order to have it look nicer and to remove it) */
      draw_block(p_block, x, y);
    }
}

/* Move the special */
static int update_special(game_t *p_game, special_t *p_special)
{
  sint16_t y = (p_special->y+SPECIAL_HEIGHT)/BLOCK_HEIGHT;
  block_t *p_block;

  p_special->lasty = p_special->y;

  /* This special has moved off-field -> signal for removal */
  if (y > FIELD_HEIGHT-1)
    return -1;
  p_block = &p_game->p_field[y*FIELD_WIDTH+p_special->x/BLOCK_WIDTH];

  /* Unless the special is placed on a block - don't move it */
  if (p_block->type == TYPE_EMPTY)
    {
      /* Move the special downwards */
      p_special->y++;

      /* If the special hits the floor - stop it! */
      if (p_game->state & SPECIAL_FLOOR && p_special->y > (SCREEN_HEIGHT-FLOOR_HEIGHT-SPECIAL_HEIGHT))
	p_special->y = (SCREEN_HEIGHT-FLOOR_HEIGHT-SPECIAL_HEIGHT);

      /* Check if the player picks it up */
      if (p_special->x+SPECIAL_WIDTH > p_game->paddle.x &&
	  p_special->x < p_game->paddle.x+p_game->paddle.width && /* X-overlap */
	  p_special->y >= PADDLE_Y-PADDLE_HEIGHT) /* Y-overlap */
	return 1;
    }

  return 0;
}


/* ---------- GAME RELATED ---------- */
static uint16_t get_input(game_t *p_game)
{
  uint16_t out = fe_get_buttons();
  fe_point_t point;

  /* Get data from touch screen */
  if (fe_get_stylus(&point))
    {
      /*
       * The touch screen is parsed as follows:
       * 1. If the point touched is to the left of the paddle,
       *    the paddle should move to the left.
       * 2. If the point is to the right - move right
       * 3. If the point is on the upper half of the screen
       *    assume that the player is firing (regardless of
       *    left/right).
       */
      if (point.x < p_game->paddle.x+(PADDLE_WIDTH/2))
	out |= FE_EVENT_LEFT;
      else
	out |= FE_EVENT_RIGHT;
      if (point.y < (SCREEN_HEIGHT/2))
	out |= FE_EVENT_SELECT;
    }

  /* This is possible since the events match the rex keys */
  return out;
}

/* Called when the player finishes a level (move to next) */
static int handle_level_finished(game_t *p_game)
{
  fe_clear_screen();
  draw_screen(p_game);

  /* Move the paddle to starting place */
  init_paddle(&p_game->paddle, PADDLE_NORMAL, (SCREEN_WIDTH/2)-(PADDLE_WIDTH/2));

  /* Init the balls (only one active) */
  memset(p_game->p_balls, 0, sizeof(ball_t)*MAX_BALLS);
  init_ball(&p_game->p_balls[0], p_game->paddle.x+(PADDLE_WIDTH/2), PADDLE_Y-PADDLE_HEIGHT, BALL_STATE_ACTIVE|BALL_STATE_HOLDING);
  p_game->nr_balls = 1;
  p_game->state = 0;

  /* Clear the specials */
  memset(p_game->p_specials, 0, sizeof(special_t)*MAX_SPECIALS);

  p_game->level++;

  return goto_level(p_game);
}

/* The main game loop */
static void do_game(game_t *p_game)
{
  uint8_t exit=FALSE;

  while(!exit)
    {
      uint16_t input = get_input(p_game);
      uint16_t fire;
      int i;

      exit = input & FE_EVENT_EXIT;
      fire = input & FE_EVENT_SELECT;

      /* Move the paddle (according to input) */
      update_paddle(&p_game->paddle, input & FE_EVENT_LEFT ? 4:0, input & FE_EVENT_RIGHT ? 4:0);

      /* Handle the ball(s) */
      p_game->free_ball = -1;
      for (i=0; i<MAX_BALLS; i++)
	{
	  /* Unused balls are skipped */
	  if (!(p_game->p_balls[i].state & BALL_STATE_ACTIVE))
	    {
	      draw_ball(&p_game->p_balls[i]);
	      p_game->free_ball = i;
	      continue;
	    }

	  /* The player is holding the ball */
	  if (p_game->p_balls[i].state & BALL_STATE_HOLDING)
	    {
	      p_game->p_balls[i].lastx = p_game->p_balls[i].x;
	      p_game->p_balls[i].x += p_game->paddle.x-p_game->paddle.lastx;

	      if (fire)
		{
		  /* The player relases the ball. The following will
		   * clear the HOLDING-bit. It will also shoot the
		   * ball away upwards.
		   */
		  p_game->p_balls[i].dx = 1;
		  p_game->p_balls[i].state &= (0xff ^ BALL_STATE_HOLDING);
		  bounce_ball_paddle(p_game, &p_game->p_balls[i]);
		}
	    }
	  else if (paddle_ball_collide(&p_game->paddle, &p_game->p_balls[i]))
	    {
	      /* Hold the ball if the pad is sticky */
	      if (p_game->paddle.type & SPECIAL_STICKY)
		{
		  p_game->p_balls[i].dy = 0;
		  p_game->p_balls[i].dx = 0;
		  p_game->p_balls[i].state |= BALL_STATE_HOLDING;
		  p_game->paddle.type--; /* Sticky for maximum 3 rounds */
		}
	      else
		bounce_ball_paddle(p_game, &p_game->p_balls[i]);
	    }
	  /* Check collisions against blocks */
	  block_ball_collide(p_game, &p_game->p_balls[i]);

	  draw_ball(&p_game->p_balls[i]);

	  /* Update the ball (and check if it moves out of the playfield) */
	  if (update_ball(p_game, &p_game->p_balls[i]))
	    {
	      /* Ball is out of bounds - Remove it */
	      init_ball(&p_game->p_balls[i], 0, 0, 0);
	      p_game->nr_balls--;
	    }
	}
      /* Handle the specials */
      p_game->free_special = -1;
      for (i=0; i<MAX_SPECIALS; i++)
	{
	  int res;

	  /* Always draw the specials for timing-reasons */
	  draw_special(&p_game->p_specials[i]);
	  /* Inactive special - continue */
	  if (!p_game->p_specials[i].type)
	    {
	      p_game->free_special = i;
	      continue;
	    }

	  /* This moves the special and also checks if it collides with the paddle */
	  res = update_special(p_game, &p_game->p_specials[i]);
	  switch(res)
	    {
	    case 1: /* Player took the special */
	      switch(p_game->p_specials[i].type)
		{
		case SPECIAL_EXTRA_BALL:
		  if (p_game->free_ball != -1) /* Check if there is free space for the ball */
		    {
		      p_game->nr_balls++;
		      init_ball(&p_game->p_balls[p_game->free_ball], p_game->paddle.x+PADDLE_WIDTH/2,
				PADDLE_Y-PADDLE_HEIGHT, BALL_STATE_ACTIVE);
		    }
		  break;
		case SPECIAL_LONG_PADDLE:
		  p_game->paddle.width = PADDLE_LONG_WIDTH;
		  break;
		case SPECIAL_SHORT_PADDLE:
		  p_game->paddle.width = PADDLE_SHORT_WIDTH;
		  break;
		case SPECIAL_EXTRA_LIFE:
		  if (p_game->lives < MAX_LIVES)
		    {
		      p_game->lives++;
		      draw_lives(p_game);
		    }
		  break;
		case SPECIAL_STICKY:
		  /*
		   * For the sticky and destroyers, we use the bits
		   * from the flag and downwards as counters.
		   *
		   * i.e: SPECIAL_STICKY is the third bit, which means
		   * that type becomes 0b00000111. type is then
		   * lowered when the ball hits the paddle and after
		   * zeroing the lower bits, the SPECIAL_STICKY bit is
		   * finally cleared. This gives a maximum of three
		   * sticky rounds.
		   *
		   * These two only adds 30 bytes to the binary :-)
		   */
		  p_game->paddle.type = (SPECIAL_STICKY | (SPECIAL_STICKY-1));
		  break;
		case SPECIAL_DESTROYER:
		  p_game->p_balls[0].state |= (BALL_STATE_DESTROYER | (BALL_STATE_DESTROYER-1));
		  break;
		case SPECIAL_FLOOR:
		  p_game->state |= (SPECIAL_FLOOR | 2); /* Three times */
		  fe_fill_area(0, SCREEN_HEIGHT-FLOOR_HEIGHT, SCREEN_WIDTH, FLOOR_HEIGHT);
		  break;
		}
	      /* Fall through */
	    case -1:
	      /* Remove the special */
	      p_game->nr_specials--;
	      fe_clear_area(p_game->p_specials[i].x, p_game->p_specials[i].y-1, SPECIAL_WIDTH, SPECIAL_HEIGHT+1);
	      p_game->p_specials[i].type = SPECIAL_NONE;
	      break;
	    }
	}

      /* No balls left in play */
      if (!p_game->nr_balls)
	{
	  if (--p_game->lives == 0)
	    exit = TRUE; /* No lives left! */
	  else
	    {
	      /* Restart the game */
	      p_game->nr_balls = 1;
	      draw_screen(p_game);
	      init_paddle(&p_game->paddle, SPECIAL_NONE, SCREEN_WIDTH/2-PADDLE_WIDTH/2);
	      init_ball(&p_game->p_balls[0], p_game->paddle.x+p_game->paddle.width/2,
			PADDLE_Y-PADDLE_HEIGHT, BALL_STATE_HOLDING | BALL_STATE_ACTIVE);
	    }
	}
      /* No blocks left to remove */
      if (!p_game->nr_blocks)
	{
	  fe_sleep(LEVEL_SLEEP_PERIOD);

	  if (handle_level_finished(p_game) < 0)
	    exit = TRUE; /* Last level */
	}

      draw_paddle(&p_game->paddle);
      /* Sleep for a short while. */
#if !defined(TARGET_REX)
      fe_sleep(SLEEP_PERIOD);
#endif /* TARGET_REX */
    }
}

/* The main part of the program */
#if defined(TARGET_SDL)
int main(int argc, char **argv)
#else
void main()
#endif
{
  game_t game;

  /* Initialize the front end */
  fe_init();

  /* Initialize the game */
  /* Create the game */
  memset(&game, 0, sizeof(game_t));

  /* Create the paddle */
  init_paddle(&game.paddle, SPECIAL_NONE, (SCREEN_WIDTH/2)-(PADDLE_WIDTH/2));

  /* Init the balls (only one active) */
  init_ball(&game.p_balls[0], game.paddle.x+(PADDLE_WIDTH/2), PADDLE_Y-PADDLE_HEIGHT, BALL_STATE_ACTIVE|BALL_STATE_HOLDING);
  game.nr_balls = 1;

  /* Init the specials */
  game.lives = NR_LIVES;

  /* Set the numbe of levels */
  if (fe_load_data(game.p_levels, 0, (FIELD_WIDTH+1)*FIELD_HEIGHT, "Zarkanoid"))
    fe_finalize(); /* Could not load the levels */
  game.levels = (game.p_levels[0]-'0')*10+(game.p_levels[1]-'0'); /* Number of levels */
  game.level = 5*((game.p_levels[3]-'0')*10+(game.p_levels[4]-'0')); /* Starting level (skip 5 levels at a time) */

#if defined(TARGET_SDL)
  /* Set the level to start at to the argument */
  if (argc > 1)
    game.level = atoi(argv[1]);
#endif

  /* Load the first level */
  goto_level(&game);

  /* Draw the game screen */
  draw_screen(&game);

  fe_sleep(100);
  /* Start the game */
  do_game(&game);


  /* Clean up afterwards */
  fe_finalize();

#if defined(TARGET_SDL)
  return 0;
#endif
}
