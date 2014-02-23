#include "headers/physim.h"
#include <string>

using namespace std;

SDL_Surface* background;
SDL_Surface* cell;
Mix_Chunk *bounce;
Mix_Chunk *bounce_loud;
Mix_Chunk *mash;
Mix_Chunk *startup;
Mix_Music *music;

bool load_media()
{
	bool fail=0;
	background=loadimage((image_loc+"cell feild.bmp").c_str());
	for(int i=0;i<10&&background==NULL;i++)
	{
		background=loadimage((image_loc+"cell feild.bmp").c_str());
		SDL_Delay(100);
		fail++;
	}
	cell=loadimage((image_loc+"cell.png").c_str());
	for(int i=0;i<10&&cell==NULL;i++)
	{
		cell=loadimage((image_loc+"cell.png").c_str());
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
	bounce_loud=Mix_LoadWAV((audio_loc+"cell eat.wav").c_str());
	for(int i=0;i<10&&bounce_loud==NULL;i++)
	{
		bounce_loud=Mix_LoadWAV((audio_loc+"cell eat.wav").c_str());
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
	if(background)
		SDL_FreeSurface(background);
	if(cell)
		SDL_FreeSurface(cell);
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

	PHYSIM aevo((vect<>){998,755,32});
	aevo.change_world_dimensions((vect<>){300,300,300});	//creating the main object
	aevo.camera_pos=aevo.world_dim/2;aevo.camera_pos.z/=2;
	aevo.title="Aevo";
	aevo.title.render_image(true);

	//loading various required images, and sounds
	bool fail=load_media();
	if(fail)
	{
		aevo.bugs="failed to load media files";
		aevo.bugs.display();
		SDL_Flip(aevo.scr);										//update the screen to allow the user to see the latest version of it
		aevo.quit=true;
		SDL_Delay(2500);
	}
	SDL_Flip(aevo.scr);										//update the screen to allow the user to see the latest version of it

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
	while(!aevo.quit)	//variable that controls the end of the program
	{
		//generates a new blue ball object every ... frames at a random position
		if(aevo.frametimer.currentframe()%1000==0)
		{
			CELL* TEMP=new CELL(aevo,loadimage(cell_loc),aevo.random_position(),first_DNA);
			aevo.cells.push_back(TEMP);
			if(TEMP)
				TEMP->PARTICLE::addvel(random((vect<>){-10,-10,0},(vect<>){10,10,50}));
		}
		//=================================initialisation
		aevo.initiateframe();
		for(unsigned int i=0;i<aevo.cells.size();i++)	//preparing all objects currently on screen for a new frame
			aevo.cells[i]->newframe();
		//=================================

		//_________________________________user interaction
		while( aevo.poll_event() )	//receive the latest user interactions
		{
			if( aevo.event.type == SDL_QUIT )	//check if the close button has been pressed
			{
					aevo.quit=true;
			}
			aevo.mousemotion();		//handle mouse motion
			aevo.HandleCameraMovement();	//handle user input related to camera motion
			if( aevo.event.type == SDL_MOUSEBUTTONDOWN )	//check if the left mouse button has been pressed and then generates a new object at that position
				if( aevo.event.button.button == SDL_BUTTON_LEFT )
			    {
					vect<> newpos=aevo.real_position_of(aevo.mousepos,50);
					if(newpos.z<0)
						newpos.z=0;
					else if(newpos.z>aevo.world_dim.z)
						newpos.z=aevo.world_dim.z;
					aevo.cells.push_back(new CELL(aevo,copy_surface(cell),newpos,first_DNA));
					//aevo.gensphere(loadimage(blue_ball),newpos,(vect<>){20,20,20});
			    }
		}
		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		if(aevo.frametimer.currentframe()>5)
		{
			aevo.MoveCamera();
			for(unsigned int i=0;i<aevo.cells.size();i++)
			{
				CELL* A=aevo.cells[i];
				/*if(aevo.frametimer.currentframe()>100)
				{
					CELL* food=A->find_food();
					CELL* predator=A->spot_predators();
					vect<> relative_position,relative_vel;
					if(predator)
					{
						relative_position=(predator->position()-A->position()).dir();
						relative_vel=predator->velocity()-A->velocity();
						relative_position*=-1;
						A->addacc(relative_position);
					}
					else if(food)
					{
						relative_position=(food->position()-A->position()).dir();
						relative_vel=food->velocity()-A->velocity();
						relative_position*=10;
						A->addacc(relative_position+relative_vel);
					}
				}*/

				for(unsigned int j=0;j<aevo.cells.size();j++)
				{
					CELL* B=aevo.cells[j];
					if(i!=j)
					{
						if(A->position().separation(B->position())<10*A->diameter())
						{
							A->connect_spring(B,10*A->diameter(),10);
						}
						A->spring(*B);
						if(A->collision(*B)&&A->continuous_contact()<2)
							Mix_PlayChannel( -1, bounce_loud, 0 );
					}
				}
				if(A->globalcollision(aevo.frametimer.deltatime()))
				{
					Mix_PlayChannel( -1, bounce, 0 );
				}
				A->integrate(aevo.frametimer.deltatime());
			}
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		aevo.display_all();
		//.................................

		//---------------------------------termination
		aevo.terminateframe(background);	//does necessary actions to terminate the current frame appropriately
		//---------------------------------
	}
	//deallocate various dynamic allocations
	free_media();
	return 0;
}
