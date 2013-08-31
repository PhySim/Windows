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
#include <math.h>

#include <headers/vect.hpp>
#include <headers/random.hpp>
#include <headers/debug.hpp>
#include <headers/objects.hpp>
#include <headers/global_assets.hpp>
#include <headers/framer.hpp>

using namespace std;

class PHYSIM
{
	SPHERE* handler;
	int N;
	double aov;
	void frametermination()
	{
		SDL_Delay(frametimer.remainingfreetime()*1000.0);
		frametimer.endframe();
	}
	SPHERE* general_genparticle(SPHERE* handler)
	{
		particle.push_back(handler);
		N++;
		return handler;
	}
public:
	SDL_Surface* scr;
	vect scrpos,scrdim;
	vector<SPHERE*> particle;
	int bpp;
	DEBUG* error;
	timer runtime;
	framer frametimer;
	bool ended;
	PHYSIM(vect user_dim,vect user_pos=(vect){0,0,0},int user_bpp=32)
	{
		runtime.start();
		error=new DEBUG((char*)"psm");
		handler=NULL;
		N=0;
		aov=M_PI/2.0;
		if(SDL_Init(SDL_INIT_EVERYTHING)==-1)
			error->found((char*)"PHYSIM()",(char*)"SDL_Init() failed");
		srand(SDL_GetTicks());
		bpp=user_bpp;
		::scrdim=scrdim=user_dim;
		::scrpos=scrpos=user_pos;
		::scr=scr=SDL_SetVideoMode(scrdim.x,scrdim.y,bpp,SDL_SWSURFACE|SDL_RESIZABLE);
		frametimer.currentfrequency();
		ended=false;
	}

	SPHERE* genparticle(SDL_Surface* user_texture)
	{
		return general_genparticle(new SPHERE(user_texture));
	}
	SPHERE* genparticle(vect position,vect dimension, SDL_Surface* user_texture)
	{
		general_genparticle(new SPHERE(position,dimension,user_texture));
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

	double AngleOfView()
	{
		return aov;
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

double SPHERE::zoomfactor(void* U)
{
	vect cameraPos(0,0,-1);
	double RealRatio;
	if((pos.z-cameraPos.z)==0)
		RealRatio=0.99;
	else
		RealRatio=(dim.y/(pos.z-cameraPos.z))/((pos.z-cameraPos.z)*1);//tan(((PHYSIM*)U)->AngleOfView()));
	if(RealRatio>0.9)
		RealRatio=0.9;
	zoom=(RealRatio*250)/dim.y;
	return zoom;
}
int SPHERE::globalcollision(void* U,double deltatime)
{
	SDL_Surface* scr=((PHYSIM*)U)->scr;
	if(pos.y+center.y+(vel.y+acc.y*deltatime)*deltatime>scrpos.y+scrdim.y)
	{
		long double frac=(vel.y*deltatime+0.5*acc.y*deltatime*deltatime);
		if(frac<0)
			frac=-frac;
		for(int i=0;i<frac;i++)
		{
			integrate(deltatime/frac);
			if(pos.y+center.y>scrpos.y+scrdim.y)
			{
				if(vel.y>0)
				{
					addvel(-vel*2,vect(0,dim.y/2,0));;
				}
				display(scr);
			}
		}
		return just_collided=1;
	}
	else if(pos.x+center.x+(vel.x+acc.x*deltatime)*deltatime>scrpos.x+scrdim.x)
	{
		long double frac=(vel.x*deltatime+0.5*acc.x*deltatime*deltatime);
		for(int i=0;i<frac;i++)
		{
			integrate(deltatime/frac);
			if(pos.x+center.x>scrpos.x+scrdim.x)
			{
				if(vel.x>0)
				{
					addvel(-vel*2,vect(dim.x/2,0,0));
				}
				display(scr);
			}
		}
		return just_collided=1;
	}
	else if(pos.y-center.y+(vel.y+acc.y*deltatime)*deltatime<scrpos.y)
	{
		long double frac=(vel.y*deltatime+0.5*acc.y*deltatime*deltatime);
		if(frac<0)
			frac=-frac;
		for(int i=0;i<frac;i++)
		{
			integrate(deltatime/frac);
			if(pos.y-center.y<scrpos.y)
			{
				if(vel.y<0)
				{
					addvel(-vel*2,vect(0,-dim.y/2,0));
				}
				display(scr);
			}
		}
		return 1;
	}
	else if(pos.x-center.x+(vel.x+acc.x*deltatime)*deltatime<scrpos.x)
	{
		long double frac=-(vel.x*deltatime+0.5*acc.x*deltatime*deltatime);
		if(frac<0)
			frac=-frac;
		for(int i=0;i<frac;i++)
		{
			integrate(deltatime/frac);
			if(pos.x-center.x<scrpos.x)
			{
				if(vel.x<0)
				{
					addvel(-vel*2,vect(-dim.x/2,0,0));
				}
				display(scr);
			}
		}
		return just_collided=1;
	}
	if(pos.z+center.z+(vel.z+acc.z*deltatime)*deltatime>scrpos.z+scrdim.z)
	{
		long double frac=(vel.z*deltatime+0.5*acc.z*deltatime*deltatime);
		if(frac<0)
			frac=-frac;
		for(int i=0;i<frac;i++)
		{
			integrate(deltatime/frac);
			if(pos.z+center.z>scrpos.z+scrdim.z)
			{
				if(vel.z>0)
				{
					vel.z=-vel.z;
				}
				display(scr);
			}
		}
		return just_collided=1;
	}
	else if(pos.z-center.z+(vel.z+acc.z*deltatime)*deltatime<scrpos.z)
	{
		long double frac=-(vel.z*deltatime+0.5*acc.z*deltatime*deltatime);
		if(frac<0)
			frac=-frac;
		for(int i=0;i<frac;i++)
		{
			integrate(deltatime/frac);
			if(pos.z-center.z<scrpos.z)
			{
				if(vel.z<0)
				{
					vel.z=-vel.z;
				}
				display(scr);
			}
		}
		return just_collided=1;
	}
	return just_collided=0;
}
