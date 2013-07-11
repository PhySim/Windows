/*
 * physim.hpp
 *
 *  Created on: Jun 30, 2013
 *      Author: Reuben
 */
#include "SDL/SDL.h"
#include "SDL/SDL_Image.h"
#include "SDL/SDL_ttf.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <ctime>

using namespace std;

SDL_Rect scrdim;
SDL_Surface* scr;
unsigned long int deltatime=100;
short bpp;

class vect
{
public:
	long double x,y,z;
	vect reverse()
	{
		x=-x;
		y=-y;
		z=-z;
		return (vect){x,y,z};
	}
	vect add(vect b)
	{
		vect temp;
		temp.x=x+=b.x;
		temp.y=y+=b.y;
		temp.x=z+=b.z;
		return temp;
	}
	vect subtract(vect b)
	{
		x-=b.x;
		y-=b.y;
		z-=b.z;
		return (vect){x,y,z};
	}
	vect multiply(long double scaler)
	{
		vect temp;
		temp.x=x*=scaler;
		temp.y=y*=scaler;
		temp.z=z*=scaler;
		return temp;
	}
	long double multiply_dot(vect b)
	{
		return x*b.x+y*b.y+z*b.z;
	}
	vect multiply_cross(vect b)
	{
		x=y*b.z-z*b.y;
		y=-(x*b.z-b.x*z);
		z=x*b.y-b.x*y;
		return (vect){x,y,z};
	}
};

vect reverse(vect a)
{
	vect temp;
	temp.x=a.x=-a.x;
	temp.y=a.y=-a.y;
	temp.z=a.z=-a.z;
	return temp;
}
vect add(vect a,vect b)
{
	vect temp;
	temp.x=a.x+=b.x;
	temp.y=a.y+=b.y;
	temp.x=a.z+=b.z;
	return temp;
}
vect subtract(vect a,vect b)
{
	vect temp;
	temp.x=a.x-=b.x;
	temp.y=a.y-=b.y;
	temp.z=a.z-=b.z;
	return temp;
}
vect multiply(vect a,long double scaler)
{
	vect temp;
	temp.x=a.x*=scaler;
	temp.y=a.y*=scaler;
	temp.z=a.z*=scaler;
	return temp;
}
long double multiply_dot(vect a,vect b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}
vect multiply_cross(vect a,vect b)
{
	vect temp;
	temp.x=a.x=a.y*b.z-a.z*b.y;
	temp.y=a.y=-(a.x*b.z-b.x*a.z);
	temp.z=a.z=a.x*b.y-b.x*a.y;
	return temp;
}

int random(int a,int b)
{
	int d=b-a;
	return a+rand()%d;
}
long double random(long double a,long double b)
{
	long double d=b-a;
	if(d<0)
		d=-d;

	if(log(d)>7)
		return a+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
//(else
		return a+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));
}
vect random(vect a,vect b)
{
	vect c;

	long double d;
	d=b.x-a.x;
			if(d<0)
				d=-d;

			if(log(d)>7)
				c.x=a.x+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
			else
				c.x=a.x+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));
	d=b.y-a.y;
			if(d<0)
				d=-d;

			if(log(d)>7)
				c.y=a.y+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
			else
				c.y=a.y+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));
	d=b.z-a.z;
			if(d<0)
				d=-d;

			if(log(d)>7)
				c.z=a.z+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
			else
				c.z=a.z+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));

	return c;
}

char* currentdatetime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char	buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return buf;
}

class DEBUG
{
	int num;
	int max;
	char debug_name[128];
	char description[1000][256];
	char source[1000][128];
	char time[1000][48];
	ofstream log;
public:
	DEBUG(char user_name[128]=NULL)
	{
		if(user_name==NULL)
		{
			strcpy(debug_name,"DEBUG");
		}
		else
			strcpy(debug_name,user_name);
		strcat(debug_name,".txt");
		log.open(debug_name,ios::app);
		log<<"\n"<<currentdatetime()<<"	---------New Execution---------	("<<debug_name<<")\n";
		num=0;
		max=1000;
		for(int i=0;i<max;i++)
		{
			strcpy(description[i],"<empty>");
			strcpy(source[i],"<empty>");
			strcpy(time[i],"<empty>");
		}
	}
	void rename_debugger(char* user_name,char* mode="default")
	{
		strcat(user_name,".txt");
		log.close();
		if(rename(debug_name,user_name)!=-1)
		{
			log.open(user_name,ios::app);
			log<<"\n"<<currentdatetime()<<"	log renamed from "<<debug_name<<" to "<<user_name<<'\n';
			strcpy(debug_name,user_name);
		}
		else if(strcmp(mode,"overwrite")==0)
		{
			remove(user_name);
			if(rename(debug_name,user_name)!=-1)
			{
				log.open(user_name,ios::app);
				log<<"\n"<<currentdatetime()<<"	log renamed from "<<debug_name<<" to "<<user_name<<'\n';
				strcpy(debug_name,user_name);
			}
			else
			{
				log.open(debug_name,ios::app);
				found("DEBUG.rename_debugger()","error renaming log (make sure the file doesn't already exist!)");
			}
		}
		else if(strcmp(mode,"switch")==0)
		{
			remove("deleteme.tmp");
			rename(user_name,"deleteme.tmp");
			rename(debug_name,user_name);
			rename("deleteme.tmp",debug_name);
			log.open(user_name,ios::app);
			log<<"\n"<<currentdatetime()<<"	log renamed from "<<debug_name<<" to "<<user_name<<'\n';
			strcpy(debug_name,user_name);
		}
		else
		{
			log.open(debug_name,ios::app);
			found("DEBUG.rename_debugger()","error renaming log (make sure the file doesn't already exist!)");
		}
	}
	int found(const char user_source[100],const char user_description[256])
	{
		for(int i=0;num<max;i++)
		{
			if(strcmp(description[i],"<empty>")==0)
			{
				strcpy(description[i],user_description);
				strcpy(source[i],user_source);
				strcpy(time[i],currentdatetime());
				num++;
			}
			return num;
		}
		return num;
	}
	int resolved(const char user_source[100],const char user_description[256])
	{
		for(int i=0;i<max;i++)
		{
			if(strcmp(user_description,description[i])==0&&strcmp(user_source,source[i])==0)
			{
				strcpy(source[i],"<empty>");
				strcpy(description[i],"<empty>");
				strcpy(time[i],"<empty>");
				num--;
				return num;
			}
		}
		return num;
	}
	~DEBUG()
	{
		int sno=0;
		for(int i=0;i<max;i++)
		{
			if(strcmp(description[i],"<empty>"))
			{
				log<<'\n'<<++sno<<".\t"<<time[i]<<"\nSource:\t\t"<<source[i]<<"\nDescription:\t"<<description[i]<<'\n';
			}
		}
		log<<"\n"<<currentdatetime()<<"	__________Termination__________\n";
		log.close();
	}
}debugger((char*)"physim.h");

class timer
{
	unsigned long int start_time,cummulative_ticks;
	bool current_state;//0 means stopped, 1 means running
public:
	timer()
	{
		reset();
	}
	void reset()
	{
		cummulative_ticks=0;
		current_state=0;
	}
	void start()
	{
		if(current_state==0)//if it is not running already
		{
			start_time=SDL_GetTicks();
			current_state=1;
		}
	}
	void stop()
	{
		cummulative_ticks+=SDL_GetTicks()-start_time;
		current_state=0;
	}
	unsigned long int elapse()
	{
		if(current_state==0)
			return cummulative_ticks;
		else
			return cummulative_ticks + (SDL_GetTicks()-start_time);
	}
};

class framer : protected timer
{
	unsigned long int count;
	double minfps,maxfps,current_fps;
	double minfreq,maxfreq,currentfreq;
public:
	framer(double user_minfps=10,double user_maxfps=32)
	{
		minfps=user_minfps;
		maxfps=user_maxfps;
		current_fps=(minfps+maxfps)/2;

		minfreq=1000/maxfps;
		maxfreq=1000/minfps;
		currentfreq=1000/current_fps;
		count=0;
	}
	void updatefpslimits(double user_minfps=10,double user_maxfps=30)
	{
		minfps=user_minfps;
		maxfps=user_maxfps;
		current_fps=(minfps+maxfps)/2;

		minfreq=1000/maxfps;
		maxfreq=1000/minfps;
		currentfreq=1000/current_fps;
	}
	void newframe()
	{
	}
	void endframe()
	{
		currentfreq=elapse();
		current_fps=1000/currentfreq;
		reset();
		start();
		count++;
	}
	unsigned long int currentframe()
	{
		return count;
	}
	double currentfrequency()
	{
		return currentfreq;
	}
	double currentfps()
	{
		return current_fps;
	}
	double remainingfreetime()
	{
		if(elapse()<minfreq)
			return minfreq-elapse();
		else return 0;
	}
	unsigned long int elapsed()
	{
		return elapse();
	}
	double deltatime()
	{
		if(currentfreq>maxfreq)
			return maxfreq;
		else if(currentfreq<minfreq)
			return minfreq;
		else
			return currentfreq;
	}
};

SDL_Surface* loadimage(string filename)
{
	SDL_Surface* mat=IMG_Load(filename.c_str());
	if(mat!=NULL)
	{
		mat=SDL_DisplayFormat(mat);
		if(mat!=NULL)
		{
			SDL_SetColorKey(mat,SDL_SRCCOLORKEY,SDL_MapRGB(mat->format,0,0xFF,0xFF));
			if(mat==NULL)
				debugger.found("loadimage()","SDL_SetColorKey() failed");
		}
		else
			debugger.found("loadimage()","SDL_DisplayFormat() failed");
	}
		else
			debugger.found("loadimage()","IMG_Load() failed");
	return mat;
}

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

class PHYSIM
{
	SDL_Surface* scr;
	SDL_Rect scrdim;
	void frametermination()
	{
		frametimer.endframe();
		SDL_Delay(frametimer.remainingfreetime());
	}
public:
	int bpp;
	DEBUG* error;
	timer runtime;
	framer frametimer;
	bool ended;

	PHYSIM(SDL_Rect user_dim,SDL_Surface* user_screen=NULL,int user_bpp=32)
	{
		runtime.start();
		error=new DEBUG((char*)"psm");
		if(SDL_Init(SDL_INIT_EVERYTHING)==-1)
			error->found((char*)"PHYSIM()",(char*)"SDL_Init() failed");
		srand(SDL_GetTicks());
		scr=user_screen;
		scrdim=user_dim;
		if(scr==NULL)
		{
			scrdim.x=0;
			scrdim.y=0;
			scrdim.w=720;
			scrdim.h=480;
			bpp=user_bpp;
		}
		::scrdim=scrdim;
		::scr=scr=SDL_SetVideoMode(scrdim.w,scrdim.h,bpp,SDL_SWSURFACE|SDL_RESIZABLE);
		::deltatime=frametimer.currentfrequency();
		ended=false;
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
		SDL_Quit();
		delete error;
		SDL_FreeSurface(scr);
	}
};


class particle
{
	SDL_Surface* mat;

public:
	vect pos,dim,vel,acc;
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

	int globalcollision(long double deltatime)
	{
		deltatime/=1000;
		if(pos.y+dim.y+(vel.y+acc.y*deltatime)*deltatime>scrdim.y+scrdim.h)
		{
			long double frac=(vel.y*deltatime+0.5*acc.y*deltatime*deltatime);
			for(int i=0;i<frac;i++)
			{
				vel.y+=acc.y*deltatime/frac;
				pos.y+=vel.y*deltatime/frac;
				if(pos.y+dim.y>scrdim.y+scrdim.h)
				{
					if(vel.y>0)
					{
						vel.y=-vel.y;
					}
				}
			}
			return 1;
		}
		else
			return 0;
		/*deltatime/=1000;
		if(pos.y+(vel.y+acc.y*deltatime)*deltatime>=scrdim.h-dim.y)
		{
			const double frac=1000;
			deltatime/=frac;
			for(int i=0;i<frac;i++)
			{
				vel.y+=acc.y*deltatime/frac;
				pos.y+=vel.y*deltatime/frac;
				if(pos.y>=scrdim.h-dim.y)
				{
					if(vel.y>0)
						vel.y=-vel.y;
				}
			}
			return 1;
		}
		else return 0;
		*/
	}
	void integrate(long unsigned int deltatime)
	{
		if(deltatime==0)
			debugger.found("integrate()","deltatimevalue=0");
		vect u=vel;
		vel.add(multiply(acc,deltatime/1000.0));
		pos.add(multiply(u,deltatime/1000.0));
		pos.add(multiply(acc,deltatime*deltatime/1000/1000));
	}

	particle(vect position,vect dimension, SDL_Surface* user_material)
	{
			pos=position;
			dim=dimension;
			mat=user_material;
			if(mat==NULL)
				debugger.found("particle()","loadimage() failed");
	}
	particle(SDL_Surface* user_material)
	{
		mat=user_material;
		if(mat==NULL)
			debugger.found("particle()","loadimage() failed");
		dim.x=mat->w;
		dim.y=mat->h;
		vect from={0,0,0};
		vect to={720-dim.x,480-dim.y,0};
		pos=random(from,to);
	}

	void display(SDL_Surface* screen)
	{
		applysurface(mat,pos);
	}

	~particle()
	{
		SDL_FreeSurface(mat);
	}
};
