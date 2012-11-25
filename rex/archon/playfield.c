/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      playfield.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Playfield impl.
 *
 * $Id:$
 *
 ********************************************************************/
#include <string.h>
#include <assert.h>
#include <playfield.h>

#define max(x,y) ( (x) > (y) ? (x) : (y) )

#define abs(x) ( (x) < 0 ? -(x) : (x) )

/*
 *      0 1 2 3 4 5 6 7 8
 * 0    V A - - p - - M Be   Banshee
 * 1    L K - - - - - G T
 * 2    U K - - - - - G Ba   Basilisk
 * 3    D K - - - - - G Sh   Shapeshifter
 * 4    W K - - p - - G Sc   Sorceress
 * 5    P K - - - - - G D
 * 6    U K - - - - - G Ba
 * 7    L K - - - - - G T    L - Golem
 * 8    V A - - p - - M Bs
 */

#define D {NULL, PF_DARK}
#define L {NULL, PF_LIGHT}
#define m {NULL, PF_MAGENTA}
static tile_t initial_field[ PF_W * PF_H ] =
{
  D,L,D,m,m,m,L,D,L,
  L,D,m,L,m,D,m,L,D,
  D,m,L,D,m,L,D,m,L,
  m,L,D,L,m,D,L,D,m,
  L,m,m,m,m,m,m,m,D,
  m,L,D,L,m,D,L,D,m,
  D,m,L,D,m,L,D,m,L,
  L,D,m,L,m,D,m,L,D,
  D,L,D,m,m,m,L,D,L,
};
#undef D
#undef L
#undef m

typedef struct
{
  point_t pos;
  uint8_t type;
  uint8_t dark;
} creature_init_t;

static uint8_t creature_movement[ 8 * 2 ] =
{
  3, 3, /* valkyire, banshee */
  3, 3, /* golem, troll */
  3, 3, /* unicorn, basilisk */
  3, 3, /* dinji, shapeshifter */
  3, 3, /* wizard, sorceress */
  3, 3, /* phoenix, dragon */
  3, 3, /* archer, manticore */
  3, 3, /* knight, goblin */
};

static uint8_t creature_power[ 8 * 2 ] =
{
  1, 1, /* valkyire, banshee */
  1, 1, /* golem, troll */
  1, 1, /* unicorn, basilisk */
  1, 1, /* dinji, shapeshifter */
  1, 1, /* wizard, sorceress */
  1, 1, /* phoenix, dragon */
  1, 1, /* archer, manticore */
  1, 1, /* knight, goblin */
};

static uint8_t creature_flying[ 8 * 2 ] =
{
  1, 1,  /* valkyire, banshee */
  0, 0,	 /* golem, troll */
  0, 0,	 /* unicorn, basilisk */
  1, 1,	 /* dinji, shapeshifter */
  0, 0,	 /* wizard, sorceress */
  1, 1,	 /* phoenix, dragon */
  0, 0,	 /* archer, manticore */
  0, 0,	 /* knight, goblin */
};

#define LIGHT(x,y,type) {{x, y}, type, 0}
#define DARK(x,y,type)  {{x, y}, type, 1}
static creature_init_t creature_positions[] =
{
  LIGHT(0, 0, CR_VALKYIRE),  LIGHT(1, 0, CR_ARCHER),        DARK(7, 0, CR_MANTICORE), DARK(8, 0, CR_BANSHEE),
  LIGHT(0, 1, CR_GOLEM),     LIGHT(1, 1, CR_KNIGHT),        DARK(7, 1, CR_GOBLIN),    DARK(8, 1, CR_TROLL),
  LIGHT(0, 2, CR_UNICORN),   LIGHT(1, 2, CR_KNIGHT),        DARK(7, 2, CR_GOBLIN),    DARK(8, 2, CR_BASILISK),
  LIGHT(0, 3, CR_DINJI),     LIGHT(1, 3, CR_KNIGHT),        DARK(7, 3, CR_GOBLIN),    DARK(8, 3, CR_SHAPESHIFTER),
  LIGHT(0, 4, CR_WIZARD),    LIGHT(1, 4, CR_KNIGHT),        DARK(7, 4, CR_GOBLIN),    DARK(8, 4, CR_SORCERESS),
  LIGHT(0, 5, CR_PHOENIX),   LIGHT(1, 5, CR_KNIGHT),        DARK(7, 5, CR_GOBLIN),    DARK(8, 5, CR_DRAGON),
  LIGHT(0, 6, CR_UNICORN),   LIGHT(1, 6, CR_KNIGHT),        DARK(7, 6, CR_GOBLIN),    DARK(8, 6, CR_BASILISK),
  LIGHT(0, 7, CR_GOLEM),     LIGHT(1, 7, CR_KNIGHT),        DARK(7, 7, CR_GOBLIN),    DARK(8, 7, CR_TROLL),
  LIGHT(0, 8, CR_VALKYIRE),  LIGHT(1, 8, CR_ARCHER),        DARK(7, 8, CR_MANTICORE), DARK(8, 8, CR_BANSHEE),
};
#undef LIGHT
#undef DARK

int playfield_is_powerpoint(point_t pt)
{
  if ( (pt.x == 4 && pt.y == 0) ||
       (pt.x == 4 && pt.y == 4) ||
       (pt.x == 4 && pt.y == 8) ||
       (pt.x == 0 && pt.y == 4) ||
       (pt.x == 8 && pt.y == 4) )
    return 1;
  return 0;
}

creature_t *playfield_get_creature(playfield_t *pf, point_t pt)
{
  tile_t t = playfield_get_tile(pf, pt);

  return t.piece;
}

void playfield_set_creature(playfield_t *pf, creature_t *cr,
			    point_t where)
{
  pf->field[ where.y * PF_W + where.x ].piece = cr;
}


int playfield_can_move(playfield_t *pf, point_t *out,
		       point_t start, point_t end)
{
  creature_t *cr_s = playfield_get_creature(pf, start);
  int dx = point_get_dx(start, end);
  int dy = point_get_dy(start, end);
  int dist = max( abs(dx), abs(dy) );
  point_t cur;

  assert(start.x >= 0 && start.x <= PF_W &&
	 start.y >= 0 && start.y <= PF_H);

  /* Not moving? */
  if (dist == 0)
    return NOK;

  /* Within the field? */
  if ( !(end.x >= 0 && end.x <= PF_W &&
	 end.y >= 0 && end.y <= PF_H) )
    return NOK;

  /* Something is very wrong! */
  if ( !cr_s )
    return NOK;

  /* Distance ok? */
  if ( cr_s->movement < dist )
    return NOK;

  /* Moving diagonally? (also check dir) */
  if (dx && dy)
    return NOK;

  cur = start;
  do
    {
      creature_t *cr_d;

      cur = point_add_dx_dy(cur, dx, dy);
      cr_d = playfield_get_creature(pf, cur);

      if ( cr_d )
	{
	  /* Moving onto the same color as self? */
	  if ( cr_d->dark == cr_s->dark )
	    {
	      if ( !cr_s->flying )
		return NOK;
	    }
	  else
	    {
	      *out = cur;
	      return BATTLE;
	    }
	}
    } while (cur.x != end.x || cur.y != end.y);
  *out = end;
  return OK;
}

static void creature_do_move(creature_t *cr, point_t dst)
{
  cr->pos = dst;
}

void playfield_do_move(playfield_t *pf,
		       point_t start, point_t dst)
{
  tile_t *s = &playfield_get_tile(pf, start);
  tile_t *d = &playfield_get_tile(pf, dst);

  /* Otherwise we should have a battle */
  assert( s->piece &&
	  d->piece == NULL);

  d->piece = s->piece;
  s->piece = NULL;

  creature_do_move(d->piece, dst);
}

void playfield_remove_creature(playfield_t *pf,
			       point_t where)
{
  tile_t *s = &playfield_get_tile(pf, where);

  assert(s->piece != NULL);

  s->piece = NULL;
}

void creature_init(creature_t *cr, int dark,
		   uint8_t type)
{
  int add = 0;

  if (dark)
    add = 1;

  cr->type = type;
  cr->dark = dark;
  cr->flying = creature_flying[ type * 2 + add ];
  cr->movement = creature_movement[ type * 2 + add ];
  cr->power = creature_power[ type * 2 + add ];
}

static void creature_add(playfield_t *pf,
			 int dark, uint8_t type, point_t where)
{
  creature_t *cr = &pf->pieces[pf->n];

  creature_init(cr, dark, type);
  cr->pos = where;

  pf->field[where.y * PF_W + where.x].piece = cr;
  pf->n++;
}

void playfield_init(playfield_t *pf)
{
  int i;

  pf->n = 0;

  memcpy(&pf->field, initial_field, sizeof(tile_t) * PF_W * PF_H );
  for (i = 0; i < N_PIECES; i++)
    {
      creature_init_t *p = &creature_positions[i];

      creature_add(pf, p->dark, p->type, p->pos);
    }
}
