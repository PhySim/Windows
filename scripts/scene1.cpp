#include "headers/physim.h"
#include <iostream>
#include <vector>
using namespace std;
SDL_Event event;
int y=9;
int main(int argc,char* args[])
{
	PHYSIM scene1((SDL_Rect){720,50});
	SDL_Surface* scr=SDL_SetVideoMode(720,480,32,SDL_SWSURFACE);

	SDL_Surface* dot=loadimage("images/dot.png");
	//particle* particl=new particle[10];
	vector<particle> part (y, particle((vect){10,10,10},(vect){20,20,0},dot));
		//particle[0]=new particle((vector){10,10,10},(vector){20,20,0},dot);
	part[0].addacc((vect){0,980,0});

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
		if(!part[0].globalcollision(scene1.deltatime()))
			part[0].integrate(scene1.deltatime());
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		part[0].display(scr);
		SDL_Delay(100);
		//.................................

		//---------------------------------termination
		while( SDL_PollEvent( &event ) )
		{
		//	particle[0]->trial(event);
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
	//delete particle[0];
	return 0;
}
