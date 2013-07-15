/*
 * global_assets.hpp
 *
 *  Created on: Jul 15, 2013
 *      Author: Reuben
 */

#ifndef GLOBAL_ASSETS_HPP_
#define GLOBAL_ASSETS_HPP_

SDL_Rect scrdim;
SDL_Surface* scr;
short bpp;

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

void applysurface(SDL_Surface* image,vect pos=(vect){0,0,0} )
{
	if(scr==NULL)
		debugger.found("applysurface()","::scr is pointing to NULL");
	else
	{
		SDL_Rect temp={pos.x,pos.y};
		SDL_BlitSurface(image,NULL,scr,&temp);
	}
}

#endif /* GLOBAL_ASSETS_HPP_ */
