/*
 * global_assets.hpp
 *
 *  Created on: Jul 15, 2013
 *      Author: Reuben
 */

#ifndef GLOBAL_ASSETS_HPP_
#define GLOBAL_ASSETS_HPP_

#include <SDL/SDL_gfx-2.0.24/SDL_rotozoom.h>
#include <headers/debug.hpp>
#include <headers/framer.hpp>
#include <SDL/SDL.h>
#include <headers/vect.hpp>
#include "string.h"

const char* font_loc ="fonts";
const char* image_loc="images";
const char* audio_loc="audio";
const long double G=6.674*pow(10.0,-11);

short bpp;
TTF_Font* font;
unsigned int global_font_size=32,global_graphicstring_id=0;

/**
 * overloading of + operator to concatenate a string variable and a const char* variable
 */
char* operator+(string a,const char* b)
{
	string temp=a;
	temp.append(b);
	return (char*)a.c_str();
}

/**
 * function used to load an image from the specified location into an SDL_Surface pointer and return it
 */
SDL_Surface* loadimage(string filename)
{
	SDL_Surface* tex=IMG_Load(filename.c_str());
	if(tex!=NULL)
	{
		tex=SDL_DisplayFormat(tex);
		if(tex!=NULL)
		{
			SDL_SetColorKey(tex,SDL_SRCCOLORKEY,SDL_MapRGB(tex->format,0,0xFF,0xFF));
			if(tex==NULL)
				debugger.found("loadimage()","SDL_SetColorKey() failed");
		}
		else
			debugger.found("loadimage()","SDL_DisplayFormat() failed");
	}
		else
			debugger.found("loadimage()","IMG_Load() failed");
	return tex;
}
/**
 * function that copies an SDL_Surface passed to it and returns the SDL_Surface pointer to the newly created SDL_Surface
 */
SDL_Surface* copy_surface(SDL_Surface* source)
{
	return SDL_ConvertSurface(source, source->format, source->flags);
}

#endif /* GLOBAL_ASSETS_HPP_ */
