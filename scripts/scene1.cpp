#include "headers/physim.h"
#include <string.h>
using namespace std;
SDL_Event event;
int cell=1234;

int main(int argc,char* args[])
{
	HWND hwnd=FindWindowA("ConsoleWindowClass",NULL);	//finds the appropriate Dos window
	ShowWindow(hwnd,false);								//hides the DOS window (so Only graphics window is shown...)
	char kerater[30]="null",blue_ball[30]="null";
	file_loc(kerater,font_loc,"KeraterMedium.ttf");
	file_loc(blue_ball,image_loc,"blue ball.png");

	PHYSIM scene1((vect){998,755,400});
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0xDD,0xDD,0xDD));

	graphicstring message("scene1",28,kerater,10000);
	message.set_position(1,1);
	message.display();

	SDL_Flip(scr);
	graphicstring fps_label("FPS:",14,kerater,10000);
	fps_label.set_position(scrdim.x-80,1);
	graphicstring fps("0",14,kerater,500);
	fps.set_position(scrdim.x-50,1);
	graphicstring particle_n_label("Sphere number:",14,kerater,100);
	particle_n_label.set_position(scrdim.x-135,25);
	graphicstring particle_n("0",14,kerater,100);
	particle_n.set_position(scrdim.x-25,25);
	SDL_Surface* skyline=loadimage(file_loc(buf,image_loc,"White Cube.jpg"));
	SDL_Surface* sphere=loadimage(file_loc(buf,image_loc,"blue ball.png"));
	//Mix_Chunk *bounce_loud=Mix_LoadWAV("audio/Bounce Loud.wav");
	Mix_Chunk *bounce=Mix_LoadWAV(file_loc(buf,audio_loc,"Bounce.wav"));
	Mix_Chunk *roll=Mix_LoadWAV(file_loc(buf,audio_loc,"roll.wav"));
	Mix_Chunk *mash=Mix_LoadWAV(file_loc(buf,audio_loc,"mash.wav"));
	while(!scene1.ended)
	{
		if(scene1.frametimer.currentframe()%50==0)
		{
			SPHERE* TEMP=scene1.gensphere(loadimage(blue_ball),cell,randomposition,(vect){20,20,20},1);
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
					scene1.gensphere(loadimage(blue_ball),cell,newpos,(vect){20,20,20});
			    }
		}
		particle_n.set((int)scene1.sphere.size());
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
					if(scene1.sphere[i]->time_since_last_collision()>100)
					{
						if(scene1.sphere[i]->globalcollision((void*)&scene1,scene1.frametimer.deltatime()))
						{
							Mix_PlayChannel( -1, bounce, 0 );
						}
					}
					else if(scene1.sphere[i]->time_since_last_collision()<50)
					{
						if(scene1.sphere[i]->globalcollision((void*)&scene1,scene1.frametimer.deltatime())&&scene1.sphere[i]->velocity().mag()>1)
						{
							Mix_PlayChannel( -1, roll, 0 );
						}
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
		message.display();
		fps_label.display();
		fps.display();
		particle_n_label.display();
		particle_n.display();
		//.................................

		//---------------------------------termination
		fps.set(scene1.frametimer.currentfps());
		scene1.terminateframe(skyline);
		if(scene1.frametimer.currentframe()>10000)
			scene1.ended=true;
		//---------------------------------
	}
	Mix_FreeChunk(bounce);
	Mix_FreeChunk(mash);
	Mix_FreeChunk(roll);
	if(sphere)
		SDL_FreeSurface(sphere);
	if(skyline)
		SDL_FreeSurface(skyline);
	return 0;
}
