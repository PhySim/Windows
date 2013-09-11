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

const vect randomposition(-0.1514,1.5122,-4.1512);

class SPHERE
{
	SDL_Surface* tex;
	vect pos,appPos,dim,center,vel,acc,f,ang,tta,tor;
	long double mas,zoom,VisualDimensionRatio;
	bool just_collided;
	double zoomfactor(void* U);
	vect apparentPos(void* U);
public:
	void general_construction()
	{
		VisualDimensionRatio=1;
		just_collided=0;
		mas=1;
		center=dim/2;
		zoom=1;
	}
	SPHERE(SDL_Surface* user_texture,vect position,vect dimension,long double U_mass=1)
	{
		tex=user_texture;
		if(tex==NULL)
			debugger.found("SPHERE()","loadimage() failed");
		dim=dimension;
		general_construction();
		VisualDimensionRatio=sqrt(dim.x*dim.x+dim.y*dim.y)/sqrt(user_texture->clip_rect.w*user_texture->clip_rect.w+user_texture->clip_rect.h*user_texture->clip_rect.h);
		if(position==randomposition)
		{
			vect from(0,0,0);
			vect to(scr->w-dim.x,scr->h-dim.y,(scr->w-dim.x+scr->h-dim.y)/2);
			pos=random(from,to);
		}
		else
			pos=position;
		mas=U_mass;
	}
	SPHERE(SDL_Surface* user_texture,long double U_mass=1)
	{
		tex=user_texture;
		if(tex==NULL)
			debugger.found("SPHERE()","loadimage() failed");
		dim.x=tex->w;
		dim.y=tex->h;
		dim.z=(dim.x+dim.y)/2.0;
		general_construction();
		vect from(0,0,0);
		vect to(scr->w-dim.x,scr->h-dim.y,(scr->w-dim.x+scr->h-dim.y)/2);
		pos=random(from,to);
		mas=U_mass;
	}
	SPHERE(SDL_Surface* user_texture,vect U_pos,long double U_mass=1)
	{
		tex=user_texture;
		if(tex==NULL)
			debugger.found("SPHERE()","loadimage() failed");
		dim.x=tex->w;
		dim.y=tex->h;
		dim.z=(dim.x+dim.y)/2.0;
		general_construction();
		pos=U_pos;
		mas=U_mass;
	}
	vect position()
	{
		return pos;
	}
	vect velocity()
	{
		return vel;
	}
	long double mass()
	{
		return mas;
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
	vect addtta(vect u_tta)
	{
		return tta+=u_tta;
	}
	vect addvel(vect b)
	{
		vel+=b;
		return vel;
	}
	vect addvel(vect u_vel,vect u_pos)
	{
		tta+=((u_vel^u_pos)/u_pos.mag());
		return vel+=u_pos.dir()*((u_vel|u_pos)/u_pos.mag());
	}
	vect addacc(vect b)
	{
		acc+=b;
		return acc;
	}
	vect addforce(vect b)
	{
		f+=b;
		return f;
	}
	void display(void* scr)
	{
		applysurface(tex,apparentPos(scr),ang,zoomfactor(scr));
	}
	void integrate(double deltatime)
	{
		if(deltatime==0)
			debugger.found("integrate()","deltatimevalue=0");
		acc=(f/mas);
		vect u=vel;
		vel+=acc*deltatime;	//v=u+at
		pos+=u*deltatime;		//s=s0+ut
		pos+=acc*0.5*deltatime*deltatime;	//s=s0+a*t^2

		tta+=tor*deltatime;
		ang+=tta*deltatime;
		ang+=tor*0.5*deltatime*deltatime;
		if(ang.z>360)
			ang.z-=360;
		else if(ang.z<=-360)
			ang.z+=360;
	}
	void gravity(SPHERE* b)
	{
		vect relpos=(b->position()-pos);
		f+=relpos.dir()*G*mas*b->mass()/(relpos.mag())/relpos.mag();
	}
	int globalcollision(void* U,double deltatime);
	int collision(SPHERE &b)
	{
		if(pos.dist(b.position())<mag(dim))
		{
			vect avel=vel;
			vect bvel=b.velocity();
			addvel(bvel-avel,(pos-b.position()));
			b.addvel(avel-bvel,(b.position()-pos));
			return just_collided=1;
		}
		return just_collided=0;
	}
	bool justcollided()
	{
		return just_collided;
	}
	void newframe()
	{
		f=(vect){0,0,0};
		acc=(vect){0,0,0};
	}
	~SPHERE()
	{
		if(tex!=NULL)
			SDL_FreeSurface(tex);
	}
};

#endif /* PARTICLE_HPP_ */
