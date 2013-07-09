#include "headers/physim.h"
#include <iostream>
using namespace std;
SDL_Event event;
int main(int argc,char* args[])
{
	int frame=0;
	PHYSIM psm((SDL_Rect){720,50});
	SDL_Surface* scr=SDL_SetVideoMode(720,480,32,SDL_SWSURFACE);
	particle* particle1;
	particle1=new particle("images/dot.png");
	particle1->addacc((vector){0,0.98,0});
			SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0,0xFF,0xFF));
	while(!psm.ended)
	{
		//=================================initialisation
		SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0,0xFF,0xFF));
		//=================================

		//_________________________________

		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		particle1->integrate();
		particle1->collision();
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

		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT )
				{
		                psm.ended=true;
		        }
		}

		//---------------------------------
	}
	delete particle1;
	return 0;
}
