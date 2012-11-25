/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      callbacks.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Game callbacks
 *
 * $Id: callbacks.c 331 2004-08-05 06:21:45Z ska $
 *
 ********************************************************************/
#include <vmgp.h>
#include <vmgputil.h> /* WaitTicks */
#include <vstream.h>

#include <boulder.h>

int callback_register(game_t *p_game,
		      void (*fn)(game_t *p_game, int id, void *p_priv),
		      void *p_priv)
{
  int i;

  assert(fn);

  for (i=0; i<N_CALLBACKS; i++)
    {
      if (!p_game->callbacks[i].fn)
	{
	  p_game->callbacks[i].fn = fn;
	  p_game->callbacks[i].p_priv = p_priv;
	  return i;
	}
    }

  return -1;
}

void callback_check_all(game_t *p_game)
{
  int i;

  for (i=0; i<N_CALLBACKS; i++)
    {
      if (p_game->callbacks[i].fn)
	p_game->callbacks[i].fn(p_game, i, p_game->callbacks[i].p_priv);
    }
}

void callback_unregister(game_t *p_game, int id)
{
  assert(id < N_CALLBACKS);
  p_game->callbacks[id].fn = NULL;
}
