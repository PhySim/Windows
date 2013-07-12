#include "headers/physim.h"
#include <iostream>
using namespace std;
SDL_Event event;
int N=1;
int main(int argc,char* args[])
{
	PHYSIM scene1((SDL_Rect){720,50});
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0xFF,0xFF,0xFF));
	SDL_Flip(scr);
	SDL_Surface* dot=loadimage("images/dot.png");
	PARTICLE temp(dot);
	scene1.particle.push_back(temp);
	scene1.particle.push_back(temp);
	scene1.particle.at(0).display(scr);
	SDL_Flip(scr);
	SDL_Delay(100);
	for(unsigned int i=0;i<1;i++)
	{
		scene1.particle.at(i).addacc((vect){0.011,0.098,0});
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
		fout<<scene1.runtime.elapse()<<"		"<<scene1.frametimer.currentfps()<<"		"<<scene1.frametimer.deltatime()<<"\n";
		if(scene1.frametimer.currentframe()>10)
			for(unsigned int i=0;i<1;i++)
			{
					if(!scene1.particle.at(i).globalcollision(scene1.frametimer.deltatime()))
					{
						scene1.particle.at(i).integrate(scene1.frametimer.deltatime());
					}
			}
		fout.close();
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		for(unsigned int i=0;i<1;i++)
			scene1.particle.at(i).display(scr);
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
	return 0;
}
