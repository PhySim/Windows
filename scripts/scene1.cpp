#include "headers/physim.h"
#include <iostream>
using namespace std;
SDL_Event event;
int N=1;
int main(int argc,char* args[])
{
	ofstream fout("framelog.txt");
	PHYSIM scene1((SDL_Rect){0,0,680,383});
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0xDD,0xDD,0xDD));
	SDL_Flip(scr);
	SDL_Surface* skyline=loadimage("images/339894_1236059844_medium.jpg");
	SDL_Surface* dot=loadimage("images/dot.png");
	PARTICLE* temp=new PARTICLE(dot);
	vector<PARTICLE*> particle;
	particle.push_back(new PARTICLE(dot));
	particle.at(0)->addacc((vect){0,980,0});
	particle[0]->display(scr);
	SDL_Flip(scr);
	SDL_Delay(1000);

	while(!scene1.ended)
	{
		if(scene1.frametimer.currentframe()%100==0&&scene1.frametimer.currentframe())
		{
			temp=new PARTICLE(dot);
			particle.push_back(temp);
			particle.back()->addacc((vect){0,980,0});
			N++;
		}
		//=================================initialisation
		scene1.initiateframe();
		//=================================

		//_________________________________

		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		if(scene1.frametimer.currentframe()>25)
			for(int i=0;i<N;i++)
			{
					if(!particle[i]->globalcollision(scene1.frametimer.deltatime()))
					{
						particle[i]->integrate(scene1.frametimer.deltatime());
					}
			}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		for(int i=0;i<N;i++)
			if(!particle[i]->justcollided())
				particle[i]->display(scr);
		//.................................

		//---------------------------------termination
		while( SDL_PollEvent( &event ) )
		{
			if( event.type == SDL_QUIT )
				{
		                scene1.ended=true;
		        }
		}
		fout<<scene1.frametimer.currentfps()<<"	"<<scene1.frametimer.remainingfreetime()<<"\n";
		scene1.terminateframe(skyline);
		if(scene1.frametimer.currentframe()>10000)
			scene1.ended=true;
		//---------------------------------
	}
		fout.close();
	remove("temp.txt");
	return 0;
}
