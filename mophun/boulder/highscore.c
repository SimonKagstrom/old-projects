/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      highscore.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Highscore list implementation
 *
 * $Id: highscore.c 331 2004-08-05 06:21:45Z ska $
 *
 ********************************************************************/
#include <vmgp.h>

#include <boulder.h>

typedef struct
{
  int16_t  time;
  uint16_t level;
  char     name[NAME_LEN];
} highscore_ent_t;



void highscore_add(game_t *p_game, int16_t time, uint16_t level, const char *p_name)
{
}

void highscore_show_all(game_t *p_game)
{
}

void highscore_clear(game_t *p_game)
{
}
