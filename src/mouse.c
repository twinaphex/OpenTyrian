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
#include "keyboard.h"
#include "nortvars.h"
#include "sprite.h"
#include "video.h"
#include "vga256d.h"

bool has_mouse = false;
bool mouse_has_three_buttons = true;

JE_word lastMouseX, lastMouseY;
JE_byte mouseCursor;
JE_word mouseX, mouseY, mouseButton;
JE_word mouseXB, mouseYB;

JE_byte mouseGrabShape[24 * 28];                 /* [1..24*28] */

void JE_drawShapeTypeOne( JE_word x, JE_word y, JE_byte *shape )
{
	JE_word xloop = 0, yloop = 0;
	JE_byte *p = shape; /* shape pointer */
	Uint8 *s;   /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	s = (Uint8 *)VGAScreen->surf->pixels;
	s += y * VGAScreen->surf->pitch + x;

	s_limit = (Uint8 *)VGAScreen->surf->pixels;
	s_limit += VGAScreen->surf->h * VGAScreen->surf->pitch;

	for (yloop = 0; yloop < 28; yloop++)
	{
		for (xloop = 0; xloop < 24; xloop++)
		{
			if (s >= s_limit) return;
			*s = *p;
			s++; p++;
		}
		s -= 24;
		s += VGAScreen->surf->pitch;
	}
}

void JE_grabShapeTypeOne( JE_word x, JE_word y, JE_byte *shape )
{
	JE_word xloop = 0, yloop = 0;
	JE_byte *p = shape; /* shape pointer */
	Uint8 *s;   /* screen pointer, 8-bit specific */
	Uint8 *s_limit; /* buffer boundary */

	s = (Uint8 *)VGAScreen->surf->pixels;
	s += y * VGAScreen->surf->pitch + x;

	s_limit = (Uint8 *)VGAScreen->surf->pixels;
	s_limit += VGAScreen->surf->h * VGAScreen->surf->pitch;

	for (yloop = 0; yloop < 28; yloop++)
	{
		for (xloop = 0; xloop < 24; xloop++)
		{
			if (s >= s_limit) return;
			*p = *s;
			s++; p++;
		}
		s -= 24;
		s += VGAScreen->surf->pitch;
	}
}

void JE_mouseStart( void )
{
}

void JE_mouseReplace( void )
{
}

