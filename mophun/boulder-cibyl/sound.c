/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      sound.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Sound functionality
 *
 * $Id: sound.c 9085 2006-07-19 10:41:58Z ska $
 *
 ********************************************************************/
#include <vmgp.h>

#include <boulder.h>

void sound_play(game_t *p_game, sound_type_t which)
{
  if (!p_game->conf.sound)
    return;
  switch(which)
    {
    case EXPLOSION:
      /* FIXME! */
      vBeep(200, 30);
      break;
    case DIAMOND_TAKEN:
      /* FIXME! */
      vBeep(2000, 60);
      break;
    default:
      break;
    }
}
