#include "headers/physim.h"
#include <iostream>
using namespace std;
int main(int argc,char* args[])
{
	SDL_Rect dim={720,480,32};
	PHYSIM psm(NULL,dim,32);
	SDL_FillRect(psm.scr,&psm.scr->clip_rect,SDL_MapRGB(psm.scr->format,0,0xFF,0xFF));
	while(!psm.ended)
	{
		//=================================initialisation

		//=================================

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering

		//.................................

		//---------------------------------termination

		//---------------------------------
	}
	return 0;
}
