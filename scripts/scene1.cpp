#include "headers/physim.h"

using namespace std;
SDL_Event event;
int main(int argc,char* args[])
{
	ofstream fout("framelog.txt");
	PHYSIM scene1((vect){998,500,2000});
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0xDD,0xDD,0xDD));
	SDL_Flip(scr);
	SDL_Surface* skyline=loadimage("images/skyline.gif");
	if(skyline)
	{
		skyline=rotozoomSurface(skyline,0,2,0);
	}
	SDL_Surface* dot=loadimage("images/dot.png");
	//scene1.gensphere(dot)->addvel(random((vect){-50,-50,0},(vect){50,50,0}));
	SDL_Delay(250);

	while(!scene1.ended)
	{
		if(scene1.frametimer.currentframe()==25)
		{
			scene1.gensphere(loadimage("images/dot.png"),(vect){500,350,1000},pow(10,18));
		}
		if(scene1.frametimer.currentframe()%50==0)
		{
			scene1.gensphere(loadimage("images/dot.png"),pow(10,10))->addvel(random((vect){-1000,-1000,0},(vect){1000,1000,0}));
		}
		//=================================initialisation
		scene1.initiateframe();
		for(unsigned int i=1;i<scene1.sphere.size();i++)
			scene1.sphere[i]->newframe();
		//=================================

		//_________________________________user interaction
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT )
				{
		                scene1.ended=true;
		        }
		}
		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		if(scene1.frametimer.currentframe()>100)
			for(unsigned int i=0;i<scene1.sphere.size();i++)
			{
				//scene1.sphere[i]->addforce((vect){0,scene1.sphere[i]->mass()*98,0});
					for(unsigned int j=0;j<scene1.sphere.size();j++)
					{
						if(i!=j)
						{
							if(j>i&&scene1.sphere[i]->mass()<pow(10,18)&&scene1.sphere[j]->mass()<pow(10,18))
								scene1.sphere[i]->collision(*scene1.sphere[j]);
							scene1.sphere[i]->gravity(scene1.sphere[j]);
						}
					}
					if(1)//!scene1.sphere[i]->globalcollision((void*)&scene1,scene1.frametimer.deltatime()))
					{
						scene1.sphere[i]->integrate(scene1.frametimer.deltatime());
					}
			}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		for(unsigned int i=0;i<scene1.sphere.size()-1;i++)
		{
			if(!scene1.sphere[i]->justcollided())
				scene1.sphere[i]->display((void*)&scene1);
		}
		//.................................

		//---------------------------------termination
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
