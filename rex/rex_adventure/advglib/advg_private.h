/*********************************************************************
 *
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      advg_private.h
 * Description:   Private definitions for the library (i.e.
 *                functions that are not visible for the user, but
 *                still exported by the library).
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
 * $Id: advg_private.h 3175 2005-07-07 13:32:32Z ska $
 *
 ********************************************************************/
#ifndef ADVG_PRIVATE_H
#define ADVG_PRIVATE_H

/* Handle a press on the touch-screen */
extern advg_event_t _advg_handle_press(advg_game_t *p_game, fe_point_t *p_point);

/* Handle player movement. */
extern advg_event_t _advg_handle_move(advg_game_t *p_game);

/* Reset game action */
extern void _advg_reset_action(advg_game_t *p_game);


#endif /* ADVG_PRIVATE_H */
