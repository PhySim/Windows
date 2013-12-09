#include "headers/physim.h"
#include <string.h>
using namespace std;
SDL_Event event;

int main(int argc,char* args[])
{
	HWND hwnd=FindWindowA("ConsoleWindowClass",NULL);	//finds the appropriate Dos window
	ShowWindow(hwnd,false);								//hides the DOS window (so Only graphics window is shown...)

	//storing locations of various files to be loaded for later use
	char kerater[30]="null",blue_ball[30]="null";
	file_loc(kerater,font_loc,"KeraterMedium.ttf");
	file_loc(blue_ball,image_loc,"blue ball.png");

	PHYSIM scene1((vect){998,755,400});	//creating the main object
	SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,0xDD,0xDD,0xDD));	//intitiate the entire screen with grey color
	graphicstring message("scene1",28,kerater,10000);	//initiate the name of the scene
	message.set_position(1,1);							//set its position
	message.display();									//display it on screen at that position
	SDL_Flip(scr);										//update the screen to allow the user to see the latest version of it

	//initiating various other texts to be labelled in screen
	graphicstring fps_label("FPS:",14,kerater,10000);
	fps_label.set_position(scrdim.x-80,1);
	graphicstring fps("0",14,kerater,500);
	fps.set_position(scrdim.x-50,1);
	graphicstring particle_n_label("Particle number:",14,kerater,100);
	particle_n_label.set_position(scrdim.x-135,25);
	graphicstring particle_n("0",14,kerater,100);
	particle_n.set_position(scrdim.x-25,25);
	graphicstring bugs("no bugs",14,kerater,500);
	bugs.set_position(scrpos.x+25,scrdim.y-25);

	//loading various required images, and sounds
	SDL_Surface* skyline=loadimage(file_loc(buf,image_loc,"White Cube.jpg"));
	SDL_Surface* blue_sphere=loadimage(file_loc(buf,image_loc,"blue ball.png"));
	//Mix_Chunk *bounce_loud=Mix_LoadWAV("audio/Bounce Loud.wav");
	Mix_Chunk *bounce=Mix_LoadWAV(file_loc(buf,audio_loc,"Bounce.wav"));
	Mix_Chunk *bounce_loud=Mix_LoadWAV(file_loc(buf,audio_loc,"Bounce Loud.wav"));
	Mix_Chunk *mash=Mix_LoadWAV(file_loc(buf,audio_loc,"mash.wav"));

	DNA first_cell;

	//main loop within which most of the processing starts
	while(!scene1.ended)	//variable that controls the end of the program
	{
		//generates a new blue ball object every ... frames at a random position
		if(scene1.frametimer.currentframe()%100==0)
		{
			CELL* TEMP=new CELL((void*)&scene1,loadimage(blue_ball),randomposition,(vect){20,20,20},200.0);
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
			scene1.CheckCameraMovement(event);	//handle user input related to camera motion
			if( event.type == SDL_MOUSEBUTTONDOWN )	//check if the left mouse button has been pressed and then generates a new object at that position
				if( event.button.button == SDL_BUTTON_LEFT )
			    {
					vect newpos=scene1.mousepos;
					newpos.z=scene1.cameraPos.z+300;
					if(newpos.z<0)
						newpos.z=0;
					else if(newpos.z>scene1.scrdim.z)
						newpos.z=scrdim.z;
					scene1.cells.push_back(new CELL((void*)&scene1,loadimage(blue_ball),newpos,(vect){20,20,20},200.0));
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
					CELL* food=scene1.cells[i]->find_food((void*)&scene1);
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
							if(scene1.cells[i]->collision(*scene1.cells[j])&&scene1.cells[i]->continuous_contact()<2)
								Mix_PlayChannel( -1, bounce_loud, 0 );
						}
					}
					if(scene1.cells[i]->time_since_last_collision()>100)
					{
						if(scene1.cells[i]->globalcollision((void*)&scene1,scene1.frametimer.deltatime()))
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
			scene1.cells[i]->display((void*)&scene1);
		}
		//display various texts on screen
		message.display();
		fps_label.display();
		fps.display();
		particle_n_label.display();
		particle_n.set(int(scene1.spheres.size()+scene1.cells.size()));	//store the number of particles currently on screen into the particle_n graphicstring object
		particle_n.display();
		bugs.display();
		//.................................

		//---------------------------------termination
		fps.set(scene1.frametimer.currentfps());	//set the current fps into fps graphic string object
		scene1.terminateframe(skyline);	//does necessary actions to terminate the current frame appropriately
		if(scene1.frametimer.currentframe()>10000)	//ends the program if more than 10000 frames have been displayed
			scene1.ended=true;
		//---------------------------------
	}
	//deallocate various dynamic allocations
	Mix_FreeChunk(bounce);
	Mix_FreeChunk(mash);
	if(blue_sphere)
		SDL_FreeSurface(blue_sphere);
	if(skyline)
		SDL_FreeSurface(skyline);
	return 0;
}
