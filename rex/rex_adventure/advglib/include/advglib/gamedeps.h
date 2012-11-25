/*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      gamedeps.h
 * Description:   Prototypes for the game-dependent functions.
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
 * $Id: gamedeps.h 6264 2006-01-21 09:46:06Z ska $
 *
 ********************************************************************/
#ifndef GAMEDEPS_H
#define GAMEDEPS_H

/** @file
 *
 * Definitions of functions that needs to be implemented for each
 * game. This is currently only the event-handler, and it is likely to
 * stay that way.
 *
 * Each game has to implement the main()-function, which should
 * initialise the advglib library (advlib is initialised from advglib)
 * and start the game loop.
 */

#include "advgame.h"

/**
 * Called to dispatch events on the current scene. This is called once
 * in each round of the main game loop and when the player hits an
 * event-point, stops or enters a new scene. The possible events are:
 *
 * - <TT>ADVG_EVENT_NULL</TT>: No event. The eventhandler is <I>not</I>
 *     called on this event.
 * - <TT>ADVG_EVENT_FIRST_OBJECT</TT> ... <TT>ADVG_EVENT_LAST_OBJECT</TT>:
 *     An object event. This is given when the player has done
 *     something with an object, i.e. looked at it or used another
 *     object with it. The number corresponds to the object in
 *     question, and is between 1 and 16 (@c ADVG_EVENT_LAST_OBJECT).
 * - <TT>ADVG_EVENT_ENTER_SCENE</TT>: Enter scene event. The event
 *     handler is invoked with this event when the player enters a new
 *     scene.
 * - <TT>ADVG_EVENT_PLAYER</TT>: A player event. This is called when
 *     the player is referenced, like when an object is used on the
 *     player.
 * - <TT>ADVG_EVENT_TICK</TT>: A tick event. This is called on each
 *     round in the game-loop, and should handle things like checking
 *     collisions against the background (with @c adv_get_surroundings()).
 * - <TT>ADVG_EVENT_PLAYER_STOP</TT>: Player stops walking event. This
 *     is invoked when the player stops walking at some non-eventpoint
 *     place. The game should handle looking at the scene in general
 *     in that case.
 * - <TT>ADVG_EVENT_EXIT</TT>: Exit/quit event. This is called when
 *     the user has pressed the quit-button.
 * - <TT> >= ADVG_EVENT_FIRST_USER</TT>: User-specified event. This is
 *     called when the player does something on a event-point, i.e.
 *     when the player stands on a event-point and looks/uses
 *     something etc.
 * - <TT>ADVG_EVENT_EXIT_EVENT_POINT</TT>: Player exits an
 *     event-point. This bit is set when the player exits an
 *     event-point, i.e. other events can be generated apart from
 *     this. This must be checked with <TT>event &
 *     ADVG_EVENT_EXIT_EVENT_POINT</TT>.
 * - <TT>ADVG_EVENT_ENTER_EVENT_POINT</TT>: Player enters an
 *     event-point. This bit is set when the player enters an
 *     event-point, i.e. other events can be generated apart from
 *     this. This must be checked with <TT>event &
 *     ADVG_EVENT_ENTER_EVENT_POINT</TT>.
 *
 *
 * @param p_game the current game
 * @param event the event-number.
 *
 * @see adv_get_surroundings()
 * @see advg_init_scene_event()
 */
extern void advg_handle_event(advg_game_t *p_game, advg_event_t event);

/* In the future we might add something here, but maybe not ;-) */

#endif /* GAMEDEPS_H */
