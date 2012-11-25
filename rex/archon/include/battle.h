/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      battle.h
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   archon battle
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __BATTLE_H__
#define __BATTLE_H__

#include <game.h>
#include <playfield.h>

/**
 * Battle between two creatures
 *
 * @param game the game
 * @param tile the tile to battle on
 * @param light the light creature to battle with
 * @param dark the dark creature to battle with
 *
 * @return the creature that won the battle
 */
extern creature_t *battle_do(game_t *game, tile_t tile,
			     creature_t *light, creature_t *dark);

#endif /* !__BATTLE_H__ */
