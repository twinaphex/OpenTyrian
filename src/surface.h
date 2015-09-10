/*
 * OpenTyrian: A modern cross-platform port of Tyrian
 * Copyright (C) 2007-2009  The OpenTyrian Development Team
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef SURFACE_H
#define SURFACE_H

#include "SDL.h"

typedef struct LR_Surface
{
   SDL_Surface *surf;
} LR_Surface;

#define RED_EXPAND   3
#define GREEN_EXPAND 2
#define BLUE_EXPAND  3

#define RED_SHIFT   11
#define GREEN_SHIFT 5
#define BLUE_SHIFT  0

#define SET_COLORFORMAT(r, g, b) ((r >> RED_EXPAND) << RED_SHIFT | (g >> GREEN_EXPAND) << GREEN_SHIFT | (b >> BLUE_EXPAND) << BLUE_SHIFT)

void LR_FillRect(LR_Surface *surface, SDL_Rect *rect, Uint8 color);

#endif
