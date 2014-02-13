#include "headers/physim.h"

using namespace std;
SDL_Event event;
int main(int argc,char* args[])
{
	ofstream fout("framelog.txt");	//used to log frame rate statistics
	PHYSIM scene1((vect<>){960,512,32});	//main variable containing the properties of the physical world
	scene1.change_world_dimensions((vect<>){10000,10000,200000});
	scene1.camera_speed=100;
	scene1.camera_pos.z=9000;
	scene1.title="Aevo";
	scene1.title.render_image(true);

	SDL_FillRect(scene1.scr,&scene1.scr->clip_rect,SDL_MapRGB(scene1.scr->format,0xDD,0xDD,0xDD));	//Initializes a grey background
	SDL_Flip(scene1.scr);
	/**
	 * loads various image and audio files,etc onto the RAM in the form of SDL_Surface pointers variables,etc
	 */
	SDL_Surface* space=loadimage("images/bright space.jpg");
	Mix_Chunk *startup=Mix_LoadWAV("audio/Startup.wav");
	Mix_Music *music = Mix_LoadMUS("audio/an-ending-a-beginning.wav");
	Mix_PlayMusic( music, -1 );
	if(startup)
	{
		Mix_PlayChannel( -1, startup, 0 );
	}
	if(space)
	{
		space=rotozoomSurface(space,0,0.5,0);
	}
	SDL_Surface* star=loadimage("images/star.png"),*ball=loadimage("images/blue ball.png");
	while(!scene1.quit)	//controls the terminating condition of the simulation
	{
		//=================================initialisation
		if(scene1.frametimer.currentframe()==25)
		{
			scene1.gensphere(loadimage("images/Blackhole.png"),(vect<>){500,350,10000},(vect<>){100,100,100},pow((long double)10,18));
		}
		if(scene1.frametimer.currentframe()%50==0)	//block that auto generates a new sphere every 50 frames and provides certain properties to it
		{
			scene1.gensphere(star,scene1.random_position(),(vect<>){20,20,20},pow((long double)10,10))->PARTICLE::addvel(random((vect<>){-1000,-1000,0},(vect<>){1000,1000,0}));
		}
		scene1.initiateframe();
		for(unsigned int i=1;i<scene1.spheres.size();i++)
			scene1.spheres[i]->newframe();
		//=================================

		//_________________________________user interaction
		while(scene1.poll_event())
		{
			scene1.SDL_handle_events();
			scene1.mousemotion();
			scene1.HandleCameraMovement();
			scene1.camera_pos.z+=1;
			if( scene1.event.type == SDL_MOUSEBUTTONDOWN )	//check if the left mouse button has been pressed and then generates a new object at that position
				if( scene1.event.button.button == SDL_BUTTON_LEFT )
				{
					vect<> newpos=scene1.real_position_of(scene1.mousepos,500);
					if(newpos.z<0)
						newpos.z=0;
					else if(newpos.z>scene1.world_dim.z)
						newpos.z=scene1.world_dim.z;
					scene1.gensphere(copy_surface(ball),newpos,(vect<>){20,20,20},pow(10,15))->PARTICLE::addvel(random((vect<>){-50,-50,-50},(vect<>){50,50,50}));
					//aevo.gensphere(loadimage(blue_ball),newpos,(vect<>){20,20,20});
				}
		}
		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		if(scene1.frametimer.currentframe()>50)
		{
			scene1.MoveCamera();
			for(unsigned int i=0;i<scene1.spheres.size();i++)
			{
					for(unsigned int j=0;j<scene1.spheres.size();j++)
					{
						if(i!=j)
						{
							if(j>i&&scene1.spheres[i]->mass()<pow((long double)10,18)&&scene1.spheres[j]->mass()<pow((long double)10,18))
								scene1.spheres[i]->collision(*scene1.spheres[j]);
							scene1.spheres[i]->gravity(scene1.spheres[j]);
						}
					}
					scene1.spheres[i]->integrate(scene1.frametimer.deltatime());
			}
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		scene1.DisplaySortAllObjects();
		for(unsigned int i=1;i<scene1.spheres.size();i++)
		{
			scene1.spheres[i]->display();
		}
		//.................................

		//---------------------------------termination
		fout<<scene1.frametimer.currentfps()<<"	"<<scene1.frametimer.deltatime()<<"\n";
		scene1.terminateframe(space);
		if(scene1.frametimer.currentframe()>10000)
			scene1.quit=true;
		//---------------------------------
	}
	if(star!=NULL)
		SDL_FreeSurface(star);
	SDL_FreeSurface(space);
	if( Mix_PausedMusic() != 1 )
		Mix_PauseMusic();
	Mix_FreeChunk(startup);
	Mix_FreeMusic( music );
	return 0;
}
