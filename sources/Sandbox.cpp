#include "headers/physim.h"
#include <string>

using namespace std;

//SDL_Surface* background;
SDL_Surface* ball;
Mix_Chunk *bounce;
Mix_Chunk *bounce_loud;
Mix_Chunk *mash;
Mix_Chunk *startup;
Mix_Music *music;

bool load_media()
{
	bool fail=0;
	/*background=loadimage((image_loc+"ball feild.bmp").c_str());
	for(int i=0;i<10&&background==NULL;i++)
	{
		background=loadimage((image_loc+"ball feild.bmp").c_str());
		SDL_Delay(100);
		fail++;
	}*/
	ball=loadimage((image_loc+"blue ball_LQ.jpg").c_str());
	for(int i=0;i<10&&ball==NULL;i++)
	{
		ball=loadimage((image_loc+"blue ball_LQ.jpg").c_str());
		SDL_Delay(100);
		fail++;
	}
	bounce=Mix_LoadWAV((audio_loc+"Bounce.wav").c_str());
	for(int i=0;i<10&&bounce==NULL;i++)
	{
		bounce=Mix_LoadWAV((audio_loc+"Bounce.wav").c_str());
		SDL_Delay(100);
		fail++;
	}
	mash=Mix_LoadWAV((audio_loc+"mash.wav").c_str());
	for(int i=0;i<10&&mash==NULL;i++)
	{
		mash=Mix_LoadWAV((audio_loc+"mash.wav").c_str());
		SDL_Delay(100);
		fail++;
	}
	startup=Mix_LoadWAV((audio_loc+"startup.wav").c_str());
	for(int i=0;i<5&&startup==NULL;i++)
	{
		startup=Mix_LoadWAV((audio_loc+"startup.wav").c_str());
		SDL_Delay(2000);
		fail++;
	}
	music = Mix_LoadMUS((audio_loc+"we move lightly_HQ.wav").c_str());
	for(int i=0;i<5&&music==NULL;i++)
	{
		music = Mix_LoadMUS((audio_loc+"we move lightly_HQ.wav").c_str());
		SDL_Delay(2000);
		fail++;
	}
	return fail;
}
void free_media()
{
	/*if(background)
		SDL_FreeSurface(background);*/
	if(ball)
		SDL_FreeSurface(ball);
	if(bounce)
		Mix_FreeChunk(bounce);
	if(bounce_loud)
		Mix_FreeChunk(bounce_loud);
	if(mash)
		Mix_FreeChunk(mash);
	if(startup)
		Mix_FreeChunk(startup);
	if(music)
		Mix_FreeMusic(music);
}
DNA first_DNA;
void load_first_cell()
{
	first_DNA.baby_energy=10;
	first_DNA.baby_fat.mass=0.5;
	first_DNA.baby_fat.material.energy_factor=37;
	first_DNA.baby_fat.material.density=0.9;
	first_DNA.cytoplasm.mass=5;
	first_DNA.cytoplasm.material.density=0.405;
	first_DNA.organelles.mass=0.5;
	first_DNA.organelles.material.density=0.1350;
	first_DNA.visibility=200;
}

int main(int argc,char* args[])
{
	HWND hwnd=FindWindowA("ConsoleWindowClass",NULL);	//finds the appropriate Dos window
	ShowWindow(hwnd,false);								//hides the DOS window (so Only graphics window is shown...)

	//storing locations of various files to be loaded for later use
	string kerater=(font_loc+"KeraterMedium.ttf"),cell_loc=(image_loc+"cell.png");

	PHYSIM P((vect<>){998,755,32});
	P.camera_speed*=10;
	P.change_world_dimensions((vect<>){1000,1000,1000});	//creating the main object
	P.camera_pos=P.world_dim/2;P.camera_pos.z/=2;
	P.title="P";
	P.title.render_image(true);

	//loading various required images, and sounds
	bool fail=load_media();
	if(fail)
	{
		P.bugs="failed to load media files";
		P.bugs.display();
		SDL_Flip(P.scr);										//update the screen to allow the user to see the latest version of it
		P.quit=true;
		SDL_Delay(2500);
	}
	SDL_Flip(P.scr);										//update the screen to allow the user to see the latest version of it

	if(!fail)
	{
		if(music)
			Mix_PlayMusic( music, -1 );
		SDL_Delay(100);
		if(startup)
		{
			Mix_PlayChannel( -1, startup, 0 );
		}
	}

	load_first_cell();

	//main loop within which most of the processing starts
	while(!P.quit)	//variable that controls the end of the program
	{
		//generates a new blue ball object every ... frames at a random position
		if(P.frametimer.currentframe()%20==0)
		{
			long double ran=random(1,100);
			P.insert(new SPHERE(P,copy_surface(ball),P.random_position((vect<>){ran,ran,ran}),(vect<>){ran,ran,ran},ran));
		}
		//=================================initialisation
		P.initiateframe();
		//=================================

		//_________________________________user interaction
		while( P.poll_event() )	//receive the latest user interactions
		{
			if( P.event.type == SDL_QUIT )	//check if the close button has been pressed
			{
					P.quit=true;
			}
			P.mousemotion();		//handle mouse motion
			P.HandleCameraMovement();	//handle user input related to camera motion
			if( P.event.type == SDL_MOUSEBUTTONDOWN )	//check if the left mouse button has been pressed and then generates a new object at that position
				if( P.event.button.button == SDL_BUTTON_LEFT )
			    {
					vect<> newpos=P.real_position_of(P.mousepos,200);
					if(newpos.z<0)
						newpos.z=0;
					else if(newpos.z>P.world_dim.z)
						newpos.z=P.world_dim.z;
					long double ran=random(1,100);
					P.insert(new SPHERE(P,copy_surface(ball),newpos,(vect<>){ran,ran,ran},ran));
					//P.gensphere(loadimage(blue_ball),newpos,(vect<>){20,20,20});
			    }
		}
		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		if(P.frametimer.currentframe()>5)
		{
			P.MoveCamera();
			for(unsigned int i=0;i<P.spheres.size();i++)
			{
				SPHERE* A=P.spheres[i];
				for(unsigned int j=0;j<P.spheres.size();j++)
				{
					SPHERE* B=P.spheres[j];
					if(i!=j)
					{
						A->electrostatic(*B);
						if(A->collision(*B)&&A->continuous_contact()<2)
						{
							Mix_PlayChannel( -1, bounce, 0 );
						}
					}
				}
				if(A->globalcollision(P.frametimer.deltatime()))
				{
					Mix_PlayChannel( -1, bounce, 0 );
				}
				A->integrate(P.frametimer.deltatime());
			}
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		P.display_all();
		//.................................

		//---------------------------------termination
		P.terminateframe((SDL_Color){255,255,255});	//does necessary actions to terminate the current frame appropriately
		//---------------------------------
	}
	//deallocate various dynamic allocations
	free_media();
	return 0;
}
