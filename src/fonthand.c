#include "opentyr.h"
#include "newshape.h"
#include "fonthand.h"

const JE_byte fontmap[136] = 	/* [33..168] */
{
	26,33,60,61,62,255,32,64,65,63,84,29,83,28,80,79,70,71,72,73,74,75,76,77,
	78,31,30,255,255,255,27,255,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
	20,21,22,23,24,25,68,82,69,255,255,255,34,35,36,37,38,39,40,41,42,43,44,45,46,
	47,48,49,50,51,52,53,54,55,56,57,58,59,66,81,67,255,255,

	86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,
	107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,
	125,126
};

const JE_integer _Partshade  = 0;
const JE_integer _Fullshade  = 1;
const JE_integer _Darken     = 2;
const JE_integer _Trick      = 3;
const JE_integer _Noshade    = 255;

/* shape constants included in newshape.h */

JE_integer DefaultBrightness = -3;
JE_byte TextGlowBrightness = 6;


void JE_Dstring(JE_word x, JE_word y, JE_string s, JE_byte font)
{
	JE_byte a, b;
	JE_boolean bright = 0; /*false*/

	for(a = 0; s[a] != 0; a++)
	{
		b = s[a];

		if((b > 32) && (b < 126))
		{
			if(fontmap[b-33] != 255)
			{
				JE_NewDrawCShapeDarken((*shapearray)[font][fontmap[b-33]], shapex[font][fontmap[b-33]], shapey[font][fontmap[b-33]], x + 2, y + 2);
				JE_NewDrawCShapeBright((*shapearray)[font][fontmap[b-33]], shapex[font][fontmap[b-33]], shapey[font][fontmap[b-33]], x, y, 15, DefaultBrightness + (bright << 1));

				x += shapex[font][fontmap[b-33]] + 1;
			}
		} else
			if(b == 32)
				x += 6;
			else
				if(b == 126)
					bright = !bright;
	}
}

/*Intended for Font drawing - maximum X size is 255*/
void JE_NewDrawCShapeBright(JE_byte *shape, JE_word xsize, JE_word ysize, JE_word x, JE_word y, JE_byte filter, JE_shortint brightness)
{
	JE_word xloop = 0, yloop = 0;
	JE_byte *p;	/* shape pointer */
	unsigned char *s;	/* screen pointer, 8-bit specific */

	SDL_LockSurface(tempscreenseg);
	s = (unsigned char *)((int)tempscreenseg->pixels + y * tempscreenseg->pitch + x * tempscreenseg->format->BytesPerPixel);

	filter <<= 4;

	for(p = shape; yloop < ysize; p++)
	{
		switch(*p)
		{
		case 255:	/* p transparent pixels */
			p++;
			s += *p; xloop += *p;
			break;
		case 254:	/* next y */
			s -= xloop; xloop = 0;
			s += tempscreenseg->w; yloop++;
			break;
		case 253:	/* 1 transparent pixel */
			s++; xloop++;
			break;
		default:	/* set a pixel */
			*s = ((*p & 0x0f) | filter) + brightness;
			s++; xloop++;
		}
		if(xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += tempscreenseg->w; yloop++;
		}
	}

	SDL_UnlockSurface(tempscreenseg);
}

/*Intended for Font drawing - maximum X size is 255*/
void JE_NewDrawCShapeShadow(JE_byte *shape, JE_word xsize, JE_word ysize, JE_word x, JE_word y)
{
	JE_word xloop = 0, yloop = 0;
	JE_byte *p;	/* shape pointer */
	unsigned char *s;	/* screen pointer, 8-bit specific */

	SDL_LockSurface(tempscreenseg);
	s = (unsigned char *)((int)tempscreenseg->pixels + y * tempscreenseg->pitch + x * tempscreenseg->format->BytesPerPixel);

	for(p = shape; yloop < ysize; p++)
	{
		switch(*p)
		{
		case 255:	/* p transparent pixels */
			p++;
			s += *p; xloop += *p;
			break;
		case 254:	/* next y */
			s -= xloop; xloop = 0;
			s += tempscreenseg->w; yloop++;
			break;
		case 253:	/* 1 transparent pixel */
			s++; xloop++;
			break;
		default:	/* set a pixel */
			*s = 0;
			s++; xloop++;
		}
		if(xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += tempscreenseg->w; yloop++;
		}
	}

	SDL_UnlockSurface(tempscreenseg);
}

/*Intended for Font drawing - maximum X size is 255*/
void JE_NewDrawCShapeDarken(JE_byte *shape, JE_word xsize, JE_word ysize, JE_word x, JE_word y)
{
	JE_word xloop = 0, yloop = 0;
	JE_byte *p;	/* shape pointer */
	unsigned char *s;	/* screen pointer, 8-bit specific */

	SDL_LockSurface(tempscreenseg);
	s = (unsigned char *)((int)tempscreenseg->pixels + y * tempscreenseg->pitch + x * tempscreenseg->format->BytesPerPixel);

	for(p = shape; yloop < ysize; p++)
	{
		switch(*p)
		{
		case 255:	/* p transparent pixels */
			p++;
			s += *p; xloop += *p;
			break;
		case 254:	/* next y */
			s -= xloop; xloop = 0;
			s += tempscreenseg->w; yloop++;
			break;
		case 253:	/* 1 transparent pixel */
			s++; xloop++;
			break;
		default:	/* set a pixel */
			*s = ((*p & 0x0f) >> 1) + (*p & 0xf0);
			s++; xloop++;
		}
		if(xloop == xsize)
		{
			s -= xloop; xloop = 0;
			s += tempscreenseg->w; yloop++;
		}
	}

	SDL_UnlockSurface(tempscreenseg);
}

void JE_Outtext(JE_word x, JE_word y, JE_string s, JE_byte colorbank, JE_shortint brightness)
{
	JE_byte a, b;
	JE_byte bright = 0;
	for(a = 0; s[a] != 0; a++)
	{
		b = s[a];

		if((b > 32) && (b < 126) && (fontmap[b-33] != 255) && ((*shapearray)[_TinyFont][fontmap[b-33]] != NULL))
		{
			if (brightness >= 0)
				JE_NewDrawCShapeBright((*shapearray)[_TinyFont][fontmap[b-33]], shapex[_TinyFont][fontmap[b-33]], shapey[_TinyFont][fontmap[b-33]], x, y, colorbank, brightness + bright);
			else
				JE_NewDrawCShapeShadow((*shapearray)[_TinyFont][fontmap[b-33]], shapex[_TinyFont][fontmap[b-33]], shapey[_TinyFont][fontmap[b-33]], x, y);

			x += shapex[_TinyFont][fontmap[b-33]] + 1;
		} else
			if(b == 32)
				x += 6;
			else
				if(b == 126)
				{
					if(bright > 0)
						bright = 0;
					else
						bright = 4;
				}
	}
	if(brightness >= 0)
		tempscreenseg = vgascreenseg;
}