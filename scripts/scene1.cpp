#include "headers/physim.h"

using namespace std;
SDL_Event event;
int main(int argc,char* args[])
{
	ofstream fout("framelog.txt");
	PHYSIM scene1((vect){998,755,400});
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0xDD,0xDD,0xDD));
	SDL_Flip(scr);
	SDL_Surface* skyline=loadimage("images/White Cube.jpg");
	SDL_Surface* sphere=loadimage("images/blue ball.png");
	//Mix_Chunk *bounce_loud=Mix_LoadWAV("audio/Bounce Loud.wav");
	Mix_Chunk *bounce=Mix_LoadWAV("audio/Bounce.wav");
	Mix_Chunk *mash=Mix_LoadWAV("audio/mash.wav");
	while(!scene1.ended)
	{
		if(scene1.frametimer.currentframe()%50==0)
		{
			SPHERE* TEMP=scene1.gensphere(loadimage("images/blue ball.png"),randomposition,(vect){20,20,20},1);
			if(TEMP)
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
					newpos.z=scene1.cameraPos.z+300;
					if(newpos.z<0)
						newpos.z=0;
					else if(newpos.z>scene1.scrdim.z)
						newpos.z=scrdim.z;
					scene1.gensphere(loadimage("images/blue ball.png"),newpos,(vect){20,20,20});
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
							if(scene1.sphere[i]->mash(scene1.sphere[j],(void*)&scene1))
								Mix_PlayChannel( -1, mash, 0 );
							//if(scene1.sphere[i]->collision(*scene1.sphere[j]))
								//Mix_PlayChannel( -1, bounce_loud, 0 );
						}
					}
					if(scene1.sphere[i]->globalcollision((void*)&scene1,scene1.frametimer.deltatime()))
					{
						if(scene1.sphere[i]->justcollided()<2)
							Mix_PlayChannel( -1, bounce, 0 );
					}
					scene1.sphere[i]->integrate(scene1.frametimer.deltatime());
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
