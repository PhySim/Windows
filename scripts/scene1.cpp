#include "headers/physim.h"
#include <iostream>
using namespace std;
SDL_Event event;
int main(int argc,char* args[])
{
	PHYSIM scene1((SDL_Rect){720,50});
	SDL_Surface* scr=SDL_SetVideoMode(720,480,32,SDL_SWSURFACE);

	SDL_Surface* dot=loadimage("images/dot.png");
	particle** particles = new  particle*[10];
	for(int i=0;i<10;i++)
	{
		particles[i]=new particle((vect){10,10,0},(vect){20,20,0},dot);
		particles[i]->addacc((vect){0,98,0});
	}

	while(!scene1.ended)
	{
		//=================================initialisation
		scene1.initiateframe();
		//=================================

		//_________________________________

		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		ofstream fout("framelog.txt",ios::app);
		fout<<scene1.runtime.elapse()<<"		"<<scene1.frametimer.currentfps()<<"		"<<scene1.frametimer.deltatime()<<'\n';
		fout.close();
		for(int i=0;i<10;i++)
		{
			if(!particles[i]->globalcollision(scene1.frametimer.deltatime()))
				particles[i]->integrate(scene1.frametimer.deltatime());
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		for(int i=0;i<10;i++)
			particles[i]->display(scr);
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
		if(scene1.frametimer.currentframe()>10000)
			scene1.ended=true;
		//---------------------------------
	}
	delete [] particles;
	return 0;
}
