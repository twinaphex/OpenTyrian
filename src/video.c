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
#include "opentyr.h"
#include "palette.h"
#include "video.h"
#include "video_scale.h"
#include "surface.h"

#include <assert.h>
#include <stdbool.h>

bool fullscreen_enabled = false;

LR_Surface *VGAScreen, *VGAScreenSeg;
LR_Surface *VGAScreen2;
LR_Surface *game_screen;

static ScalerFunction scaler_function;

void init_video( void )
{
	if (SDL_WasInit(SDL_INIT_VIDEO))
		return;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
	{
		fprintf(stderr, "error: failed to initialize SDL video: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_WM_SetCaption("OpenTyrian", NULL);

   VGAScreen    = (LR_Surface*)calloc(1, sizeof(*VGAScreen));
   VGAScreen2   = (LR_Surface*)calloc(1, sizeof(*VGAScreen2));
   VGAScreenSeg = (LR_Surface*)calloc(1, sizeof(*VGAScreenSeg));
   game_screen  = (LR_Surface*)calloc(1, sizeof(*game_screen));

	VGAScreen->surf   = VGAScreenSeg->surf = SDL_CreateRGBSurface(SDL_SWSURFACE, vga_width, vga_height, 8, 0, 0, 0, 0);
	VGAScreen2->surf  = SDL_CreateRGBSurface(SDL_SWSURFACE, vga_width, vga_height, 8, 0, 0, 0, 0);
	game_screen->surf = SDL_CreateRGBSurface(SDL_SWSURFACE, vga_width, vga_height, 8, 0, 0, 0, 0);

	LR_FillRect(VGAScreen, NULL, 0);

	if (!init_scaler(scaler, fullscreen_enabled) &&  // try desired scaler and desired fullscreen state
	    !init_any_scaler(fullscreen_enabled) &&      // try any scaler in desired fullscreen state
	    !init_any_scaler(!fullscreen_enabled))       // try any scaler in other fullscreen state
	{
		fprintf(stderr, "error: failed to initialize any supported video mode\n");
		exit(EXIT_FAILURE);
	}
}

int can_init_scaler( unsigned int new_scaler, bool fullscreen )
{
   (void)fullscreen;

	if (new_scaler >= scalers_count)
		return false;
	
   return 16;
}

bool init_scaler( unsigned int new_scaler, bool fullscreen )
{
	int w = scalers[new_scaler].width,
	    h = scalers[new_scaler].height;
	int bpp = can_init_scaler(new_scaler, fullscreen);
	int flags = SDL_SWSURFACE | SDL_HWPALETTE | (fullscreen ? SDL_FULLSCREEN : 0);
	
	if (bpp == 0)
		return false;
	
	SDL_Surface *const surface = SDL_SetVideoMode(w, h, bpp, flags);
	
	if (surface == NULL)
	{
		fprintf(stderr, "error: failed to initialize %s video mode %dx%dx%d: %s\n", fullscreen ? "fullscreen" : "windowed", w, h, bpp, SDL_GetError());
		return false;
	}
	
	w = surface->w;
	h = surface->h;
	bpp = surface->format->BitsPerPixel;
	
	printf("initialized video: %dx%dx%d %s\n", w, h, bpp, fullscreen ? "fullscreen" : "windowed");
	
	scaler = new_scaler;
	fullscreen_enabled = fullscreen;
   scaler_function = scalers[scaler].scaler16;

	if (scaler_function == NULL)
	{
		assert(false);
		return false;
	}
	
	input_grab(input_grab_enabled);
	
	JE_showVGA();
	
	return true;
}

bool can_init_any_scaler( bool fullscreen )
{
	for (int i = scalers_count - 1; i >= 0; --i)
		if (can_init_scaler(i, fullscreen) != 0)
			return true;
	
	return false;
}

bool init_any_scaler( bool fullscreen )
{
	// attempts all scalers from last to first
	for (int i = scalers_count - 1; i >= 0; --i)
		if (init_scaler(i, fullscreen))
			return true;
	
	return false;
}

void deinit_video( void )
{
	SDL_FreeSurface(VGAScreenSeg->surf);
	SDL_FreeSurface(VGAScreen2->surf);
	SDL_FreeSurface(game_screen->surf);

   if (VGAScreenSeg)
      free(VGAScreenSeg);
   VGAScreen   = NULL;
   if (VGAScreen2)
      free(VGAScreen2);
   VGAScreen2  = NULL;
   if (game_screen)
      free(game_screen);
   game_screen = NULL;
	
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void JE_clr256( LR_Surface *screen)
{
	memset(screen->surf->pixels, 0, screen->surf->pitch * screen->surf->h);
}
void JE_showVGA( void ) { scale_and_flip(VGAScreen); }

void scale_and_flip( LR_Surface *src_surface )
{
   LR_Surface dst_surface;
	assert(src_surface->surf->format->BitsPerPixel == 8);
	
	dst_surface.surf = SDL_GetVideoSurface();
	
	assert(scaler_function != NULL);
	scaler_function(src_surface, &dst_surface);
	
	SDL_Flip(dst_surface.surf);
}

