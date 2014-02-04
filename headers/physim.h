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

#ifndef M_PI
#define M_PI 3.141592654
#include <math.h>
#endif

#include <headers/vect.hpp>
#include <headers/random.hpp>
#include <headers/debug.hpp>
#include <headers/global_assets.hpp>
#include <headers/framer.hpp>
using namespace std;

//header for the PHYSIM class
class PHYSIM;
//header for the PHYSIM class
class graphicstring;

/**
 * class that uses SDL to get user textual input and produce the input as a Graphic text on screen
 * It can support a max of 3 lines
 */
class graphicstringinput
{
	SDL_Rect rect[3];
	string text[3];
	SDL_Surface* image[3];
	char new_char;
	timer imagetimer;
	timer start,repeat;
	unsigned int start_time,repeat_time,graphic_update_interval,max_char;
	int lines,line,xspacing,yspacing;
	void reset()
	{
		line=done=0;
		for(int i=0;i<lines;i++)
		{
			text[i].assign("");
			rect[i].x=xspacing;
			rect[i].y=yspacing+i*global_font_size;
		}
		text[0].assign("$");
		imagetimer.reset();
		imagetimer.start();
	}
public:
	bool done;
	void renderimages(bool forced=0)
	{
		if(imagetimer.elapse()>graphic_update_interval)
		{
			for(int i=0;i<=line;i++)
			{
				if(text[i].size()>0)
				{
					if(image[i]!=NULL)
						SDL_FreeSurface(image[i]);
					image[i]=TTF_RenderText_Solid(font,text[i].c_str(),(SDL_Color){0,0xFF,0});
					rect[i].w=image[i]->w;
					rect[i].h=image[i]->h;
				}
				else image[i]=NULL;
			}
			imagetimer.reset();
			imagetimer.start();
		}
	}
	graphicstringinput(SDL_Surface* scr,unsigned int UstartT=500,unsigned int UrepeatT=25,unsigned int Ugraphic_update_interval=50)
	{
		xspacing=10;yspacing=50;
		lines=3;
		max_char=2*(scr->w-xspacing)/global_font_size;
		start_time=UstartT;repeat_time=UrepeatT;graphic_update_interval=Ugraphic_update_interval;
		reset();
		for(int i=0;i<lines;i++)
			image[i]=NULL;
		renderimages(1);
		new_char=0;
		global_graphicstring_id++;
	}
	~graphicstringinput()
	{
		for(int i=0;i<lines;i++)
			SDL_FreeSurface(image[i]);
	}
	void handle_input(SDL_Event event)
	{
		if(event.type==SDL_KEYDOWN&&!done)
		{
			if((event.key.keysym.unicode>='A'&&event.key.keysym.unicode<='Z')
			||(event.key.keysym.unicode>='a'&&event.key.keysym.unicode<='z')
			||(event.key.keysym.unicode>='0'&&event.key.keysym.sym<='9')
			||(event.key.keysym.unicode==SDLK_SPACE)
			||(event.key.keysym.unicode==SDLK_RETURN)
			||(event.key.keysym.unicode==SDLK_BACKSPACE)
			||(event.key.keysym.unicode==SDLK_TAB)
			||(event.key.keysym.unicode==SDLK_EXCLAIM)
			||(event.key.keysym.unicode==SDLK_AT)
			//||(event.key.keysym.unicode==SDLK_HASH)
			||(event.key.keysym.unicode==SDLK_DOLLAR)
			||(event.key.keysym.unicode==SDLK_QUOTEDBL)
			||(event.key.keysym.unicode=='^')
			||(event.key.keysym.unicode==SDLK_AMPERSAND)
			||(event.key.keysym.unicode==SDLK_ASTERISK)
			||(event.key.keysym.unicode==SDLK_LEFTPAREN)
			||(event.key.keysym.unicode==SDLK_RIGHTPAREN)
			||(event.key.keysym.unicode==SDLK_MINUS)
			||(event.key.keysym.unicode==SDLK_SPACE)
			||(event.key.keysym.unicode==SDLK_SPACE)
			||(event.key.keysym.unicode==SDLK_SLASH)
			||(event.key.keysym.unicode==SDLK_SPACE)
			||(event.key.keysym.unicode==SDLK_SEMICOLON)
			||(event.key.keysym.unicode==SDLK_COLON)
			||(event.key.keysym.unicode==SDLK_QUOTE)
			||(event.key.keysym.unicode==SDLK_COMMA)
			||(event.key.keysym.unicode==SDLK_SPACE)
			||(event.key.keysym.unicode==SDLK_PERIOD)
			||(event.key.keysym.unicode==SDLK_SPACE))
			{
				if(new_char!=(char)event.key.keysym.unicode)
				{
					start.reset();
				}
				start.start();
				if(new_char!=(char)event.key.keysym.unicode||start.elapse()==0||(start.elapse()>start_time&&(repeat.elapse()==0||repeat.elapse()>repeat_time)))
				{
					new_char=(char)event.key.keysym.unicode;
					switch(new_char)
					{
					case 0:
						//do nothing if no key is pressed...
					break;
					case SDLK_BACKSPACE:
						if(text[line].size()>1)
							text[line].erase(text[line].size()-1);
						else if(line>0)
						{
							text[line]="";
							line--;
						}
					break;
					case SDLK_RETURN:
						finished();
					break;
					default:
						if(text[line].size()<max_char)
							text[line]+=new_char;
						else if(line+1<lines)
						{
							line++;
							text[line]+=new_char;
						}
					break;
					}
				}

				if(start.elapse()>start_time)
				{
					if(repeat.elapse()>repeat_time)
						repeat.reset();
					repeat.start();
				}
			}
		}
		else
		{
			start.reset();
		}
	}
	void display(SDL_Surface* scr)
	{
		renderimages();
		for(int i=0;i<=line&&image[i]!=NULL;i++)
			SDL_BlitSurface(image[i],NULL,scr,&rect[i]);
	}
	void get(string &temp)
	{
		temp=text[0].substr(1,text[0].size()-1);
		temp+=text[1].substr(0,text[1].size());
		temp+=text[2].substr(0,text[2].size());
	}
	void finished(bool U=1)
	{
		done=U;
		if(!U)
		{
			reset();
		}
	}
};

/**
 * changes the color of the pixel at (x,y) of the SDL_Surface that is passed using the color that is passed
 */
void put_pixel( SDL_Surface *surface, int x, int y, SDL_Color color)
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;
    Uint32 pixel=SDL_MapRGB(surface->format,255, 0, 0);
    //Set the pixel
    pixels[ ( y * surface->w ) + x ] = pixel;
}
/**
 * draws a line to the SDL_Surface that is passed from coordinates (x1,y1) to (x2,y2)
 * using the color that is passed int the SDL_Color variable
 * (by simply calls the put_pixel() many times)
 */
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
/**
 * Similar to the Drawline() exept it accepts vect variables as coordinates instead of (float,float)
 */
void Draw_vect_Line(PHYSIM &PhySimObj,vect a,vect b,SDL_Color color);

/**
 * A simple structure that stores information about the properties of a spring
 */
struct SPRING
{
	double l,k;	//l(spring length),k(spring constant)
	SDL_Color color;	//color(spring color)
};

/**
 * A class representing all the features of a primitive physical object.
 * An ideal class from which other classes may be derived
 */
class PARTICLE
{
protected:
	/**
	 * A reference to the main PhySim object (representing the physical scene in which this object has been created
	 * In other world it is a way by which this object can access information about the properties of
	 * the world it was created in!
	 */
	PHYSIM &P;
	/**
	 * An integer that stores a unique ID that is provided by the PHYSIM class.
	 * It may be used to uniquely identify every element that is generated!
	 */
	int tag;
	SDL_Surface* tex;	//Variable into which the graphics (texture) of this object is stored
	/**
	 * various physical properties like:
	 * pos(position of the object)
	 * appPos(apparent position of the object with respect to the camera that is observing it (note that since it is the apparent position of the object as seen on the 2D screen (your monitor), the z coordinate is not used)
	 * vel(velocity of the object)
	 * acc(acceleration of the object)
	 * f(net force acting on the object)
	 */
	vect pos,appPos,vel,acc,f;
	/**
	 * mas(mass of the object)
	 * zoom(ratio of the resolution of the original image (stored in the 'tex' variable) to that as seen on the screen (your monitor))
	 * VisualDimensionRatio(A simple ration that is used for convenience by the programmer to control the amount different objects are scaled (grown or shrunk) on the screen)
	 */
	long double mas,zoom,VisualDimensionRatio;
	/**
	 * bool variable that says whether this object is a part of a larger more complex object (mother object),
	 * or whether this is an independent object
	 */
	bool independent;
	vect relpos_parent;	//relative position of this object with respect to the object it is a part of (if it is part of a larger object)
	vector<PARTICLE*> member;	//pointers to all the daughter particles (objects that are a part of this (mother) object
	/**
	 * continuous_contact_n(variable that counts how many consecutive frames this object has been touching another one)
	 * latest_collision(variable that stores what time (in milliseconds) the last collision was)
	 */
	unsigned int continuous_contact_n,latest_collision;
	/**
	 * function that gets information about the camera from the PHYSIM reference (P)
	 * and returns a vector representing the apparent 2D coordinates (on the screen)
	 * where the user should observe it
	 */
	vect apparentPos();
	/**
	 * A function that is commonly run by all the PARTICLE constructors to set some general values to variables
	 */
	virtual void general_PARTICLE_construction()
	{
		VisualDimensionRatio=1;
		continuous_contact_n=0;
		latest_collision=-1;
		independent=true;
		mas=1;
		zoom=1;
	}
	/**
	 * structure that stores information about how a spring is connected to this object
	 */
	struct SPRING_CONNECTION
	{
		SPRING spring;
		PARTICLE* partner;
		SPRING_CONNECTION(PARTICLE* Partner,double rest_length,double spring_constant,SDL_Color color=(SDL_Color){0,255,0})
		{
			spring.l=rest_length;
			spring.k=spring_constant;
			partner=Partner;
		}
	};
	/**
	 * a vector that stores a list of all current spring connections made to this object
	 */
	vector<SPRING_CONNECTION> spring_connection;
public:
	/**
	 * constructor that attains information about the scene(PHYSIM object) it was created in, its texture and mass to generate a PARTICLE
	 */
	PARTICLE(PHYSIM &PhySimObj,SDL_Surface* texture,long double U_mass);
	int get_tag()	//access function to it tag
	{
		return tag;
	}
	bool isindependent()	//access function to bool independent
	{
		return independent;
	}
	vect position()	//access function to pos
	{
		return pos;
	}
	vect relpos_with_respect_to_parent()	//access function to relpos_parent
	{
		return relpos_parent;
	}
	vect velocity()	//access function to vel
	{
		return vel;
	}
	long double mass()	//access function to mas
	{
		return mas;
	}
	vect momentum()	//function that returns the momentum of this object
	{
		return vel*mas;
	}
	vect addvel(vect b)	//function that adds the velocity passed to its own
	{
		vel+=b;
		return vel;
	}
	vect addmomentum(vect momentum)	//function that adds the velocity passed to its own
	{
		return vel+=momentum/mas;
	}
	vect addacc(vect b)	//function that adds the acceleration passed to its own
	{
		acc+=b;
		return acc;
	}
	vect addforce(vect b)	//function that adds the force passed to its own
	{
		f+=b;
		return f;
	}
	long double addmass(long double mass)	//function that adds the mass passed to its own
	{
		mas+=mass;
		return mas;
	}

	void PARTICLE_integrate(double deltatime)	//integrates the motion variables (using the 3 laws of motion equations)
	{
		if(deltatime==0)
			debugger.found("integrate()","deltatimevalue=0");
		acc+=(f/mas);
		vect u=vel;
		vel+=acc*deltatime;	//v=u+at
		pos+=u*deltatime;		//s=s0+ut
		pos+=acc*0.5*deltatime*deltatime;	//s=s0+a*t^2
	}
	void gravity(PARTICLE* &b)	//function that calculates and adds the gravitational force between this object and the one passed
	{
		vect relpos=(b->position()-pos);
		f+=relpos.dir()*G*mas*b->mass()/(relpos.mag())/relpos.mag();
	}
	unsigned int number_of_springs_connected()	//returns the number of springs currently connected to this object
	{
		return spring_connection.size();
	}
	bool spring_connected(PARTICLE* b)	//checks whether a spring is connected between this object and the one passed
	{
		for(unsigned int i=0;i<spring_connection.size();i++)
		{
			if(spring_connection[i].partner==b)
				return true;
		}
		return false;
	}
	bool connect_spring(PARTICLE* &b,double rest_length,double spring_constant)	//attaches a spring between this object and the one passed
	{
		bool spring_is_connected=spring_connected(b);
		if(!spring_is_connected)
			if(spring_connection.size()<1)
				spring_connection.push_back(SPRING_CONNECTION(b,rest_length,spring_constant));
		return spring_is_connected;
	}
	vect spring(PARTICLE &b)	//function that first checks if the passed spring is connected to this object and then calculated and adds the forces between them
	{
		for(unsigned int i=0;i<spring_connection.size();i++)
		{
			if(spring_connection[i].partner==&b)
			{
				vect relpos=(b.position()-pos);
					//force=	dir		*		extension							*	spring constant
				return f+=(relpos.dir()*(relpos.mag()-spring_connection[i].spring.l)*spring_connection[i].spring.k);
			}
		}
		return (vect){0,0,0};
	}
	virtual int globalcollision(double deltatime);	//virtual function that first checks if the object has collided with the walls of the scene and then causes it to bounce of that wall it collided with(if any)
	virtual void attach(PARTICLE* &b);	//virtual function that attaches the passed PARTICLE to this one
	vect set_relative_position(vect parent_position)	//makes this object a daughter object of the passed particle
	{
		independent=false;
		return relpos_parent=pos-parent_position;
	}
	unsigned int set_collision_time()	//stores what time (in milliseconds) the last collision was
	{
		return latest_collision=SDL_GetTicks();
	}
	unsigned int time_since_last_collision()	//returns how long ago the last collision was (in milliseconds) from the current time
	{
		return SDL_GetTicks()-latest_collision;
	}
	unsigned int just_collided(bool collision)	//checks whether the object just collided
	{
		if(collision)
		{
			set_collision_time();
			return ++continuous_contact_n;
		}
		else
			return continuous_contact_n=0;
	}
	unsigned int continuous_contact()	//access function to continuous_contact_n
	{
		return continuous_contact_n;
	}
	void newframe()	//prepares the object for anew frame by reseting the force and acceleration variable
	{
		f=(vect){0,0,0};
		acc=(vect){0,0,0};
	}
	virtual ~PARTICLE()	//deallocates the 'tex' variable before destroying the object
	{
		if(tex)
			SDL_FreeSurface(tex);
	}
};
/**
 * A primitive physical object representing a SPHERE.
 * It is derived from the PARTICLE class with additional features related to spheres
 */
class SPHERE:public PARTICLE
{
protected:
	/**
	 * dim(describes the size of the sphere)
	 * center(points to the center of the sphere in local space)
	 * ang(angular displacement of the sphere)
	 * tta(angular velocity of the sphere)
	 * tor(angular torque of the sphere)
	 */
	vect dim,center,ang,tta,tor;
	double zoomfactor();	//calculates the factor by which the object is to be scaled to accurately represent its 3D depth virtually on the 2D screen
	/**
	 * A function that is commonly run by all the PARTICLE constructors to set some general values to variables
	 */
	virtual void general_SPHERE_construction()
	{
		general_PARTICLE_construction();
		center=dim/2;
	}
public:
	/**
	 * overloaded constructors with default valuese representing the various properties the SPHERE can attain while be generated...
	 */
	SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,vect position,vect dimension,long double U_mass);
	SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,long double U_mass);
	SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,vect U_pos,long double U_mass);
	long double diameter()	//returns the diameter of the SPHERE
	{
		return dim.mag();
	}
	virtual long double volume()	//calculates and returns the volume of the SPHERE
	{
		return 4*M_PI*pow((diameter()/2.0),3)/3.0;
	}
	vect addtta(vect u_tta)	//adds a little more angular rotation (spin) to the SPHERE
	{
		return tta+=u_tta;
	}
	/**
	 * 2 parameters:
	 * u_vel(the amount of velocity to add to the ball)
	 * u_pos(the position on the ball onto which this velocity is added determines how much the ball will end up spinning)
	 */
	vect addvel(vect u_vel,vect u_pos)
	{
		tta+=((u_vel^u_pos)/pow(u_pos.mag(),2));
		return vel+=u_pos.dir()*((u_vel|u_pos)/u_pos.mag());
	}
	vect addmomentum(vect momentum,vect u_pos)	//similar to addvel() exept momentum is being added!
	{
		return addvel(momentum/mas,u_pos);
	}
	long double addvolume(long double volume)	//adds a little volume to the SPHERE
	{
		long double new_diameter=pow(pow((double)diameter(),3)+8*3*volume/4.0/M_PI,1/3.0);
		dim*=new_diameter/diameter();
		return volume;
	}
	bool touched(SPHERE &b)	//checks whether the SPHERE ot touched by another SPHERE
	{
		return pos.separation(b.position())<(diameter()/2.0+b.diameter()/2.0);
	}
	/**
	 * displays the current SPHERE on the screen...
	 * whose details are provided by accessing the information about the scene it was created in...
	 * by using the PHYSIM object
	 */
	void display();
	/**
	 * displays this object as a member (daughter) of another (mother) object
	 * It is usually called by the mother cell in its display()
	 */
	bool display_as_member(SPHERE &parent);
	/**
	 * similar to the integrate function used in the PARTICLE class except it also integratess the rotationsal ascpects
	 */
	void integrate(double deltatime)
	{
		PARTICLE_integrate(deltatime);
		tta+=tor*deltatime;
		ang+=tta*deltatime;
		ang+=tor*0.5*deltatime*deltatime;
	}
	void gravity(SPHERE* &b)	//similar to PARTICLE::gravity()
	{
		vect relpos=(b->position()-pos);
		f+=relpos.dir()*G*mas*b->mass()/(relpos.mag())/relpos.mag();
	}
	virtual int globalcollision(double deltatime);	//similar to PARTICLE::globalcollision() except it takes into account the size and shape of the sphere
	int collision(SPHERE &b);	//causes this SPHERE to collided with the SPHERE that is passed if they are in contact!
	bool mash(SPHERE &b);	//mashes the SPHERE passed to this SPHERE into 1 larger and more massive SPHERE
	void attach(SPHERE* &b);	//attaches the passed SPHERE to this SPHERE or vice versa appropriately
};

/**
 * stores properties of a particular material
 * Mainly involved with properties that are independent on its amount
 */
struct MATERIAL
{
	double density,viscosity,energy_factor;	//energy_factor(the chemical equivalent energy on consumption)
	bool operator==(MATERIAL b)
	{
		return (
				density==b.density&&
				viscosity==b.viscosity&&
				energy_factor==b.energy_factor);
	}
	MATERIAL operator+=(MATERIAL b)
	{
		density=(density+b.density)/2;
		viscosity=(viscosity+b.viscosity)/2;
		energy_factor=(energy_factor+b.energy_factor)/2;
		return *this;
	}
};
/**
 * stores information about a particular substance
 * Mainly involved with properties that are dependent on its amount
 */
struct SUBSTANCE
{
	MATERIAL material;
	double mass;
	double energy_equivalence()
	{
		return mass*material.energy_factor;
	}
	double volume()
	{
		return mass/material.density;
	}
	SUBSTANCE operator+=(SUBSTANCE b)	//combines 2 SUBSTANCES
	{
		mass+=b.mass;
		material+=b.material;
		return *this;
	}
};
/**
 * stores information about the hereditary features of a life form
 */
struct DNA
{
	double visibility,baby_energy;
	SUBSTANCE cytoplasm,baby_fat,organelles;
	double total_volume()
	{
		return organelles.volume()+baby_fat.volume()+cytoplasm.volume();
	}
	double total_mass()
	{
		return organelles.mass+baby_fat.mass+cytoplasm.mass;
	}
	vect dimensions()
	{
		long double radius=pow(total_volume()/(4/3*M_PI),1/3.0);
		return (vect){radius,radius,radius};
	}
	//cell memberane mass
};
/**
 * represents a single celled organism
 * class derived from SPHERE
 */
class CELL: public SPHERE
{
protected:
	DNA dna;	//variable that stores hereditary information of the CELL
	/**
	 * visib(controls how far around the CELL can see other cells)
	 * E(stores how much Energy the cell currently has at its disposal)
	 */
	double visib,E;
	void inherit_DNA()	//inherits all the DNA information from its DNA
	{
		cytoplasm=dna.cytoplasm;
		food_reserve=dna.baby_fat;
		organelles=dna.organelles;
		E=dna.baby_energy;
		visib=dna.visibility;
	}
	vect calculated_dimensions()	//calculates and returns the dimensions of the CELL
	{
		long double radius=pow(volume()/(4/3*M_PI),1/3.0);
		return (vect){radius,radius,radius};
	}
public:
	SUBSTANCE food_reserve,organelles,cytoplasm,waste;
	double total_mass()	////calculates and returns the total mass of the CELL
	{
		return organelles.mass+food_reserve.mass+cytoplasm.mass;
	}
	long double volume()	//calculates and returns the volume of the CELL
	{
		return organelles.volume()+food_reserve.volume()+cytoplasm.volume();
	}
	long double visibility()	//Accesses the visibility of the CELL
	{
		return visib;
	}
	double energy()	//Accesses the Energy of the CELL
	{
		return E;
	}
	double energy_equivalence()	//Accesses the Energy another CELL can obtain if it consumes this CELL
	{
		return E+food_reserve.energy_equivalence();
	}
	bool touched(CELL &b)	//checks if this CELL has been touched(similar to that in SPHERE)
	{
		return pos.separation(b.position())<(diameter()/2.0+b.diameter()/2.0);
	}
	int collision(CELL &b);	//causes this cell to collide with the CELL passed
	bool mash(CELL &b);	//causes this cell to mash up with the CELL passed (similar to SPHERE::mash())
	/*
	 * constructors that represents the various ways in which the CELL can be created
	 */
	CELL(PHYSIM &PhySimObj,SDL_Surface* texture,vect position,DNA seed_DNA)	:	SPHERE(PhySimObj,texture,position,seed_DNA.dimensions(),seed_DNA.total_mass()),dna(seed_DNA)
	{
		inherit_DNA();
	}
	CELL(PHYSIM &PhySimObj,SDL_Surface* texture,long double Visibility,long double U_mass)	:	SPHERE(PhySimObj,texture,U_mass)
	{
		visib=Visibility;
		inherit_DNA();
	}
	CELL(PHYSIM &PhySimObj,SDL_Surface* texture,vect U_pos,long double Visibility,long double U_mass)	:	SPHERE(PhySimObj,texture,U_pos,U_mass)
	{
		visib=Visibility;
		inherit_DNA();
	}
	CELL* find_food();	//function that returns the pointer to the best CELL within its visibility range in terms of its Energy equivalence
	CELL* spot_predators();	////function that returns the pointer to predator CELL within its visibility range
};

/**
 * most significant class and contains all the information regarding the properties of a physical world
 */
class PHYSIM
{
	unsigned int tag_provider;	//variable that increments every time an object is created
	SPHERE* handler;	//a pointer to a SPHERE used to temporarily store addresses in various processes
	double aov;	//"angle of view": the angle that spans the visibility of the camera
	void frametermination()	//performs actions that are to be done at the end of a frame (like delaying if all frame processes finished early)
	{
		SDL_Delay(frametimer.remainingfreetime()*1000.0);
		frametimer.endframe();
	}
	void general_construction()	//general actions that are done common to all PHYSIM constructors
	{
		camera_speed=10;
		frametimer.updatefpslimits(10,30);
		runtime.start();
		error=new DEBUG(file_loc(buf,log_loc,"psm"));
		handler=NULL;
		aov=M_PI/4.0;
		if(SDL_Init(SDL_INIT_EVERYTHING)==-1)
			error->found((char*)"PHYSIM()",(char*)"SDL_Init() failed");
		if( TTF_Init()==-1 )
			error->found((char*)"PHYSIM()",(char*)"TTF_Init() failed");
		if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
			error->found((char*)"PHYSIM()",(char*)"Mix_OpenAudio() failed");
		srand(SDL_GetTicks());
		ended=false;
	}
	SPHERE* general_gensphere(SPHERE* handler)	//common actions that are to be done while generating any SPHERE
	{
		if(handler)
		{
			spheres.push_back(handler);
		}
		return handler;
	}
public:
	SDL_Surface* scr;
	/**
	 * (scrpos)	represents the origin of the physical world (and is generally (0,0,0))
	 * (scrdim)	represents the dimensions of the physical world (if active, objects will collide of these walls)
	 * (mousepos) stores the current position coordinates of the mouse pointer
	 * (cameraPos) stores the current position of the camera
	 * (cameraVel) stores the current velocity with which the camera is to move
	 */
	vect scrpos,scrdim,mousepos,cameraPos,cameraVel;
	long double camera_speed;	//stores how much speed with which the camera is to move in a particular direction
	vector<void*> objects;	//vector of objects that could potentially point to any kind of object
	vector<PARTICLE*> particles;	//vector of PARTICLEs that point to all particles currently on screen
	vector<SPHERE*> spheres;	//vector of SPHEREs that point to all SPHEREs currently on screen
	vector<CELL*> cells;	//vector of CELLs that point to all CELLs currently on screen
	int bpp;	//stores amount of bits that are to be stored per pixel
	DEBUG* error;	//pointer variable error of the type DEBUG
	timer runtime;	//used to measure for how long the program has been running
	framer frametimer;	//used to control the framerate at which the program runs
	bool ended;	//used to control when the program ends

	/**
	 * used to apply an SDL_Surface onto the scr(SDL_Surface variable that represents the screen)
	 * at the coordinates given by pos
	 */
	void applysurface(SDL_Surface* image,vect pos=(vect){0,0,0} )
	{
		if(scr==NULL)
			debugger.found("applysurface()","::scr is pointing to NULL");
		else
		{
			SDL_Rect temp={pos.x,pos.y};
			SDL_BlitSurface(image,NULL,scr,&temp);
		}
	}
	/**
	 * used to apply an SDL_Surface onto the scr(SDL_Surface variable that represents the screen)
	 * at the coordinates given by pos
	 * after being rotated by a user_angle
	 * and scaled up by a factor of 'zoom'
	 */
	void applysurface(SDL_Surface* image,vect pos,vect user_angle,double zoom)
	{
		if(scr==NULL)
			debugger.found("applysurface()","::scr is pointing to NULL");
		else
		{
			image=rotozoomSurface(image,user_angle.z,zoom,0);
			SDL_Rect temp={pos.x-image->w/2.0,pos.y-image->h/2.0};
			if(image==NULL)
					debugger.found("applysurface()","rotozoom() causes image to point to NULL");
			SDL_BlitSurface(image,NULL,scr,&temp);
			SDL_FreeSurface(image);//frees newly created rotated image (retaining the original surface passed to applysurface!)
		}
	}
	/**
	 * draws a line on the screen from 'a'to 'b' with the color 'color'
	 * it basically calls the Drawline()
	 */
	void vect_line(vect a,vect b,SDL_Color color)
	{
		vect appPos_a=apparent_pos_of(a),appPos_b=apparent_pos_of(b);
		if(
			appPos_a.x>scr->clip_rect.x+50
			&&appPos_b.x<scr->clip_rect.x+scr->clip_rect.w-50
			&&appPos_a.y>scr->clip_rect.y+50
			&&appPos_b.y<scr->clip_rect.y+scr->clip_rect.h-50
			)
			Drawline(scr,appPos_a.x,appPos_a.y,appPos_b.x,appPos_b.y,color);
	}
	/**
	 * constructor that creates a physical world that:
	 * has 'user_dim' dimensions,
	 * has user_pos as origin,
	 * has user_bpp as the bits per pixel of its screen
	 */
	PHYSIM(vect user_dim,vect user_pos=(vect){0,0,0},int user_bpp=32)
	{
		general_construction();
		bpp=user_bpp;
		::scrdim=scrdim=user_dim;
		cameraPos=user_dim/2;
		cameraPos.z=-200;
		::scrpos=scrpos=user_pos;
		scr=SDL_SetVideoMode(scrdim.x,scrdim.y,bpp,SDL_SWSURFACE|SDL_RESIZABLE);
	}
	/**
	 * used to provide a unique id to a object that has just been created and
	 * outputs certain information about the creation of the  particle to a log
	 */
	unsigned int object_created(PARTICLE* address)
	{
		ofstream fout("logs/allocation log.txt",ios::app);
		fout<<"Tag number "<<address->get_tag()<<": Particle number "<<(++tag_provider)<<" created at memory location "<<address<<'\n';
		return tag_provider;
	}
	//represents a set of functions that can be used to generate SPHERES on screen with specific properties
	SPHERE* gensphere(SDL_Surface* texture,long double U_mass=1)
	{
		handler=new SPHERE(*this,texture,U_mass);
		if(handler)
			return general_gensphere(handler);
		else
			return NULL;
	}
	SPHERE* gensphere(SDL_Surface* texture,vect position,vect dimension,long double U_mass=1)
	{
		handler=new SPHERE(*this,texture,position,dimension,U_mass);
		if(handler)
			return general_gensphere(handler);
		else
			return NULL;
	}
	SPHERE* gensphere(SDL_Surface* texture,vect position,long double U_mass=1)
	{
		handler=new SPHERE(*this,texture,position,U_mass);
		if(handler)
			return general_gensphere(handler);
		else
			return NULL;
	}
	int findParticle(PARTICLE* U)	//searches through the particles vector for a pointer to a particle that matches the one passed
	{
		for(unsigned int i=0;i<particles.size();i++)
		{
			if(particles[i]==U)
				return i;
		}
		return -1;
	}
	int findSphere(SPHERE* U)	//searches through the spheres vector for a pointer to a sphere that matches the one passed
	{
		for(unsigned int i=0;i<spheres.size();i++)
		{
			if(spheres[i]==U)
				return i;
		}
		return -1;
	}
	int findCell(CELL* U)	//searches through the cells vector for a pointer to a cell that matches the one passed
	{
		for(unsigned int i=0;i<cells.size();i++)
		{
			if(cells[i]==U)
				return i;
		}
		return -1;
	}
	bool delParticle(PARTICLE* U=NULL)	//destroys the particle from the particles vector that matches the one passed
	{
		if(U==NULL)
		{
			if(particles.size()>1)
				particles.pop_back();
			return true;
		}
		else
		{
			int result=findParticle(U);
			if(result!=-1)
			{
				particles.erase(particles.begin()+result);
				return true;
			}
		}
		return false;
	}
	bool delsphere(SPHERE* U=NULL)	//destroys the sphere from the spheres vector that matches the one passed
	{
		if(U==NULL)
		{
			if(spheres.size()>1)
				spheres.pop_back();
			return true;
		}
		else
		{
			int result=findSphere(U);
			 if(result!=-1)
			 {
				 spheres.erase(spheres.begin()+result);
				 return true;
			 }
		}
		return false;
	}
	bool delCell(CELL* U=NULL)	//destroys the cell from the cells vector that matches the one passed
	{
		if(U==NULL)
		{
			if(cells.size()>1)
				cells.pop_back();
			return true;
		}
		else
		{
			int result=findCell(U);
			 if(result!=-1)
			 {
				 cells.erase(cells.begin()+result);
				 return true;
			 }
		}
		return false;
	}
	bool delsphere(unsigned int U)	//destroys the sphere from the spheres vector that matches the one passed
	{
		if(U<spheres.size())
			spheres.erase(spheres.begin()+U);
		return false;
	}
	bool delparticle(unsigned int U)	//destroys the particle from the particles vector that matches the one passed
	{
		if(U<spheres.size())
			spheres.erase(spheres.begin()+U);
		return false;
	}
	void initiateframe()	//tasks that need to be run at the start of every frame
	{
		frametimer.newframe();
	}
	/**
	 * tasks that need to be run at the end of every frame
	 * like waiting if the frame finished ahead of schedule
	 * and displaying a background color for the next frame
	 */
	void terminateframe(SDL_Color user_color)
	{
		SDL_Flip(scr);
		SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,user_color.r,user_color.g,user_color.b));
		frametermination();
	}
	/**
	 * tasks that need to be run at the end of every frame
	 * like waiting if the frame finished ahead of schedule
	 * and displaying a background color for the next frame
	 */
	void terminateframe(SDL_Surface* user_background)
	{
		SDL_Flip(scr);
		applysurface(user_background);
		frametermination();
	}
	/**
	 * sorts the spheres based on distance from the camera to ensure closer spheres are displayed in from on distant ones
	 */
	void DisplaySortSpheres()
	{
		for(unsigned int i=1;i<spheres.size();i++)
		{
			for(unsigned int j=1;j<spheres.size()-i;j++)
			{
				if((spheres[j]->position().z-cameraPos.z)<(spheres[j+1]->position().z-cameraPos.z))
				{
					SPHERE* temp=spheres[j];
					spheres[j]=spheres[j+1];
					spheres[j+1]=temp;
				}
			}
		}
	}
	/**
	 * sorts the cells based on distance from the camera to ensure closer cells are displayed in from on distant ones
	 */
	void DisplaySortCells()
	{
		for(unsigned int i=1;i<cells.size();i++)
		{
			for(unsigned int j=1;j<cells.size()-i;j++)
			{
				if((cells[j]->position().z-cameraPos.z)<(cells[j+1]->position().z-cameraPos.z))
				{
					CELL* temp=cells[j];
					cells[j]=cells[j+1];
					cells[j+1]=temp;
				}
			}
		}
	}
	double AngleOfView()	//accesses aov
	{
		return aov;
	}
	bool OnScreen(vect pos,vect dim)	//checks of a particular coordinate and dimension is on the screen or off it
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
	bool InFrontOfCamera(vect pos)	//checks if a particular 3D coordinate is in front of the camera
	{
		if((pos-cameraPos).z>0)
			return true;
		else
			return false;
	}
	/**
	 * gets information about the camera from the PHYSIM reference (P)
	 * and returns a vector representing the apparent 2D coordinates (on the screen)
	 * where the user should observe it
	 */
	vect apparent_pos_of(vect pos)
	{
		vect appPos,relPos=(pos-cameraPos);
		appPos.y=(1+relPos.y/(relPos.z*tan(AngleOfView())))*scrdim.y/2;
		appPos.x=(1+relPos.x/(relPos.z*tan(AngleOfView())))*scrdim.x/2;
		return appPos;
	}
	bool mousemotion(SDL_Event ev)	//handles events related to mouse motion
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
	vect cameraPosition()	//accesses cameraPos
	{
		return cameraPos;
	}
	bool HandleCameraMovement(SDL_Event e)	//handles events related to camera motion
	{
		vect oldcameraPos=cameraPos;
		switch(e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			switch(e.button.button)
			{
			case SDL_BUTTON_WHEELUP:
				cameraPos.z+=camera_speed;
			break;
			case SDL_BUTTON_WHEELDOWN:
				cameraPos.z-=camera_speed;
			break;
			}
		break;
		}
		switch(e.type)
		{
		case SDL_KEYDOWN:
			switch((unsigned int)e.key.keysym.sym)
			{
			case SDLK_UP:
				cameraVel.y=-camera_speed;
			break;
			case SDLK_DOWN:
				cameraVel.y=camera_speed;
			break;
			case SDLK_LEFT:
				cameraVel.x=-camera_speed;
			break;
			case SDLK_RIGHT:
				cameraVel.x=camera_speed;
			break;
			}
		break;
		case SDL_KEYUP:
			switch((unsigned int)e.key.keysym.sym)
			{
			case SDLK_UP:
				cameraVel.y=0;
			break;
			case SDLK_DOWN:
				cameraVel.y=0;
			break;
			case SDLK_LEFT:
				cameraVel.x=0;
			break;
			case SDLK_RIGHT:
				cameraVel.x=0;
			break;
			}
		break;
		}
		return oldcameraPos==cameraPos;
	}
	void MoveCamera()	//changes the camera position based on it current velocity
	{
		cameraPos+=cameraVel;
	}
	vect random_position()	//returns a random position in the physical worls
	{
        vect from(0,0,0);
        vect to=scrdim;
        return random(from,to);
	}
	~PHYSIM()	//deallocates all allocated memory and generates status log files,etc
	{
		char log_file_location[50];
		file_loc(log_file_location,log_loc,"allocation log.txt");
		ofstream allo(log_file_location);
		allo<<spheres.size()<<" spheres to delete:"<<"\n";
		allo.close();
		allo.open(log_file_location,ios::app);
		allo<<cells.size()<<" cells to delete:"<<"\n";
		allo.close();
		if(!cells.empty())
		{
			for(int i=cells.size()-1;i>=0;i--)
			{
				handler=cells[i];
				allo.open(log_file_location,ios::app);
				allo<<i+1<<".	deleting	"<<handler<<"\n";
				allo.close();
				delete handler;
			}
		}
		if(!spheres.empty())
		{
			for(int i=spheres.size()-1;i>=0;i--)
			{
				handler=spheres[i];
				allo.open(log_file_location,ios::app);
				allo<<i+1<<".	deleting	"<<handler<<"\n";
				allo.close();
				delete handler;
			}
		}
		SDL_FreeSurface(scr);
		Mix_CloseAudio();
		TTF_Quit();
		SDL_FreeSurface(scr);
		SDL_Quit();
		delete error;
	}
};

/**
 * class that uses SDL to produce and display Graphic texts on screen
 * It can support a max of 3 lines
 */
class graphicstring
{
	timer imagetimer;
	unsigned int graphic_update_interval,max_char;
	int lines,line,xspacing,yspacing;
	SDL_Rect rect[3];
	string text[3];
	string all;
	TTF_Font* font;
	SDL_Color TextColor;
	void general_set()
	{
		for(int i=0;i<lines;i++)
		{
			text[i].clear();
		}
		unsigned int progress=0;
		line=-1;
		while(progress<all.size()&&line<lines-1)
		{
			line++;
			all.push_back('\0');
			text[line].assign(all.substr(progress,max_char<(all.size()-progress)?max_char:(all.size()-progress)));
			progress+=text[line].size();
			bool stop=false;
			for(unsigned int i=0;i<text[line].size()&&!stop;i++)
			{
				if((text[line][i]=='\n'&&text[line][i+1]!='\0')&&line<lines-1)
				{
					text[line+1].assign(text[line].substr(i+1,text[line].size()-(i+2)));
					text[line].erase(i,text[line].size());
					stop=true;
					line++;
				}
			}
		}
		renderimages(1);
	}
protected:
	void renderimages(bool forced=0)
	{
		if(imagetimer.elapse()>graphic_update_interval||forced)
		{
			for(int i=0;i<=line;i++)
			{
				if(text[i].size()>0)
				{
					if(image[i]!=NULL)
						SDL_FreeSurface(image[i]);
					if(font)
						image[i]=TTF_RenderText_Solid(font,text[i].c_str(),TextColor);
					rect[i].w=image[i]->w;
					rect[i].h=image[i]->h;
				}
				else image[i]=NULL;
			}
			imagetimer.reset();
			imagetimer.start();
		}
	}
public:
	SDL_Surface* image[3];
	bool done;
	void set(string newstring)
	{
		all.assign(newstring);
		general_set();
	}
	void set(const char* U)
	{
		all.assign(U);
		general_set();
	}
	void set(int i)
	{
		char U[10];
		itoa(i,U,10);
		all.assign(U);
		general_set();
	}
	void set(double d)
	{
		char U[10];
		sprintf(U,"%f",d);
		all.assign(U);
		general_set();
	}
	void set_color(SDL_Color Ucol)
	{
		TextColor=Ucol;
	}
	void set_font(int U_font_size,const char* font_loc="physim/fonts/lazy.ttf")
	{
		font=TTF_OpenFont(font_loc,U_font_size);
		renderimages(1);
	}
	void set_color(unsigned int r,unsigned int g,unsigned int b)
	{
		TextColor.r=r;
		TextColor.g=g;
		TextColor.b=b;
	}
	void set_position(unsigned int x,unsigned int y)
	{
		xspacing=x;yspacing=y;
		for(int i=0;i<lines;i++)
		{
			rect[i].x=xspacing;
			rect[i].y=yspacing+(i)*global_font_size;
		}
	}
	void set_update_interval(unsigned int Ugraphic_update_interval=50)
	{
		graphic_update_interval=Ugraphic_update_interval;
	}
	graphicstring(PHYSIM* PhySimObj,string U_text="$",int font_size=28,const char* font_loc="Fonts/KeraterMedium.ttf",unsigned int Ugraphic_update_interval=50)
	{
		lines=3;
		line=done=0;
		xspacing=10;yspacing=10;
		//max_char=2*(scr->w-xspacing)/global_font_size;
		graphic_update_interval=Ugraphic_update_interval;
		TextColor.r=255;
		TextColor.g=0;TextColor.b=0;
		set_font(font_size,font_loc);
		for(int i=0;i<lines;i++)
		{
			text[i]="";
			rect[i].x=xspacing;
			rect[i].y=yspacing+(global_graphicstring_id*(lines+1)+i)*global_font_size;
			image[i]=NULL;
		}
		global_graphicstring_id++;
		set(U_text);
		renderimages(1);
		imagetimer.start();
	}
	~graphicstring()
	{
		for(int i=0;i<lines;i++)
			SDL_FreeSurface(image[i]);
		if(font!=::font)
			TTF_CloseFont(font);
	}
	void display(PHYSIM &P)
	{
		renderimages();
		for(int i=0;i<=line&&image[i]!=NULL;i++)
			SDL_BlitSurface(image[i],NULL,P.scr,&rect[i]);
	}
	const char* c_str()
	{
		return all.c_str();
	}
};

/**
 * various other member functions of classes that could not be defined inside the class
 */


PARTICLE::PARTICLE(PHYSIM &PhySimObj,SDL_Surface* user_texture,long double U_mass=1):P(PhySimObj),tag(P.object_created(this))
{
	tex=user_texture;
	if(tex==NULL)
	   debugger.found("SPHERE()","loadimage() failed");
	general_PARTICLE_construction();
	vect from(0,0,0);
	vect to(P.scrdim.x,P.scrdim.y,P.scrdim.z);
	pos=random(from,to);
	mas=U_mass;
}
SPHERE::SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,vect position,vect dimension,long double U_mass=1):PARTICLE(PhySimObj,texture,U_mass)
{
       tex=texture;
       if(tex==NULL)
               debugger.found("SPHERE()","loadimage() failed");
       dim=dimension;
       general_SPHERE_construction();
       VisualDimensionRatio=sqrt(dim.x*dim.x+dim.y*dim.y)/sqrt(texture->clip_rect.w*texture->clip_rect.w+texture->clip_rect.h*texture->clip_rect.w*texture->clip_rect.w+texture->clip_rect.h*texture->clip_rect.h);
       pos=position;
       mas=U_mass;
}
SPHERE::SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,long double U_mass=1):PARTICLE(PhySimObj,texture,U_mass)
{
	P=PhySimObj;
	tex=texture;
	if(tex==NULL)
	   debugger.found("SPHERE()","loadimage() failed");
	dim.x=tex->w;
	dim.y=tex->h;
	dim.z=(dim.x+dim.y)/2.0;
	general_SPHERE_construction();
	vect from(0,0,0);
	vect to(P.scrdim.x,P.scrdim.y,P.scrdim.z);
	pos=random(from,to);
	mas=U_mass;
}
SPHERE::SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,vect position,long double U_mass=1):PARTICLE(PhySimObj,texture,U_mass)
{
       P=PhySimObj;
       tex=texture;
       if(tex==NULL)
               debugger.found("SPHERE()","loadimage() failed");
       dim.x=tex->w;
       dim.y=tex->h;
       dim.z=(dim.x+dim.y)/2.0;
       general_SPHERE_construction();
       pos=position;
       mas=U_mass;
}
double SPHERE::zoomfactor()
{
	vect relPos=(pos-P.cameraPosition());
	double RealRatio;
	if((relPos.z)==0)
		RealRatio=0.99;
	else
		RealRatio=(dim.y/relPos.z)/(tan(M_PI/4));
	if(RealRatio>0.9)
		RealRatio=0.9;
	zoom=(RealRatio*P.scrdim.y)/tex->clip_rect.h;
	return zoom;
}
vect PARTICLE::apparentPos()
{
	vect relPos=(pos-P.cameraPosition());
	appPos.y=(1+relPos.y/(relPos.z*tan(P.AngleOfView())))*P.scrdim.y/2;
	appPos.x=(1+relPos.x/(relPos.z*tan(P.AngleOfView())))*P.scrdim.x/2;
	return appPos;
}
int PARTICLE::globalcollision(double deltatime)
{
	if(pos.y+(vel.y+acc.y*deltatime)*deltatime>P.scrpos.y+P.scrdim.y)
	{
		if(continuous_contact()>20)
			pos.y=(scrpos.y+P.scrdim.y)/2.0;
		if(vel.y>0)
		{
			addvel(-vel*2);
		}
		return just_collided(true);
	}
	else if(pos.x+(vel.x+acc.x*deltatime)*deltatime>P.scrpos.x+P.scrdim.x)
	{
		if(continuous_contact()>2)
			pos.x=(scrpos.x+P.scrdim.x)/2.0;
		if(vel.x>0)
		{
			addvel(-vel*2);
		}
		return just_collided(true);
	}
	else if(pos.y+(vel.y+acc.y*deltatime)*deltatime<P.scrpos.y)
	{
		if(continuous_contact()>2)
			pos.y=(scrpos.y)/2.0;
		if(vel.y<0)
		{
			addvel(-vel*2);
		}
		return just_collided(true);
	}
	else if(pos.x+(vel.x+acc.x*deltatime)*deltatime<P.scrpos.x)
	{
		if(continuous_contact()>2)
			pos.x=(scrpos.x)/2.0;
		if(vel.x<0)
		{
			addvel(-vel*2);
		}
		return just_collided(true);
	}
	else if(pos.z+(vel.z+acc.z*deltatime)*deltatime>P.scrpos.z+P.scrdim.z)
	{
		if(continuous_contact()>2)
			pos.z=(scrpos.z+P.scrdim.z)/2.0;
		if(vel.z>0)
		{
			vel.z=-vel.z;
		}
		return just_collided(true);
	}
	else if(pos.z+(vel.z+acc.z*deltatime)*deltatime<P.scrpos.z)
	{
		if(continuous_contact()>2)
			pos.z=(scrpos.z)/2.0;
		if(vel.z<0)
		{
			vel.z=-vel.z;
		}
		return just_collided(true);
	}
	else return just_collided(false);
}
int SPHERE::globalcollision(double deltatime)
{
	if(pos.y+center.y+(vel.y+acc.y*deltatime)*deltatime>P.scrpos.y+P.scrdim.y)
	{
		if(continuous_contact()>20)
			pos.y=(scrpos.y+P.scrdim.y-dim.y)/2.0;
		if(vel.y>0)
		{
			addvel(-vel*2,vect(0,dim.y/2,0));
		}
		return just_collided(true);
	}
	else if(pos.x+center.x+(vel.x+acc.x*deltatime)*deltatime>P.scrpos.x+P.scrdim.x)
	{
		if(continuous_contact()>2)
			pos.x=(scrpos.x+P.scrdim.x)/2.0;
		if(vel.x>0)
		{
			addvel(-vel*2,vect(dim.x/2,0,0));
		}
		return just_collided(true);
	}
	else if(pos.y-center.y+(vel.y+acc.y*deltatime)*deltatime<P.scrpos.y)
	{
		if(continuous_contact()>2)
			pos.y=(scrpos.y)/2.0;
		if(vel.y<0)
		{
			addvel(-vel*2,vect(0,-dim.y/2,0));
		}
		return just_collided(true);
	}
	else if(pos.x-center.x+(vel.x+acc.x*deltatime)*deltatime<P.scrpos.x)
	{
		if(continuous_contact()>2)
			pos.x=(scrpos.x)/2.0;
		if(vel.x<0)
		{
			addvel(-vel*2,vect(-dim.x/2,0,0));
		}
		return just_collided(true);
	}
	else if(pos.z+center.z+(vel.z+acc.z*deltatime)*deltatime>P.scrpos.z+P.scrdim.z)
	{
		if(continuous_contact()>2)
			pos.z=(scrpos.z+P.scrdim.z-dim.z)/2.0;
		if(vel.z>0)
		{
			vel.z=-vel.z;
		}
		return just_collided(true);
	}
	else if(pos.z-center.z+(vel.z+acc.z*deltatime)*deltatime<P.scrpos.z)
	{
		if(continuous_contact()>2)
			pos.z=(scrpos.z)/2.0;
		if(vel.z<0)
		{
			vel.z=-vel.z;
		}
		return just_collided(true);
	}
	else return just_collided(false);
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
		return just_collided(true);
	}
	return just_collided(false);
}
bool SPHERE::mash(SPHERE &b)
{
	if(touched(b))
	{
		pos*=mas;
		pos+=b.position()*b.mass();
		mas+=b.mass();
		PARTICLE::addmomentum(b.momentum());
		addvolume(b.volume());
		if(P.findSphere(&b)!=-1)
			P.delsphere(&b);
		return true;
	}
	return false;
}
void PARTICLE::attach(PARTICLE* &b)
{
	if(b->independent)
	{
			b->set_relative_position(pos);
			member.push_back(b);
			mas+=b->mass();
			if(P.findParticle(b))
				P.delParticle(b);
	}
}
void SPHERE::attach(SPHERE* &b)
{
	if(b->independent)
	{
			b->set_relative_position(pos);
			member.push_back(b);
			mas+=b->mass();
			addmomentum(b->momentum(),b->relpos_with_respect_to_parent());
			if(P.findSphere(b))
				P.delsphere(b);
	}
}
void SPHERE::display()
{
	vect apparentPosition=apparentPos();
	if(P.OnScreen(apparentPosition,dim)&&P.InFrontOfCamera(position()))
		P.applysurface(tex,apparentPosition,ang,zoomfactor());
	for(unsigned int i=0;i<number_of_springs_connected();i++)
	{
		P.vect_line(pos,spring_connection[i].partner->position(),spring_connection[i].spring.color);
	}
}

int CELL::collision(CELL &b)
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
		return just_collided(true);
	}
	return just_collided(false);
}
bool CELL::mash(CELL &b)
{
	if(touched(b))
	{
		if(mas>=b.mass())
		{
			pos*=mas;
			pos+=b.position()*b.mass();
			mas+=b.mass();
			pos/=mas;
			PARTICLE::addmomentum(b.momentum());
			addvolume(b.volume());
			food_reserve+=b.food_reserve;
			E+=b.energy();
			if(P.findCell(&b)!=-1)
				P.delCell(&b);
			return true;
		}
	}
	return false;
}
CELL* CELL::find_food()
{
	if(P.cells.size()>1)
	{
			double max_energy_reward=-1000000;
			CELL* optimum=NULL;
			vector<CELL*> masses;
			for(unsigned int i=0;i<P.cells.size();i++)
			{
				if(P.cells[i]!=this)
				{
					if(P.cells[i]->mass()<=mass())
					{
						double energy_reward=P.cells[i]->mass();
						if(energy_reward>max_energy_reward)
						{
							max_energy_reward=energy_reward;
							optimum=P.cells[i];
						}
					}
				}
			}
			return optimum;
	}
	return NULL;
}
CELL* CELL::spot_predators()
{
	if(P.cells.size()>1)
	{
			CELL* predator=NULL;
			double biggest_predator=-10000;
			vector<CELL*> masses;
			for(unsigned int i=0;i<P.cells.size();i++)
			{
				if(P.cells[i]!=this)
				{
					if(P.cells[i]->mass()>mass())
					{
						double mass=P.cells[i]->mass();
						if(mass>biggest_predator)
						{
							biggest_predator=mass;
							predator=P.cells[i];
						}
					}
				}
			}
			return predator;
	}
	return NULL;
}
