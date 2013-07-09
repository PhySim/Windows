#include "headers/physim.h"
#include <iostream>
using namespace std;
int main(int argc,char* args[])
{
	PHYSIM psm((SDL_Rect){720,480});
	SDL_FillRect(psm.scr,&psm.scr->clip_rect,SDL_MapRGB(psm.scr->format,0,0xFF,0xFF));
	psm.error->found("main()","Testing debugger class");
	while(0)
	{
		//=================================initialisation

		//=================================

		//_________________________________

		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering

		//.................................

		//---------------------------------termination

		//---------------------------------
	}
	Sleep(1000);
	return 0;
}
