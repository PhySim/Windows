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

class vector
{
public:
	long double x,y,z;
};
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
int random(int a,int b)
{
	int d=b-a;
	return a+rand()%d;
}

class DEBUG
{
	int num;
	char description[1000][256];
	char source[1000][100];
public:
	DEBUG()
	{
		num=0;
		for(int i=0;i<1000;i++)
			for(int j=0;j<250;j++)
				description[i][j]='\0';
	}
	int found(char user_source[100],char user_description[256])
	{
		for(int i=0;i<num&&num<1000;i++)
		{
			if(strcmp(description[i],"empty")==0)
			{
				strcpy(source[i],user_source);
				strcpy(description[i],user_description);
				num++;
			}
			return num;
		}
		return num;
	}
	int resolved(char user_source[100],char user_description[256])
	{
		for(int i=0;i<1000;i++)
		{
			if(strcmp(user_description,description[i])==0)
			{
				strcpy(source[i],"empty");
				strcpy(description[i],"empty");
				num--;
				return num;
			}
		}
		return num;
	}
};

class PHYSIM
{
public:
	SDL_Rect scrdim;
	int bpp;
	SDL_Surface* scr;
	DEBUG error;

	bool ended;

	PHYSIM(SDL_Surface* user_screen,SDL_Rect user_dim,int user_bpp=32)
	{
		if(SDL_Init(SDL_INIT_EVERYTHING)==-1)
			error.found((char*)"PHYSIM()",(char*)"SDL_Init() failed");
		scr=user_screen;
		scrdim=user_dim;
		if(scr==NULL)
		{
			scrdim.x=0;
			scrdim.y=0;
			scrdim.w=720;
			scrdim.h=480;
			bpp=user_bpp;
			user_screen=scr=SDL_SetVideoMode(scrdim.w,scrdim.h,bpp,SDL_SWSURFACE|SDL_RESIZABLE);
		}
		ended=false;
	}
	~PHYSIM()
	{
		SDL_Quit();
	}
};


class particle
{
	vector pos,dim;
	SDL_Surface mat;
	particle(vector position,vector dimension)
	{
			pos=position;
			dim=dimension;
	}
};
