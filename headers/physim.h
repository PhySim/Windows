/*
 * physim.hpp
 *
 *  Created on: Jun 30, 2013
 *      Author: Reuben
 */
#include "SDL/SDL.h"
#include "SDL/SDL_Image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include <fstream>
#include <vector>

#include <headers/vect.hpp>
#include <headers/random.hpp>
#include <headers/debug.hpp>
#include <headers/particle.hpp>
#include <headers/global_assets.hpp>
#include <headers/framer.hpp>

using namespace std;

class PHYSIM
{
	SDL_Surface* scr;
	SDL_Rect scrdim;
	PARTICLE* handler;
	int N;
	void frametermination()
	{
		SDL_Delay(frametimer.remainingfreetime());
		frametimer.endframe();
	}
	void general_genparticle()
	{
		particle.push_back(handler);
		N++;
	}
public:
	vector<PARTICLE*> particle;
	int bpp;
	DEBUG* error;
	timer runtime;
	framer frametimer;
	bool ended;
	PHYSIM(SDL_Rect user_dim,int user_bpp=32)
	{
		runtime.start();
		error=new DEBUG((char*)"psm");
		handler=NULL;
		N=0;
		if(SDL_Init(SDL_INIT_EVERYTHING)==-1)
			error->found((char*)"PHYSIM()",(char*)"SDL_Init() failed");
		srand(SDL_GetTicks());
		bpp=user_bpp;
		::scrdim=scrdim=user_dim;
		::scr=scr=SDL_SetVideoMode(scrdim.w,scrdim.h,bpp,SDL_SWSURFACE|SDL_RESIZABLE);
		frametimer.currentfrequency();
		ended=false;
	}

	PARTICLE* genparticle(SDL_Surface* user_texture)
	{
		handler=new PARTICLE(user_texture);
		general_genparticle();
		return handler;
	}
	PARTICLE* genparticle(vect position,vect dimension, SDL_Surface* user_texture)
	{
		handler=new PARTICLE(position,dimension,user_texture);
		general_genparticle();
		return handler;
	}
	void initiateframe()
	{
		frametimer.newframe();
	}
	void terminateframe(SDL_Color user_color)
	{
		SDL_Flip(scr);
		SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,user_color.r,user_color.g,user_color.b));
		frametermination();
	}
	void terminateframe(SDL_Surface* user_background)
	{
		SDL_Flip(scr);
		applysurface(user_background);
		frametermination();
	}
	~PHYSIM()
	{
		ofstream allo("allocation log.txt");
		allo<<particle.size()-1<<" particles to delete:"<<"\n";
		allo.close();
		for(int i=particle.size()-1;i>=0;i--)
		{
			handler=particle[i];
			allo.open("allocation log.txt",ios::app);
			allo<<"deleting	"<<handler<<"\n";
			allo.close();
			delete handler;
		}
		SDL_FreeSurface(scr);
		SDL_Quit();
		delete error;
	}
};
