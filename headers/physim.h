/*
 * physim.hpp
 *
 *  Created on: Jun 30, 2013
 *      Author: Reuben
 */
#include "SDL/SDL.h"
#include "SDL/SDL_Image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"
#include <string>
#include <fstream>
#include <vector>
#include <math.h>

#include <headers/vect.hpp>
#include <headers/random.hpp>
#include <headers/debug.hpp>
#include <headers/global_assets.hpp>
#include <headers/framer.hpp>

using namespace std;

const vect randomposition(-0.1514,1.5122,-4.1512);
int debug_sphere_count=0;

void put_pixel( SDL_Surface *surface, int x, int y, SDL_Color color)
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint32 pixel=SDL_MapRGB(surface->format,255, 0, 0);
    //Set the pixel
    pixels[ ( y * surface->w ) + x ] = pixel;
}
void Drawline( SDL_Surface *surface, float x1, float y1, float x2, float y2, const SDL_Color& color )
{
	if( SDL_MUSTLOCK( surface ) )
	{
		//Lock the surface
		SDL_LockSurface( surface );
	}
    // Bresenham's line algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if(steep)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if(x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;
    for(int x=(int)x1; x<maxX; x++)
    {
        if(steep)
        {
        	put_pixel(surface,y,x, color);
        }
        else
        {
        	put_pixel(surface,x,y, color);
        }

        error -= dy;
        if(error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
    if( SDL_MUSTLOCK( surface ) )
	{
		SDL_UnlockSurface( surface );
	}
}
void vect_Line(void* PhySimObj,vect a,vect b,SDL_Color color);

struct SPRING
{
	double l,k;
	SDL_Color color;
};
class SPHERE
{
	vector<SPHERE*> member;
	SDL_Surface* tex;
	vect pos,appPos,dim,center,vel,acc,f,ang,tta,tor;
	vect relpos_parent;
	long double mas,zoom,VisualDimensionRatio;
	bool just_collided,independent;
	double zoomfactor(void* U);
	vect apparentPos(void* U);
	struct SPRING_CONNECTION
	{
		SPRING spring;
		SPHERE* partner;
		SPRING_CONNECTION(SPHERE* Partner,double rest_length,double spring_constant,SDL_Color color=(SDL_Color){0,255,0})
		{
			spring.l=rest_length;
			spring.k=spring_constant;
			partner=Partner;
		}
	};
	vector<SPRING_CONNECTION> spring_connection;
	struct cell_features
	{
		//double food, max_food...
	};
public:
	void general_construction()
	{
		debug_sphere_count++;
		VisualDimensionRatio=1;
		just_collided=0;
		independent=true;
		mas=1;
		center=dim/2;
		zoom=1;
	}
	SPHERE(void* PhySimObj,SDL_Surface* texture,vect position,vect dimension,long double U_mass);
	SPHERE(void* PhySimObj,SDL_Surface* texture,long double U_mass);
	SPHERE(void* PhySimObj,SDL_Surface* texture,vect U_pos,long double U_mass);
	bool isindependent()
	{
		return independent;
	}
	long double diameter()
	{
		return dim.mag();
	}
	vect position()
	{
		return pos;
	}
	vect relpos_with_respect_to_parent()
	{
		return relpos_parent;
	}
	vect velocity()
	{
		return vel;
	}
	long double mass()
	{
		return mas;
	}
	vect momentum()
	{
		return vel*mas;
	}
	long double volume()
	{
		return 4*M_PI*pow((diameter()/2.0),3)/3.0;
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
		tta+=((u_vel^u_pos)/pow(u_pos.mag(),2));
		return vel+=u_pos.dir()*((u_vel|u_pos)/u_pos.mag());
	}
	vect addmomentum(vect momentum)
	{
		return vel+=momentum/mas;
	}
	vect addmomentum(vect momentum,vect u_pos)
	{
		return addvel(momentum/mas,u_pos);
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
	long double addmass(long double mass)
	{
		mas+=mass;
		return mas;
	}
	long double addvolume(long double volume)
	{
		long double new_diameter=pow(pow((double)diameter(),3)+8*3*volume/4.0/M_PI,1/3.0);
		dim*=new_diameter/diameter();
		return volume;
	}
	void display(void* PhySimObject);
	bool display_as_member(void* PhySimObject,SPHERE &parent);
	void integrate(double deltatime)
	{
		if(deltatime==0)
			debugger.found("integrate()","deltatimevalue=0");
		acc+=(f/mas);
		vect u=vel;
		vel+=acc*deltatime;	//v=u+at
		pos+=u*deltatime;		//s=s0+ut
		pos+=acc*0.5*deltatime*deltatime;	//s=s0+a*t^2

		tta+=tor*deltatime;
		ang+=tta*deltatime;
		ang+=tor*0.5*deltatime*deltatime;
	}
	void gravity(SPHERE* &b)
	{
		vect relpos=(b->position()-pos);
		f+=relpos.dir()*G*mas*b->mass()/(relpos.mag())/relpos.mag();
	}
	unsigned int number_of_springs_connected()
	{
		return spring_connection.size();
	}
	bool spring_connected(SPHERE* b)
	{
		for(unsigned int i=0;i<spring_connection.size();i++)
		{
			if(spring_connection[i].partner==b)
				return true;
		}
		return false;
	}
	bool connect_spring(SPHERE* &b,double rest_length,double spring_constant)
	{
		bool spring_is_connected=spring_connected(b);
		if(!spring_is_connected)
			if(spring_connection.size()<1)
				spring_connection.push_back(SPRING_CONNECTION(b,rest_length,spring_constant));
		return spring_is_connected;
	}
	vect spring(SPHERE &b)
	{
		for(unsigned int i=0;i<spring_connection.size();i++)
		{
			if(spring_connection[i].partner==&b)
			{
				vect relpos=(b.position()-pos);
							//dir				extension							spring constant
				return f+=(relpos.dir()*(relpos.mag()-spring_connection[i].spring.l)*spring_connection[i].spring.k);
			}
		}
		return (vect){0,0,0};
	}
	bool touched(SPHERE &b)
	{
		return pos.separation(b.position())<(diameter()/2.0+b.diameter()/2.0);
	}
	int globalcollision(void* U,double deltatime);
	int collision(SPHERE &b);
	bool mash(SPHERE* &b,void* U);
	void attach(SPHERE* &b,void* U);
	bool justcollided()
	{
		return just_collided;
	}
	void newframe()
	{
		f=(vect){0,0,0};
		acc=(vect){0,0,0};
	}
	vect set_relative_position(vect parent_position)
	{
		independent=false;
		return relpos_parent=pos-parent_position;
	}
	~SPHERE()
	{
		debug_sphere_count--;
		if(tex)
			SDL_FreeSurface(tex);
	}
};

class PHYSIM
{
	SPHERE* handler;
	double aov;
	void frametermination()
	{
		SDL_Delay(frametimer.remainingfreetime()*1000.0);
		frametimer.endframe();
	}
	void general_construction()
	{
		frametimer.updatefpslimits(10,30);
		runtime.start();
		error=new DEBUG((char*)"psm");
		handler=NULL;
		aov=M_PI/4.0;
		if(SDL_Init(SDL_INIT_EVERYTHING)==-1)
			error->found((char*)"PHYSIM()",(char*)"SDL_Init() failed");
		if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
			error->found((char*)"PHYSIM()",(char*)"Mix_OpenAudio() failed");
		srand(SDL_GetTicks());
		ended=false;
	}
	SPHERE* general_gensphere(SPHERE* handler)
	{
		if(handler)
		{
			sphere.push_back(handler);
		}
		return handler;
	}
	bool up,down,left,right;
public:
	SDL_Surface* scr;
	vect scrpos,scrdim,cameraPos,mousepos;
	vector<SPHERE*> sphere;
	int bpp;
	DEBUG* error;
	timer runtime;
	framer frametimer;
	bool ended;
	PHYSIM(vect user_dim,vect user_pos=(vect){0,0,0},int user_bpp=32)
	{
		up=down=left=right=false;
		general_construction();
		bpp=user_bpp;
		::scrdim=scrdim=user_dim;
		cameraPos=user_dim/2;
		cameraPos.z=-200;
		::scrpos=scrpos=user_pos;
		::scr=scr=SDL_SetVideoMode(scrdim.x,scrdim.y,bpp,SDL_SWSURFACE|SDL_RESIZABLE);
	}

	SPHERE* gensphere(SDL_Surface* texture,long double U_mass=1)
	{
		handler=new SPHERE((void*)this,texture,U_mass);
		if(handler)
			return general_gensphere(handler);
		else
			return NULL;
	}
	SPHERE* gensphere(SDL_Surface* texture,vect position,vect dimension,long double U_mass=1)
	{
		handler=new SPHERE((void*)this,texture,position,dimension,U_mass);
		if(handler)
			return general_gensphere(handler);
		else
			return NULL;
	}
	SPHERE* gensphere(SDL_Surface* texture,vect position,long double U_mass=1)
	{
		handler=new SPHERE((void*)this,texture,position,U_mass);
		if(handler)
			return general_gensphere(handler);
		else
			return NULL;
	}
	int findSphere(SPHERE* U)
	{
		for(unsigned int i=0;i<sphere.size();i++)
		{
			if(sphere[i]==U)
				return i;
		}
		return -1;
	}
	bool delsphere(SPHERE* U=NULL)
	{
		if(U==NULL)
		{
			if(sphere.size()>1)
				sphere.pop_back();
			return true;
		}
		else
		{
			int result=findSphere(U);
			 if(result!=-1)
			 {
				 sphere.erase(sphere.begin()+result);
				 return true;
			 }
		}
		return false;
	}
	bool delsphere(unsigned int U)
	{
		if(U<sphere.size())
			sphere.erase(sphere.begin()+U);
		return false;
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
	void DisplaySortSpheres()
	{
		for(unsigned int i=1;i<sphere.size();i++)
		{
			for(unsigned int j=1;j<sphere.size()-i;j++)
			{
				if((sphere[j]->position().z-cameraPos.z)<(sphere[j+1]->position().z-cameraPos.z))
				{
					SPHERE* temp=sphere[j];
					sphere[j]=sphere[j+1];
					sphere[j+1]=temp;
				}
			}
		}
	}
	double AngleOfView()
	{
		return aov;
	}
	bool OnScreen(vect pos,vect dim)
	{
		if(	pos.x+dim.x>=scrpos.x
			&&pos.y+dim.x>=scrpos.y
			&&pos.z+dim.x>=scrpos.z
			&&pos.x-dim.x/2<=scrpos.x+scrdim.x
			&&pos.y-dim.y<=scrpos.y+scrdim.y
			&&pos.z-dim.z<=scrpos.z+scrdim.z)
			return true;
		else
			return false;
	}
	vect apparent_pos_of(vect pos)
	{
		vect appPos,relPos=(pos-cameraPos);
		appPos.y=(1+relPos.y/(relPos.z*tan(AngleOfView())))*scrdim.y/2;
		appPos.x=(1+relPos.x/(relPos.z*tan(AngleOfView())))*scrdim.x/2;
		return appPos;
	}
	bool mousemotion(SDL_Event ev)
	{
		if(ev.type == SDL_MOUSEMOTION )
		{

			mousepos.x = ev.motion.x;
			mousepos.y = ev.motion.y;
			mousepos.z = random(scrpos.z,(scrpos.z+scrdim.z));
			return 1;
		}
		return 0;
	}
	bool CheckCameraMovement(SDL_Event e)
	{
		vect oldcameraPos=cameraPos;
		bool new_state;
		if(e.type==SDL_KEYDOWN)
			new_state=true;
		if(e.type==SDL_KEYUP)
			new_state=false;
		switch((unsigned int)e.key.keysym.sym)
		{
		case SDLK_UP:
			up=new_state;
		break;
		case SDLK_DOWN:
			down=new_state;
		break;
		case SDLK_LEFT:
			left=new_state;
		break;
		case SDLK_RIGHT:
			right=new_state;
		break;
		}
		return oldcameraPos==cameraPos;
	}
	void MoveCamera()
	{
		if(up)
			cameraPos.z+=10;
		if(down)
			cameraPos.z-=10;
		if(left)
			cameraPos.x-=10;
		if(right)
			cameraPos.x+=10;
	}
	~PHYSIM()
	{
		ofstream allo("allocation log.txt");
		allo<<"Constructor-Destructor count="<<debug_sphere_count<<"\n";
		allo<<sphere.size()<<" spheres to delete:"<<"\n";
		allo.close();
		for(int i=sphere.size()-1;i>=0;i--)
		{
			handler=sphere[i];
			allo.open("allocation log.txt",ios::app);
			allo<<i+1<<".	deleting	"<<handler<<"\n";
			allo.close();
			delete handler;
		}
		allo.open("allocation log.txt",ios::app);
		allo<<"Constructor-Destructor count="<<debug_sphere_count<<"\n";
		allo.close();
		Mix_CloseAudio();
		SDL_FreeSurface(scr);
		SDL_Quit();
		delete error;
	}
};

void vect_line(void* PhySimObj,vect a,vect b,SDL_Color color)
{
	PHYSIM* P=(PHYSIM*)PhySimObj;
	vect appPos_a=P->apparent_pos_of(a),appPos_b=P->apparent_pos_of(b);
	Drawline(P->scr,appPos_a.x,appPos_a.y,appPos_b.x,appPos_b.y,color);
}

SPHERE::SPHERE(void* PhySimObj,SDL_Surface* texture,vect position,vect dimension,long double U_mass=1)
{
       PHYSIM* P=(PHYSIM*)PhySimObj;
       tex=texture;
       if(tex==NULL)
               debugger.found("SPHERE()","loadimage() failed");
       dim=dimension;
       general_construction();
       VisualDimensionRatio=sqrt(dim.x*dim.x+dim.y*dim.y)/sqrt(texture->clip_rect.w*texture->clip_rect.w+texture->clip_rect.h*texture->clip_rect.w*texture->clip_rect.w+texture->clip_rect.h*texture->clip_rect.h);
       if(position==randomposition)
       {
               vect from(0,0,0);
               vect to(P->scrdim.x,P->scrdim.y,P->scrdim.z);
               pos=random(from,to);
       }
       else
               pos=position;
       mas=U_mass;
}
SPHERE::SPHERE(void* PhySimObj,SDL_Surface* user_texture,long double U_mass=1)
{
       PHYSIM* P=(PHYSIM*)PhySimObj;
       tex=user_texture;
       if(tex==NULL)
               debugger.found("SPHERE()","loadimage() failed");
       dim.x=tex->w;
       dim.y=tex->h;
       dim.z=(dim.x+dim.y)/2.0;
       general_construction();
       vect from(0,0,0);
       vect to(P->scrdim.x,P->scrdim.y,P->scrdim.z);
       pos=random(from,to);
       mas=U_mass;
}
SPHERE::SPHERE(void* PhySimObj,SDL_Surface* user_texture,vect position,long double U_mass=1)
{
       PHYSIM* P=(PHYSIM*)PhySimObj;
       tex=user_texture;
       if(tex==NULL)
               debugger.found("SPHERE()","loadimage() failed");
       dim.x=tex->w;
       dim.y=tex->h;
       dim.z=(dim.x+dim.y)/2.0;
       general_construction();
       if(position==randomposition)
       {
               vect from(0,0,0);
               vect to(P->scrdim.x,P->scrdim.y,P->scrdim.z);
               pos=random(from,to);
       }
       else
               pos=position;
       mas=U_mass;
}
double SPHERE::zoomfactor(void* U)
{
	PHYSIM* P=(PHYSIM*)U;
	vect relPos=(pos-P->cameraPos);
	double RealRatio;
	if((relPos.z)==0)
		RealRatio=0.99;
	else
		RealRatio=(dim.y/relPos.z)/(tan(M_PI/4));
	if(RealRatio>0.9)
		RealRatio=0.9;
	zoom=(RealRatio*P->scrdim.y)/tex->clip_rect.h;
	return zoom;
}
vect SPHERE::apparentPos(void* U)
{
	PHYSIM* P=(PHYSIM*)U;
	vect relPos=(pos-P->cameraPos);
	appPos.y=(1+relPos.y/(relPos.z*tan(P->AngleOfView())))*P->scrdim.y/2;
	appPos.x=(1+relPos.x/(relPos.z*tan(P->AngleOfView())))*P->scrdim.x/2;
	return appPos;
}
int SPHERE::globalcollision(void* U,double deltatime)
{
	PHYSIM* P=(PHYSIM*)U;
	if(pos.y+center.y+(vel.y+acc.y*deltatime)*deltatime>P->scrpos.y+P->scrdim.y)
	{
		if(just_collided>2)
			pos.y=(scrpos.y+P->scrdim.y-dim.y)/2.0;
		if(vel.y>0)
		{
			addvel(-vel*2,vect(0,dim.y/2,0));
		}
		return ++just_collided;
	}
	else if(pos.x+center.x+(vel.x+acc.x*deltatime)*deltatime>P->scrpos.x+P->scrdim.x)
	{
		if(just_collided>2)
			pos.x=(scrpos.x+P->scrdim.x)/2.0;
		if(vel.x>0)
		{
			addvel(-vel*2,vect(dim.x/2,0,0));
		}
		return ++just_collided;
	}
	else if(pos.y-center.y+(vel.y+acc.y*deltatime)*deltatime<P->scrpos.y)
	{
		if(just_collided>2)
			pos.y=(scrpos.y)/2.0;
		if(vel.y<0)
		{
			addvel(-vel*2,vect(0,-dim.y/2,0));
		}
		return ++just_collided;
	}
	else if(pos.x-center.x+(vel.x+acc.x*deltatime)*deltatime<P->scrpos.x)
	{
		if(just_collided>2)
			pos.x=(scrpos.x)/2.0;
		if(vel.x<0)
		{
			addvel(-vel*2,vect(-dim.x/2,0,0));
		}
		return ++just_collided;
	}
	if(pos.z+center.z+(vel.z+acc.z*deltatime)*deltatime>P->scrpos.z+P->scrdim.z)
	{
		if(just_collided>2)
			pos.z=(scrpos.z+P->scrdim.z-dim.z)/2.0;
		if(vel.z>0)
		{
			vel.z=-vel.z;
		}
		return ++just_collided;
	}
	else if(pos.z-center.z+(vel.z+acc.z*deltatime)*deltatime<P->scrpos.z)
	{
		if(just_collided>2)
			pos.z=(scrpos.z)/2.0;
		if(vel.z<0)
		{
			vel.z=-vel.z;
		}
		return ++just_collided;
	}
	else return just_collided=0;
}
int SPHERE::collision(SPHERE &b)
{
	if(touched(b))
	{
		vect avel=vel;
		vect bvel=b.velocity();
		if((avel|bvel)>0)
		{
			addvel(bvel-avel,(pos-b.position()));
			b.addvel(avel-bvel,(b.position()-pos));
		}
		return just_collided=1;
	}
	return just_collided=0;
}
bool SPHERE::mash(SPHERE* &b,void* U)
{
	PHYSIM* P=(PHYSIM*)U;
	if(touched(*b))
	{
		pos*=mas;
		pos+=b->position()*b->mass();
		mas+=b->mass();
		pos/=mas;
		addmomentum(b->momentum());
		addvolume(b->volume());
		if(P->findSphere(b))
			P->delsphere(b);
		return true;
	}
	return false;
}
void SPHERE::attach(SPHERE* &b,void* U)
{
	PHYSIM* P=(PHYSIM*)U;
	if(b->independent)
	{
		if(pos.separation(b->position())<dim.mag()*2)
		{
			b->set_relative_position(pos);
			member.push_back(b);
			mas+=b->mass();
			addmomentum(b->momentum(),b->relpos_with_respect_to_parent());
			if(P->findSphere(b))
				P->delsphere(b);
		}
	}
}
void SPHERE::display(void* PhySimObject)
{
	PHYSIM* P=(PHYSIM*)PhySimObject;
	vect apparentPosition=apparentPos(P);
	if(P->OnScreen(apparentPosition,dim))
		applysurface(tex,apparentPosition,ang,zoomfactor(P));
	for(unsigned int i=0;i<number_of_springs_connected();i++)
	{
		//vect_line(PhySimObject,pos,spring_connection[i].partner->position(),spring_connection[i].spring.color);
	}
}
