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

#include <headers/debug.hpp>
#include <headers/global_assets.hpp>
#include <headers/framer.hpp>
#include <global_assets/global_assets.h>
#include <global_assets/vect.hpp>
#include <global_assets/random.hpp>
using namespace std;

const long double G=6.674*pow(10.0,-11);

//header for the PHYSIM class
class PHYSIM;

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
 * Similar to the Drawline() exept it accepts vect<> variables as coordinates instead of (float,float)
 */
void Draw_vect_Line(PHYSIM &PhySimObj,vect<> a,vect<> b,SDL_Color color);

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
	vect<> pos,appPos,vel,acc,f;
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
	vect<> relpos_parent;	//relative position of this object with respect to the object it is a part of (if it is part of a larger object)
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
	vect<> apparentPos();
	vect<> realPos(long double l);
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
	vect<> position()	//access function to pos
	{
		return pos;
	}
	vect<> relpos_with_respect_to_parent()	//access function to relpos_parent
	{
		return relpos_parent;
	}
	vect<> velocity()	//access function to vel
	{
		return vel;
	}
	long double mass()	//access function to mas
	{
		return mas;
	}
	vect<> momentum()	//function that returns the momentum of this object
	{
		return vel*mas;
	}
	vect<> addvel(vect<> b)	//function that adds the velocity passed to its own
	{
		vel+=b;
		return vel;
	}
	vect<> addmomentum(vect<> momentum)	//function that adds the velocity passed to its own
	{
		return vel+=momentum/mas;
	}
	vect<> addacc(vect<> b)	//function that adds the acceleration passed to its own
	{
		acc+=b;
		return acc;
	}
	vect<> addforce(vect<> b)	//function that adds the force passed to its own
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
		vect<> u=vel;
		vel+=acc*deltatime;	//v=u+at
		pos+=u*deltatime;		//s=s0+ut
		pos+=acc*0.5*deltatime*deltatime;	//s=s0+a*t^2
	}
	void gravity(PARTICLE* &b)	//function that calculates and adds the gravitational force between this object and the one passed
	{
		vect<> relpos=(b->position()-pos);
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
	vect<> spring(PARTICLE &b)	//function that first checks if the passed spring is connected to this object and then calculated and adds the forces between them
	{
		for(unsigned int i=0;i<spring_connection.size();i++)
		{
			if(spring_connection[i].partner==&b)
			{
				vect<> relpos=(b.position()-pos);
					//force=	dir		*		extension							*	spring constant
				return f+=(relpos.dir()*(relpos.mag()-spring_connection[i].spring.l)*spring_connection[i].spring.k);
			}
		}
		return (vect<>){0,0,0};
	}
	virtual int globalcollision(double deltatime);	//virtual function that first checks if the object has collided with the walls of the scene and then causes it to bounce of that wall it collided with(if any)
	virtual void attach(PARTICLE* &b);	//virtual function that attaches the passed PARTICLE to this one
	vect<> set_relative_position(vect<> parent_position)	//makes this object a daughter object of the passed particle
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
		f=(vect<>){0,0,0};
		acc=(vect<>){0,0,0};
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
	vect<> dim,center,ang,tta,tor;
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
	SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,vect<> position,vect<> dimension,long double U_mass);
	SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,long double U_mass);
	SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,vect<> U_pos,long double U_mass);
	long double diameter()	//returns the diameter of the SPHERE
	{
		return dim.mag();
	}
	virtual long double volume()	//calculates and returns the volume of the SPHERE
	{
		return 4*M_PI*pow((diameter()/2.0),3)/3.0;
	}
	vect<> addtta(vect<> u_tta)	//adds a little more angular rotation (spin) to the SPHERE
	{
		return tta+=u_tta;
	}
	/**
	 * 2 parameters:
	 * u_vel(the amount of velocity to add to the ball)
	 * u_pos(the position on the ball onto which this velocity is added determines how much the ball will end up spinning)
	 */
	vect<> addvel(vect<> u_vel,vect<> u_pos)
	{
		tta+=((u_vel^u_pos)/pow(u_pos.mag(),2));
		return vel+=u_pos.dir()*((u_vel|u_pos)/u_pos.mag());
	}
	vect<> addmomentum(vect<> momentum,vect<> u_pos)	//similar to addvel() exept momentum is being added!
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
		vect<> relpos=(b->position()-pos);
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
	vect<> dimensions()
	{
		long double radius=pow(total_volume()/(4/3*M_PI),1/3.0);
		return (vect<>){radius,radius,radius};
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
	vect<> calculated_dimensions()	//calculates and returns the dimensions of the CELL
	{
		long double radius=pow(volume()/(4/3*M_PI),1/3.0);
		return (vect<>){radius,radius,radius};
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
	CELL(PHYSIM &PhySimObj,SDL_Surface* texture,vect<> position,DNA seed_DNA)	:	SPHERE(PhySimObj,texture,position,seed_DNA.dimensions(),seed_DNA.total_mass()),dna(seed_DNA)
	{
		inherit_DNA();
	}
	CELL(PHYSIM &PhySimObj,SDL_Surface* texture,long double Visibility,long double U_mass)	:	SPHERE(PhySimObj,texture,U_mass)
	{
		visib=Visibility;
		inherit_DNA();
	}
	CELL(PHYSIM &PhySimObj,SDL_Surface* texture,vect<> U_pos,long double Visibility,long double U_mass)	:	SPHERE(PhySimObj,texture,U_pos,U_mass)
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
class PHYSIM:public SDL_3D
{
	unsigned int tag_provider;	//variable that increments every time an object is created
	SPHERE* handler;	//a pointer to a SPHERE used to temporarily store addresses in various processes
	void frametermination()	//performs actions that are to be done at the end of a frame (like delaying if all frame processes finished early)
	{
		if(debug_mode)
			framelog<<frametimer.currentfps()<<"	"<<frametimer.deltatime()<<"\n";
		SDL_Delay(frametimer.remainingfreetime()*1000.0);
		frametimer.endframe();
	}
	void general_construction()	//general actions that are done common to all PHYSIM constructors
	{
		camera_speed=10;
		frametimer.updatefpslimits(10,30);
		runtime.start();
		error=new DEBUG(file_loc(buf,log_loc.c_str(),"psm"));
		handler=NULL;
		aov=M_PI/4.0;
		if(SDL_Init(SDL_INIT_EVERYTHING)==-1)
			error->found((char*)"PHYSIM()",(char*)"SDL_Init() failed");
		if( TTF_Init()==-1 )
			error->found((char*)"PHYSIM()",(char*)"TTF_Init() failed");
		if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
			error->found((char*)"PHYSIM()",(char*)"Mix_OpenAudio() failed");
		srand(SDL_GetTicks());
		load_HUD();
		if(debug_mode)
			framelog.open("logs/framelog.txt");
		load_HUD();
	}
	SPHERE* general_gensphere(SPHERE* handler)	//common actions that are to be done while generating any SPHERE
	{
		if(handler)
		{
			spheres.push_back(handler);
		}
		return handler;
	}
	bool debug_mode;
public:
	/**
	 * (world_origin)	represents the origin of the physical world (and is generally (0,0,0))
	 * (world_dim)	represents the dimensions of the physical world (if active, objects will collide of these walls)
	 * (mousepos) stores the current position coordinates of the mouse pointer
	 * (camera_pos) stores the current position of the camera
	 * (cameraVel) stores the current velocity with which the camera is to move
	 */
	vect<> world_origin,mousepos,cameraVel;
	long double camera_speed;	//stores how much speed with which the camera is to move in a particular direction
	vector<void*> objects;	//vector of objects that could potentially point to any kind of object
	vector<PARTICLE*> particles;	//vector of PARTICLEs that point to all particles currently on screen
	vector<SPHERE*> spheres;	//vector of SPHEREs that point to all SPHEREs currently on screen
	vector<CELL*> cells;	//vector of CELLs that point to all CELLs currently on screen
	int bpp;	//stores amount of bits that are to be stored per pixel
	DEBUG* error;	//pointer variable error of the type DEBUG
	timer runtime;	//used to measure for how long the program has been running
	framer frametimer;	//used to control the framerate at which the program runs
	ofstream framelog;

	GRAPHIC_STRING title;	//name of the scene
	/**
	 * constructor that creates a physical world that:
	 * has 'user_dim' dimensions,
	 * has user_pos as origin,
	 * has user_bpp as the bits per pixel of its screen
	 */
	void load_HUD()
	{
		title.set_color(250);
		title="aevo";
		title.set_font(font_pocket.new_font((font_loc+"KeraterMedium.ttf").c_str(),28));
		title.set_update_interval(10000);
		title.set_position(1,1);							//set its position
	}
	PHYSIM(vect<> screen_dimensions,vect<> user_pos=(vect<>){0,0,0})
	:
		SDL_3D(SDL_SetVideoMode(screen_dimensions.x,screen_dimensions.y,screen_dimensions.z,SDL_SWSURFACE|SDL_RESIZABLE)),
		title(SDL_2D_obj)
	{
		debug_mode=false;
		general_construction();
		change_world_dimensions(screen_dimensions);
		camera_pos=screen_dimensions/2;
		camera_pos.z=-200;
		world_origin=user_pos;
		scr=SDL_SetVideoMode(screen_dimensions.x,screen_dimensions.y,screen_dimensions.z,SDL_SWSURFACE|SDL_RESIZABLE);
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
	//returns how many objects are currently on screen
	int object_count()
	{
		return (int)(particles.size()+spheres.size()+cells.size());
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
	SPHERE* gensphere(SDL_Surface* texture,vect<> position,vect<> dimension,long double U_mass=1)
	{
		handler=new SPHERE(*this,texture,position,dimension,U_mass);
		if(handler)
			return general_gensphere(handler);
		else
			return NULL;
	}
	SPHERE* gensphere(SDL_Surface* texture,vect<> position,long double U_mass=1)
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
				SPHERE *a=spheres[j],*b=spheres[j+1];
				if((a->position().z-camera_pos.z)<(b->position().z-camera_pos.z))
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
				CELL *a=cells[j],*b=cells[j+1];
				if((a->position().z-camera_pos.z)<(b->position().z-camera_pos.z))
				{
					CELL* temp=cells[j];
					cells[j]=cells[j+1];
					cells[j+1]=temp;
				}
			}
		}
	}
	bool OnScreen(vect<> pos,vect<> dim)	//checks of a particular coordinate and dimension is on the screen or off it
	{
		if(	pos.x+dim.x>=world_origin.x
			&&pos.y+dim.x>=world_origin.y
			&&pos.z+dim.x>=world_origin.z
			&&pos.x-dim.x/2<=world_origin.x+scr_dim.x
			&&pos.y-dim.y<=world_origin.y+scr_dim.y
			&&pos.z-dim.z<=world_origin.z+scr_dim.z)
			return true;
		else
			return false;
	}
	bool InFrontOfCamera(vect<> pos)	//checks if a particular 3D coordinate is in front of the camera
	{
		if((pos-camera_pos).z>0)
			return true;
		else
			return false;
	}
	bool mousemotion()	//handles events related to mouse motion
	{
		if(event.type == SDL_MOUSEMOTION )
		{

			mousepos.x = event.motion.x;
			mousepos.y = event.motion.y;
			mousepos.z = 0;
			return 1;
		}
		return 0;
	}
	bool HandleCameraMovement()	//handles events related to camera motion
	{
		vect<> oldcameraPos=camera_pos;
		switch(event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			switch(event.button.button)
			{
			case SDL_BUTTON_WHEELUP:
				camera_pos.z+=camera_speed;
			break;
			case SDL_BUTTON_WHEELDOWN:
				camera_pos.z-=camera_speed;
			break;
			}
		break;
		}
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch((unsigned int)event.key.keysym.sym)
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
			switch((unsigned int)event.key.keysym.sym)
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
		return oldcameraPos==camera_pos;
	}
	void MoveCamera()	//changes the camera position based on it current velocity
	{
		camera_pos+=cameraVel;
	}
	vect<> random_position()	//returns a random position in the physical worls
	{
        vect<> from(0,0,0);
        vect<> to=world_dim;
        return random(from,to);
	}
	template<class T>
	void deallocate_objects(vector<T*> A)
	{
		string file_loc=(log_loc+"allocation log.txt");
		ofstream allo;
		if(!A.empty())
		{
			for(int i=A.size()-1;i>=0;i--)
			{
				T* handler=A[i];
				allo.open(file_loc.c_str(),ios::app);
				allo<<i+1<<".	deleting	"<<handler<<"\n";
				allo.close();
				delete handler;
			}
		}
	}
	~PHYSIM()	//deallocates all allocated memory and generates status log files,etc
	{
		string file_loc=(log_loc+"allocation log.txt");
		ofstream allo(file_loc.c_str());
		allo<<spheres.size()<<" spheres to delete:"<<"\n";
		allo.close();
		allo.open(file_loc.c_str(),ios::app);
		allo<<cells.size()<<" cells to delete:"<<"\n";
		allo.close();
		deallocate_objects(cells);
		deallocate_objects(spheres);
		SDL_FreeSurface(scr);
		Mix_CloseAudio();
		TTF_Quit();
		SDL_FreeSurface(scr);
		SDL_Quit();
		delete error;
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
	vect<> from(0,0,0);
	vect<> to(P.world_dim.x,P.world_dim.y,P.world_dim.z);
	pos=random(from,to);
	mas=U_mass;
}
SPHERE::SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,vect<> position,vect<> dimension,long double U_mass=1):PARTICLE(PhySimObj,texture,U_mass)
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
	tex=texture;
	if(tex==NULL)
	   debugger.found("SPHERE()","loadimage() failed");
	dim.x=tex->w;
	dim.y=tex->h;
	dim.z=(dim.x+dim.y)/2.0;
	general_SPHERE_construction();
	vect<> from(0,0,0);
	vect<> to(P.world_dim.x,P.world_dim.y,P.world_dim.z);
	pos=random(from,to);
	mas=U_mass;
}
SPHERE::SPHERE(PHYSIM &PhySimObj,SDL_Surface* texture,vect<> position,long double U_mass=1):PARTICLE(PhySimObj,texture,U_mass)
{
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
	vect<> relPos=(pos-P.cameraPosition());
	double RealRatio;
	if((relPos.z)==0)
		RealRatio=0.99;
	else
		RealRatio=(dim.y/relPos.z)/(tan(M_PI/4));
	if(RealRatio>0.9)
		RealRatio=0.9;
	zoom=(RealRatio*P.scr_dim.y)/tex->clip_rect.h;
	return zoom;
}
vect<> PARTICLE::apparentPos()
{
	return appPos=P.apparent_pos_of(pos);
}
int PARTICLE::globalcollision(double deltatime)
{
	if(pos.y+(vel.y+acc.y*deltatime)*deltatime>P.world_origin.y+P.scr_dim.y)
	{
		if(continuous_contact()>20)
			pos.y=(P.world_origin.y+P.world_dim.y)/2.0;
		if(vel.y>0)
		{
			addvel(-vel*2);
		}
		return just_collided(true);
	}
	else if(pos.x+(vel.x+acc.x*deltatime)*deltatime>P.world_origin.x+P.world_dim.x)
	{
		if(continuous_contact()>2)
			pos.x=(P.world_origin.x+P.world_dim.x)/2.0;
		if(vel.x>0)
		{
			addvel(-vel*2);
		}
		return just_collided(true);
	}
	else if(pos.y+(vel.y+acc.y*deltatime)*deltatime<P.world_origin.y)
	{
		if(continuous_contact()>2)
			pos.y=(P.world_origin.y)/2.0;
		if(vel.y<0)
		{
			addvel(-vel*2);
		}
		return just_collided(true);
	}
	else if(pos.x+(vel.x+acc.x*deltatime)*deltatime<P.world_origin.x)
	{
		if(continuous_contact()>2)
			pos.x=(P.world_origin.x)/2.0;
		if(vel.x<0)
		{
			addvel(-vel*2);
		}
		return just_collided(true);
	}
	else if(pos.z+(vel.z+acc.z*deltatime)*deltatime>P.world_origin.z+P.world_dim.z)
	{
		if(continuous_contact()>2)
			pos.z=(P.world_origin.z+P.world_dim.z)/2.0;
		if(vel.z>0)
		{
			vel.z=-vel.z;
		}
		return just_collided(true);
	}
	else if(pos.z+(vel.z+acc.z*deltatime)*deltatime<P.world_origin.z)
	{
		if(continuous_contact()>2)
			pos.z=(P.world_origin.z)/2.0;
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
	if(pos.y+center.y+(vel.y+acc.y*deltatime)*deltatime>P.world_origin.y+P.world_dim.y)
	{
		if(continuous_contact()>20)
			pos.y=(P.world_origin.y+P.world_dim.y-dim.y)/2.0;
		if(vel.y>0)
		{
			addvel(-vel*2,vect<>(0,dim.y/2,0));
		}
		return just_collided(true);
	}
	else if(pos.x+center.x+(vel.x+acc.x*deltatime)*deltatime>P.world_origin.x+P.world_dim.x)
	{
		if(continuous_contact()>2)
			pos.x=(P.world_origin.x+P.world_dim.x)/2.0;
		if(vel.x>0)
		{
			addvel(-vel*2,vect<>(dim.x/2,0,0));
		}
		return just_collided(true);
	}
	else if(pos.y-center.y+(vel.y+acc.y*deltatime)*deltatime<P.world_origin.y)
	{
		if(continuous_contact()>2)
			pos.y=(P.world_origin.y)/2.0;
		if(vel.y<0)
		{
			addvel(-vel*2,vect<>(0,-dim.y/2,0));
		}
		return just_collided(true);
	}
	else if(pos.x-center.x+(vel.x+acc.x*deltatime)*deltatime<P.world_origin.x)
	{
		if(continuous_contact()>2)
			pos.x=(P.world_origin.x)/2.0;
		if(vel.x<0)
		{
			addvel(-vel*2,vect<>(-dim.x/2,0,0));
		}
		return just_collided(true);
	}
	else if(pos.z+center.z+(vel.z+acc.z*deltatime)*deltatime>P.world_origin.z+P.world_dim.z)
	{
		if(continuous_contact()>2)
			pos.z=(P.world_origin.z+P.world_dim.z-dim.z)/2.0;
		if(vel.z>0)
		{
			vel.z=-vel.z;
		}
		return just_collided(true);
	}
	else if(pos.z-center.z+(vel.z+acc.z*deltatime)*deltatime<P.world_origin.z)
	{
		if(continuous_contact()>2)
			pos.z=(P.world_origin.z)/2.0;
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
		vect<> avel=vel;
		vect<> bvel=b.velocity();
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
	vect<> apparentPosition=apparentPos();
	if(P.OnScreen(apparentPosition,dim)&&P.InFrontOfCamera(position()))
		P.applysurface(tex,apparentPosition,ang,zoomfactor());
	for(unsigned int i=0;i<number_of_springs_connected();i++)
	{
		P.draw_vect_line(pos,spring_connection[i].partner->position(),spring_connection[i].spring.color);
	}
}

int CELL::collision(CELL &b)
{
	if(touched(b))
	{
		vect<> avel=vel;
		vect<> bvel=b.velocity();
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
					CELL* a=P.cells[i];
					if(a->mass()<=mass())
					{
						double energy_reward=a->mass();
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
		double biggest_predator_mass=-10000;
		for(unsigned int i=0;i<P.cells.size();i++)
		{
			if(P.cells[i]!=this)
			{
				CELL* a=P.cells[i];
				if(a->mass()>mass())
				{
					if(a->mass()>biggest_predator_mass)
					{
						biggest_predator_mass=a->mass();
						predator=P.cells[i];
					}
				}
			}
		}
		return predator;
	}
	return NULL;
}
