#include "headers/physim.h"

using namespace std;
SDL_Event event;
int main(int argc,char* args[])
{
	ofstream fout("framelog.txt");
	PHYSIM scene((vect){960,512,2000});
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0xDD,0xDD,0xDD));
	SDL_Flip(scr);
	SDL_Surface* space=loadimage("images/bright space.jpg");
	if(space)
	{
		space=rotozoomSurface(space,0,0.5,0);
	}
	SDL_Surface* star=loadimage("images/star.png");
	//scene.gensphere(dot)->addvel(random((vect){-50,-50,0},(vect){50,50,0}));
	SDL_Delay(250);

	while(!scene.ended)
	{
		if(scene.frametimer.currentframe()==25)
		{
			scene.gensphere(loadimage("images/dot.png"),(vect){500,350,1000},pow(10,18));
		}
		if(scene.frametimer.currentframe()%50==0)
		{
			scene.gensphere(star,pow(10,10))->addvel(random((vect){-1000,-1000,0},(vect){1000,1000,0}));
		}
		//=================================initialisation
		scene.initiateframe();
		for(unsigned int i=1;i<scene.sphere.size();i++)
			scene.sphere[i]->newframe();
		//=================================

		//_________________________________user interaction
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT )
				{
		                scene.ended=true;
		        }
		}
		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		if(scene.frametimer.currentframe()>100)
			for(unsigned int i=0;i<scene.sphere.size();i++)
			{
				//scene.sphere[i]->addforce((vect){0,scene.sphere[i]->mass()*98,0});
					for(unsigned int j=0;j<scene.sphere.size();j++)
					{
						if(i!=j)
						{
							if(j>i&&scene.sphere[i]->mass()<pow(10,18)&&scene.sphere[j]->mass()<pow(10,18))
								scene.sphere[i]->collision(*scene.sphere[j]);
							scene.sphere[i]->gravity(scene.sphere[j]);
						}
					}
					if(1)//!scene.sphere[i]->globalcollision((void*)&scene,scene.frametimer.deltatime()))
					{
						scene.sphere[i]->integrate(scene.frametimer.deltatime());
					}
			}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		for(unsigned int i=0;i<scene.sphere.size()-1;i++)
		{
			if(!scene.sphere[i]->justcollided())
				scene.sphere[i]->display((void*)&scene);
		}
		//.................................

		//---------------------------------termination
		fout<<scene.frametimer.currentfps()<<"	"<<scene.frametimer.deltatime()<<"\n";
		scene.terminateframe(space);
		if(scene.frametimer.currentframe()>10000)
			scene.ended=true;
		//---------------------------------
	}
	if(star!=NULL)
		SDL_FreeSurface(dot);
	SDL_FreeSurface(space);
	return 0;
}
