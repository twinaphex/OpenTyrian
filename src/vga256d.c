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
#include "config.h" // For fullscreen stuff
#include "keyboard.h"
#include "opentyr.h"
#include "palette.h"
#include "vga256d.h"
#include "video.h"

#include "surface.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

void JE_pix( LR_Surface *surface, int x, int y, JE_byte c )
{
	/* Bad things happen if we don't clip */
	if (x <  surface->surf->pitch && y <  surface->surf->h)
	{
		Uint8 *vga = surface->surf->pixels;
		vga[y * surface->surf->pitch + x] = c;
	}
}

void JE_pix3( LR_Surface *surface, int x, int y, JE_byte c )
{
	/* Originally impemented as several direct accesses */
	JE_pix(surface, x, y, c);
	JE_pix(surface, x - 1, y, c);
	JE_pix(surface, x + 1, y, c);
	JE_pix(surface, x, y - 1, c);
	JE_pix(surface, x, y + 1, c);
}

void JE_rectangle( LR_Surface *surface, int a, int b, int c, int d, int e ) /* x1, y1, x2, y2, color */
{
	if (a < surface->surf->pitch && b < surface->surf->h &&
	    c < surface->surf->pitch && d < surface->surf->h)
	{
		Uint8 *vga = surface->surf->pixels;
		int i;

		/* Top line */
		memset(&vga[b * surface->surf->pitch + a], e, c - a + 1);

		/* Bottom line */
		memset(&vga[d * surface->surf->pitch + a], e, c - a + 1);

		/* Left line */
		for (i = (b + 1) * surface->surf->pitch + a; i < (d * surface->surf->pitch + a); i += surface->surf->pitch)
		{
			vga[i] = e;
		}

		/* Right line */
		for (i = (b + 1) * surface->surf->pitch + c; i < (d * surface->surf->pitch + c); i += surface->surf->pitch)
		{
			vga[i] = e;
		}
	} else {
		printf("!!! WARNING: Rectangle clipped: %d %d %d %d %d\n", a, b, c, d, e);
	}
}

void fill_rectangle_xy( LR_Surface *surface, int x, int y, int x2, int y2, Uint8 color )
{
	SDL_Rect rect = { x, y, x2 - x + 1, y2 - y + 1 };
	SDL_FillRect(surface->surf, &rect, color);
}

void JE_barShade( LR_Surface *surface, int a, int b, int c, int d ) /* x1, y1, x2, y2 */
{
	if (a < surface->surf->pitch && b < surface->surf->h &&
	    c < surface->surf->pitch && d < surface->surf->h)
	{
		Uint8 *vga = surface->surf->pixels;
		int i, j, width;

		width = c - a + 1;

		for (i = b * surface->surf->pitch + a; i <= d * surface->surf->pitch + a; i += surface->surf->pitch)
		{
			for (j = 0; j < width; j++)
			{
				vga[i + j] = ((vga[i + j] & 0x0F) >> 1) | (vga[i + j] & 0xF0);
			}
		}
	} else {
		printf("!!! WARNING: Darker Rectangle clipped: %d %d %d %d\n", a,b,c,d);
	}
}

void JE_barBright( LR_Surface *surface, int a, int b, int c, int d ) /* x1, y1, x2, y2 */
{
	if (a < surface->surf->pitch && b < surface->surf->h &&
	    c < surface->surf->pitch && d < surface->surf->h)
	{
		Uint8 *vga = surface->surf->pixels;
		int i, j, width;

		width = c-a+1;

		for (i = b * surface->surf->pitch + a; i <= d * surface->surf->pitch + a; i += surface->surf->pitch)
		{
			for (j = 0; j < width; j++)
			{
				JE_byte al, ah;
				al = ah = vga[i + j];

				ah &= 0xF0;
				al = (al & 0x0F) + 2;

				if (al > 0x0F)
				{
					al = 0x0F;
				}

				vga[i + j] = al + ah;
			}
		}
	} else {
		printf("!!! WARNING: Brighter Rectangle clipped: %d %d %d %d\n", a,b,c,d);
	}
}

void draw_segmented_gauge( LR_Surface *surface, int x, int y, Uint8 color, uint segment_width, uint segment_height, uint segment_value, uint value )
{
	assert(segment_width > 0 && segment_height > 0);

	const uint segments = value / segment_value,
	           partial_segment = value % segment_value;

	for (uint i = 0; i < segments; ++i)
	{
		fill_rectangle_hw(surface, x, y, segment_width, segment_height, color + 12);
		x += segment_width + 1;
	}
	if (partial_segment > 0)
		fill_rectangle_hw(surface, x, y, segment_width, segment_height, color + (12 * partial_segment / segment_value));
}

