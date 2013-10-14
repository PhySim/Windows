#include "headers/physim.h"

using namespace std;
SDL_Event event;
int main(int argc,char* args[])
{
	ofstream fout("framelog.txt");
	PHYSIM scene1((vect){998,755,40});
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0xDD,0xDD,0xDD));
	SDL_Flip(scr);
	SDL_Surface* skyline=loadimage("images/White Cube.jpg");
	SDL_Surface* sphere=loadimage("images/blue ball.png");
	//scene1.gensphere(sphere)->addvel(random((vect){-50,-50,0},(vect){50,50,0}));
	//SDL_Delay(500);
	while(!scene1.ended)
	{
		if(scene1.frametimer.currentframe()%100==0)
		{
			SPHERE* TEMP=NULL;
			while(!TEMP)
			{
				TEMP=new SPHERE((void*)&scene1,loadimage("images/blue ball.png"),randomposition,(vect){20,20,20},1);
				if(!TEMP)
					SDL_Delay(500);
			}
			scene1.sphere.push_back(TEMP);
			TEMP->addvel(random((vect){-10,-10,0},(vect){10,10,50}));
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
			scene1.mousemotion(event);
			scene1.CheckCameraMovement(event);
			if( event.type == SDL_MOUSEBUTTONDOWN )
				if( event.button.button == SDL_BUTTON_LEFT )
			    {
					vect newpos=scene1.mousepos;
					newpos.z=random(scene1.scrpos.z,scene1.scrpos.z+scene1.scrdim.z);
					scene1.gensphere(loadimage("images/blue ball.png"),newpos,(vect){20,20,20});
					//scene1.delsphere(3);
			    }
		}
		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		if(scene1.frametimer.currentframe()>10)
		{
			scene1.MoveCamera();
			for(unsigned int i=1;i<scene1.sphere.size();i++)
			{
				scene1.sphere[i]->addacc((vect){0,98,0});
					for(unsigned int j=1;j<scene1.sphere.size();j++)
					{
						if(i!=j)
						{
							if(scene1.sphere[j]->isindependent())
								scene1.sphere[i]->attach(scene1.sphere[j],(void*)&scene1);
							else if(scene1.sphere[i]->isindependent())
								scene1.sphere[j]->attach(scene1.sphere[i],(void*)&scene1);
							//else ... transfer all objects from j to i here...

						}
					}
					if(!scene1.sphere[i]->globalcollision((void*)&scene1,scene1.frametimer.deltatime()))
					{
						scene1.sphere[i]->integrate(scene1.frametimer.deltatime());
					}
			}
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		scene1.DisplaySortSpheres();
		for(unsigned int i=1;i<scene1.sphere.size();i++)
		{
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
	if(sphere)
		SDL_FreeSurface(sphere);
	if(skyline)
		SDL_FreeSurface(skyline);
	return 0;
}
