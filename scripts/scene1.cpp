#include "headers/physim.h"

using namespace std;
SDL_Event event;
int main(int argc,char* args[])
{
	ofstream fout("framelog.txt");
	PHYSIM scene1((vect){998,755,50});
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0xDD,0xDD,0xDD));
	SDL_Flip(scr);
	SDL_Surface* skyline=loadimage("images/White Cube.jpg");
	SDL_Surface* dot=loadimage("images/dot.png");
	//scene1.genparticle(dot)->addvel(random((vect){-50,-50,0},(vect){50,50,0}));
	//SDL_Delay(500);

	while(!scene1.ended)
	{
		if(scene1.frametimer.currentframe()%50==0)
		{
			scene1.genparticle(loadimage("images/dot.png"))->addvel(random((vect){-10,-10,-5},(vect){10,10,5}));
		}
		//=================================initialisation
		scene1.initiateframe();
		for(unsigned int i=1;i<scene1.particle.size();i++)
			scene1.particle[i]->newframe();
		//=================================

		//_________________________________user interaction

		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		if(scene1.frametimer.currentframe()>10)
			for(unsigned int i=1;i<scene1.particle.size()-1;i++)
			{
				scene1.particle[i]->addforce((vect){0,98,0});
					for(unsigned int j=i+1;j<scene1.particle.size();j++)
					{
							scene1.particle[i]->collision(*scene1.particle[j],scene1.frametimer.deltatime());
					}
					if(!scene1.particle[i]->globalcollision((void*)&scene1,scene1.frametimer.deltatime()))
					{
						scene1.particle[i]->integrate(scene1.frametimer.deltatime());
					}
			}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		for(unsigned int i=1;i<scene1.particle.size()-1;i++)
			if(!scene1.particle[i]->justcollided())
				scene1.particle[i]->display((void*)&scene1);
		//.................................

		//---------------------------------termination
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT )
				{
		                scene1.ended=true;
		        }
		}
		fout<<scene1.frametimer.currentfps()<<"	"<<scene1.frametimer.deltatime()<<"\n";
		scene1.terminateframe(skyline);
		if(scene1.frametimer.currentframe()>10000)
			scene1.ended=true;
		//---------------------------------
	}
	if(dot!=NULL)
		SDL_FreeSurface(dot);
	SDL_FreeSurface(skyline);
	return 0;
}
