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

short bpp;

class vector
{
public:
	long double x,y,z;
	vector add(vector b)
	{
		x+=b.x;
		y+=b.y;
		z+=b.z;
		return (vector){x,y,z};
	}
	vector subtract(vector b)
	{
		x-=b.x;
		y-=b.y;
		z-=b.z;
		return (vector){x,y,z};
	}
	vector multiply(long double scaler)
	{
		x*=scaler;
		y*=scaler;
		z*=scaler;
		return (vector){x,y,z};
	}
	long double multiply_dot(vector b)
	{
		return x*b.x+y*b.y+z*b.z;
	}
	vector multiply_cross(vector b)
	{
		x=y*b.z-z*b.y;
		y=-(x*b.z-b.x*z);
		z=x*b.y-b.x*y;
		return (vector){x,y,z};
	}
};

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
vector random(vector a,vector b)
{
	vector c;

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

class PHYSIM
{
public:
	SDL_Surface* scr;
	SDL_Rect scrdim;
	int bpp;
	DEBUG* error;

	bool ended;

	PHYSIM(SDL_Rect user_dim,SDL_Surface* user_screen=NULL,int user_bpp=32)
	{
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
			user_screen=scr=SDL_SetVideoMode(scrdim.w,scrdim.h,bpp,SDL_SWSURFACE|SDL_RESIZABLE);
		ended=false;
	}
	~PHYSIM()
	{
		SDL_Quit();
		delete error;
	}
};


class particle
{
	vector pos,dim,vel,acc;
	SDL_Surface* mat;
	SDL_Surface* loadimage(string filename)
	{
		mat=IMG_Load(filename.c_str());
		if(mat!=NULL)
		{
			mat=SDL_DisplayFormat(mat);
			if(mat!=NULL)
			{
				SDL_SetColorKey(mat,SDL_SRCCOLORKEY,SDL_MapRGB(mat->format,0,0xFF,0xFF));
				if(mat==NULL)
					debugger.found("particle()","SDL_SetColorKey() failed");
			}
			else
				debugger.found("particle()","SDL_DisplayFormat() failed");
		}
			else
				debugger.found("particle()","IMG_Load() failed");
		return mat;
	}
public:
	vector addacc(vector add)
	{
		acc.x+=add.x;
		acc.y+=add.y;
		acc.z+=add.z;
		return acc;
	}
	void colbot()
	{
		if(pos.y>=scrdim.h-dim.y)
		{
			vel.y=-vel.y;
			pos.y+=vel.y;
		}
	}
	void gravity()
	{
			vel.y+=acc.y;
			pos.y+=vel.y;

			colbot();
	}

	particle(vector position,vector dimension, string filename)
	{
			pos=position;
			dim=dimension;
			loadimage(filename);
	}
	particle(string filename)
	{
		loadimage(filename);
		dim.x=mat->w;
		dim.y=mat->h;
		vector from={0,0,0};
		vector to={720-dim.x,480-dim.y,0};
		pos=random(from,to);
	}
	void display(SDL_Surface* screen)
	{
		gravity();
		SDL_Rect offset;
		offset.x=pos.x;
		offset.y=pos.y;
		SDL_BlitSurface(mat,NULL,screen,&offset);
	}

	~particle()
	{
		SDL_FreeSurface(mat);
	}
};
