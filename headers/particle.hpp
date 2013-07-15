/*
 * particle.hpp
 *
 *  Created on: Jul 15, 2013
 *      Author: Reuben
 */
#include <headers/debug.hpp>
#include <headers/global_assets.hpp>
#ifndef PARTICLE_HPP_
#define PARTICLE_HPP_

using namespace std;

class PARTICLE
{
	SDL_Surface* tex;
	vect pos,dim,vel,acc;
	bool just_collided;
public:
	void general_construction()
	{
		just_collided=0;
	}
	PARTICLE(vect position,vect dimension, SDL_Surface* user_texture)
	{
		general_construction();
		pos=position;
		dim=dimension;
		tex=user_texture;
		if(tex==NULL)
			debugger.found("PARTICLE()","loadimage() failed");
	}
	PARTICLE(SDL_Surface* user_texture)
	{
		general_construction();
		tex=user_texture;
		if(tex==NULL)
			debugger.found("PARTICLE()","loadimage() failed");
		dim.x=tex->w;
		dim.y=tex->h;
		vect from(0,0,0);
		vect to(scr->w-dim.x,scr->h-dim.y,0);
		pos=random(from,to);
	}
	void trial(SDL_Event eve)
	{

		//If a key was pressed
		if( eve.type == SDL_KEYDOWN )
		{
			//vel.x++;
			//Set the proper message surface
			switch( (int)eve.key.keysym.sym )
			{
				//case SDLK_UP: ge; break;
				//case SDLK_DOWN: message = downMessage; break;
				case SDLK_LEFT: vel.x-=4;break;
				case SDLK_RIGHT: vel.x+=4; break;
			}
			//pos.x+=vel.x;
		}
	}
	vect addvel(vect b)
	{
		vel.add(b);
		return vel;
	}
	vect addacc(vect b)
	{
		acc.add(b);
		return acc;
	}

	void integrate(double deltatime_1000)
	{
		long double deltatime=deltatime_1000/1000.0;
		if(deltatime==0)
			debugger.found("integrate()","deltatimevalue=0");
		vect u=vel;
		vel.add(multiply(acc,deltatime));	//v=u+at
		pos.add(multiply(u,deltatime));		//s=s0+ut
		pos.add(multiply(acc,0.5*deltatime*deltatime));	//s=s0+a*t^2
	}
	int globalcollision(double deltatime_1000)
	{
		long double deltatime=deltatime_1000/1000.0;
		if(pos.y+dim.y+(vel.y+acc.y*deltatime)*deltatime>scrdim.y+scrdim.h)
		{
			long double frac=(vel.y*deltatime+0.5*acc.y*deltatime*deltatime);
			if(frac<0)
				frac=-frac;
			for(int i=0;i<frac;i++)
			{
				integrate(1000.0*deltatime/frac);
				if(pos.y+dim.y>scrdim.y+scrdim.h)
				{
					if(vel.y>0)
					{
						vel.y=-vel.y;
					}
					applysurface(tex,pos);
				}
			}
			return just_collided=1;
		}
		else if(pos.x+dim.x+(vel.x+acc.x*deltatime)*deltatime>scrdim.x+scrdim.w)
		{
			long double frac=(vel.x*deltatime+0.5*acc.x*deltatime*deltatime);
			for(int i=0;i<frac;i++)
			{
				integrate(1000.0*deltatime/frac);
				if(pos.x+dim.x>scrdim.x+scrdim.w)
				{
					if(vel.x>0)
					{
						vel.x=-vel.x;
					}
					applysurface(tex,pos);
				}
			}
			return just_collided=1;
		}
		else if(pos.y+(vel.y+acc.y*deltatime)*deltatime<scrdim.y)
		{
			long double frac=(vel.y*deltatime+0.5*acc.y*deltatime*deltatime);
			if(frac<0)
				frac=-frac;
			for(int i=0;i<frac;i++)
			{
				integrate(1000.0*deltatime/frac);
				if(pos.y<scrdim.y)
				{
					if(vel.y<0)
					{
						vel.y=-vel.y;
					}
					applysurface(tex,pos);
				}
			}
			return 1;
		}
		else if(pos.x+(vel.x+acc.x*deltatime)*deltatime<scrdim.x)
		{
			long double frac=-(vel.x*deltatime+0.5*acc.x*deltatime*deltatime);
			if(frac<0)
				frac=-frac;
			for(int i=0;i<frac;i++)
			{
				integrate(1000.0*deltatime/frac);
				if(pos.x<scrdim.x)
				{
					if(vel.x<0)
					{
						vel.x=-vel.x;
					}
					applysurface(tex,pos);
				}
			}
			return just_collided=1;
		}
		return just_collided=0;
	}
	bool justcollided()
	{
		return just_collided;
	}

	void display(SDL_Surface* screen)
	{
		applysurface(tex,pos);
	}

	~PARTICLE()
	{
		SDL_FreeSurface(tex);
	}
};

#endif /* PARTICLE_HPP_ */
