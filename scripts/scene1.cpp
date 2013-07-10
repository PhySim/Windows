#include "headers/physim.h"
#include <iostream>
using namespace std;
SDL_Event event;
int main(int argc,char* args[])
{
	PHYSIM scene1((SDL_Rect){720,50});
	SDL_Surface* scr=SDL_SetVideoMode(720,480,32,SDL_SWSURFACE);

	SDL_Surface* dot=loadimage("images/dot.png");
	particle* particle1;
	particle1=new particle((vector){10,10,10},(vector){20,20,0},dot);
	particle1->addacc((vector){0,98,0});

	while(!scene1.ended)
	{
		//=================================initialisation
		scene1.initiateframe();
		//=================================

		//_________________________________

		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		ofstream fout("temp.txt",ios::app);
		fout<<scene1.runtime.elapse()<<"	"<<scene1.deltatime();
		fout.close();
		if(!particle1->globalcollision(scene1.deltatime()))
			particle1->integrate(scene1.deltatime());
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		particle1->display(scr);
		//.................................

		//---------------------------------termination
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT )
				{
		                scene1.ended=true;
		        }
		}
		scene1.terminateframe((SDL_Color){0,0xFF,0});
		if(scene1.currentframe()>10000)
			scene1.ended=true;
		//---------------------------------
	}
	delete particle1;
	return 0;
}
