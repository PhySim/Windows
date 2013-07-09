#include "headers/physim.h"
#include <iostream>
using namespace std;
int main(int argc,char* args[])
{
	PHYSIM psm((SDL_Rect){720,50});
	SDL_Surface* scr=SDL_SetVideoMode(720,480,32,SDL_SWSURFACE);
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0,0xFF,0xFF));

	vector pos={10,10};
	vector dim=pos;
	particle particle1(pos,dim,"images/dot.png");
	while(1)
	{
		//=================================initialisation

		//=================================

		//_________________________________

		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		particle1.display(scr);
		//.................................

		//---------------------------------termination
		SDL_Flip(scr);
		//---------------------------------
	}
	SDL_Delay(1000);
	return 0;
}
