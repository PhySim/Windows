#include "headers/physim.h"
#include <iostream>
using namespace std;
int main(int argc,char* args[])
{
	int frame=0;
	PHYSIM psm((SDL_Rect){720,50});
	SDL_Surface* scr=SDL_SetVideoMode(720,480,32,SDL_SWSURFACE);
	particle* particle1;
	while(!psm.ended)
	{
		//=================================initialisation
		particle1=new particle("images/dot.png");
		SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0,0xFF,0xFF));
		//=================================

		//_________________________________

		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		particle1->display(scr);
		//.................................

		//---------------------------------termination
		SDL_Flip(scr);
		SDL_Delay(50);
		frame++;
		if(frame>1000)
			psm.ended=true;
		delete particle1;
		//---------------------------------
	}
	SDL_Delay(1000);
	return 0;
}
