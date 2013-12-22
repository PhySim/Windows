#include "headers/physim.h"
#include <string.h>
using namespace std;
SDL_Event event;

SDL_Surface* background;
SDL_Surface* cell;
Mix_Chunk *bounce;
Mix_Chunk *bounce_loud;
Mix_Chunk *mash;
Mix_Chunk *startup;
Mix_Music *music;

bool load_media()
{
	bool success=true;
	background=loadimage(file_loc(buf,image_loc,"cell feild.bmp"));
	if(background==NULL)
		success=false;
	cell=loadimage(file_loc(buf,image_loc,"cell.png"));
	if(cell==NULL)
		success=false;
	bounce=Mix_LoadWAV(file_loc(buf,audio_loc,"Bounce.wav"));
	if(bounce==NULL)
		success=false;
	bounce_loud=Mix_LoadWAV(file_loc(buf,audio_loc,"cell eat.wav"));
	if(bounce_loud==NULL)
		success=false;
	mash=Mix_LoadWAV(file_loc(buf,audio_loc,"mash.wav"));
	if(mash==NULL)
		success=false;
	startup=Mix_LoadWAV("audio/Startup.wav");
	if(startup==NULL)
		success=false;
	music = Mix_LoadMUS("audio/we move lightly.mp3");
	if(music==NULL)
		success=false;
	return success;
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
	char kerater[30]="null",cell_loc[30]="null";
	file_loc(kerater,font_loc,"KeraterMedium.ttf");
	file_loc(cell_loc,image_loc,"cell.png");

	PHYSIM scene1((vect){998,755,400});	//creating the main object
	SDL_FillRect(scene1.scr,&scene1.scr->clip_rect,SDL_MapRGB(scene1.scr->format,0xDD,0xDD,0xDD));	//intitiate the entire screen with grey color
	graphicstring message(&scene1,"scene1",28,kerater,10000);	//initiate the name of the scene
	message.set_position(1,1);							//set its position
	message.display(scene1);									//display it on screen at that position
	graphicstring bugs(&scene1,"no bugs",14,kerater,500);
	bugs.set_position(scrpos.x+25,scrdim.y-25);
	if(!load_media())
	{
		bugs.set("failed to load media files");
		SDL_Flip(scene1.scr);										//update the screen to allow the user to see the latest version of it
		scene1.ended=true;
		SDL_Delay(5000);
	}
	SDL_Flip(scene1.scr);										//update the screen to allow the user to see the latest version of it

	//initiating various other texts to be labelled in screen
	graphicstring fps_label(&scene1,"FPS:",14,kerater,10000);
	fps_label.set_position(scrdim.x-80,1);
	graphicstring fps(&scene1,"0",14,kerater,500);
	fps.set_position(scrdim.x-50,1);
	graphicstring particle_n_label(&scene1,"Particle number:",14,kerater,100);
	particle_n_label.set_position(scrdim.x-135,25);
	graphicstring particle_n(&scene1,"0",14,kerater,100);
	particle_n.set_position(scrdim.x-25,25);
	bugs.set_position(scrpos.x+25,scrdim.y-25);

	if(music)
		Mix_PlayMusic( music, -1 );
	if(startup)
	{
		Mix_PlayChannel( -1, startup, 0 );
	}

	load_first_cell();

	//main loop within which most of the processing starts
	while(!scene1.ended)	//variable that controls the end of the program
	{
		//generates a new blue ball object every ... frames at a random position
		if(scene1.frametimer.currentframe()%1000==0)
		{
			CELL* TEMP=new CELL(scene1,loadimage(cell_loc),randomposition,first_DNA);
			scene1.cells.push_back(TEMP);
			if(TEMP)
				TEMP->addvel(random((vect){-10,-10,0},(vect){10,10,50}));
		}
		//=================================initialisation
		scene1.initiateframe();
		for(unsigned int i=0;i<scene1.cells.size();i++)	//preparing all objects currently on screen for a new frame
			scene1.cells[i]->newframe();
		//=================================

		//_________________________________user interaction
		while( SDL_PollEvent( &event ) )	//receive the latest user interactions
		{
			if( event.type == SDL_QUIT )	//check if the close button has been pressed
			{
					scene1.ended=true;
			}
			scene1.mousemotion(event);		//handle mouse motion
			scene1.HandleCameraMovement(event);	//handle user input related to camera motion
			if( event.type == SDL_MOUSEBUTTONDOWN )	//check if the left mouse button has been pressed and then generates a new object at that position
				if( event.button.button == SDL_BUTTON_LEFT )
			    {
					vect newpos=scene1.mousepos;
					newpos.z=scene1.cameraPosition().z+300;
					if(newpos.z<0)
						newpos.z=0;
					else if(newpos.z>scene1.scrdim.z)
						newpos.z=scrdim.z;
					scene1.cells.push_back(new CELL(scene1,copy_surface(cell),newpos,first_DNA));
					//scene1.gensphere(loadimage(blue_ball),newpos,(vect){20,20,20});
			    }
		}
		//_________________________________

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~physics simulation
		if(scene1.frametimer.currentframe()>10)
		{
			scene1.MoveCamera();
			for(unsigned int i=0;i<scene1.cells.size();i++)
			{
				if(scene1.frametimer.currentframe()>50)
				{
					CELL* food=scene1.cells[i]->find_food();
					if(food)
					{
						vect a=(food->position()-scene1.cells[i]->position()).dir()*20;
						bugs.set((double)a.mag());
						scene1.cells[i]->addacc(a);
					}
				}

					for(unsigned int j=0;j<scene1.cells.size();j++)
					{
						if(i!=j)
						{
							if(scene1.cells[i]->mash(*scene1.cells[j])&&scene1.cells[i]->continuous_contact()<2)
								Mix_PlayChannel( -1, bounce_loud, 0 );
						}
					}
					if(scene1.cells[i]->time_since_last_collision()>100)
					{
						if(scene1.cells[i]->globalcollision(scene1.frametimer.deltatime()))
						{
							Mix_PlayChannel( -1, bounce, 0 );
						}
					}
					scene1.cells[i]->integrate(scene1.frametimer.deltatime());
			}
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//.................................graphic rendering
		scene1.DisplaySortCells();	//makes sure objects closer to the camera are displayed on top of objects further away
		for(unsigned int i=0;i<scene1.cells.size();i++)	//display all objects on screen
		{
			scene1.cells[i]->display();
		}
		//display various texts on screen
		message.display(scene1);
		fps_label.display(scene1);
		fps.display(scene1);
		particle_n_label.display(scene1);
		particle_n.set(int(scene1.spheres.size()+scene1.cells.size()));	//store the number of particles currently on screen into the particle_n graphicstring object
		particle_n.display(scene1);
		bugs.display(scene1);
		//.................................

		//---------------------------------termination
		fps.set(scene1.frametimer.currentfps());	//set the current fps into fps graphic string object
		scene1.terminateframe(background);	//does necessary actions to terminate the current frame appropriately
		if(scene1.frametimer.currentframe()>10000)	//ends the program if more than 10000 frames have been displayed
			scene1.ended=true;
		//---------------------------------
	}
	//deallocate various dynamic allocations
	free_media();
	return 0;
}
