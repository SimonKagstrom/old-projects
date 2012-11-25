/*********************************************************************
 * Copyright (C) 2002,  Simon Kagstrom
 *
 * Filename:      advg_rex_lib.h
 * Description:   Definitions for the REX-library.
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
 * $Id: advg_rex_lib.h 6264 2006-01-21 09:46:06Z ska $
 *
 *********************************************************************/
#ifndef ADVG_REX_LIB_H
#define ADVG_REX_LIB_H

#define ADV_INIT_SCENE           1
#define ADV_LOAD_SCENE           2
#define ADV_MASK_OBJECT          3
#define ADV_MASK_BACKGROUND      4
#define ADV_DRAW_SCENE           5
#define ADV_GET_SURROUNDINGS     6

#define ADVG_INIT_GAME           7
#define ADVG_LOAD_MESSAGES       8
#define ADVG_INIT_SCENE_EVENT    9
#define ADVG_DISPLAY_MESSAGE    10
#define ADVG_DRAW_OBJECTS       11
#define ADVG_GOTO_SCENE         12
#define ADVG_DRAW_SCREEN        13
#define _ADVG_HANDLE_MOVE       14
#define _ADVG_HANDLE_PRESS      15
#define _ADVG_RESET_ACTION      16

#ifndef ADVG_SIG
/* Can be defined in the Makefile, in which case this is ignored */
# define ADVG_SIG "ADV"
#endif /* ADVG_SIG */

#endif /* ADVG_REX_LIB_H */
