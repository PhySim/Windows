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

int random(const char* range)
{
	int i,a,b;
	i=a=b=0;
	while(range[i]!='-')
	{
		a*=10;
		a+=range[i]-48;
		i++;
	}
	if(range[i]=='-')
	{
		i++;
		while(range[i])
		{
			b*=10;
			b+=(range[i]-48);
			i++;
		}
	}
	return a+rand()%(b-a+1);
}
long double random(long double a,long double b)
{
	if(log(b-a)>8)
		return a+(long double)(rand()%(int)(((b-a)/pow(10,log(b-a)-8)))*pow(10,log(b-a)-8));
	else
		return a+(long double)(rand()%(int)(((b-a)*pow(10,8-log(b-a))))/pow(10,8-log(b-a)));
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

class vector
{
public:
	long double x,y,z;
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
	vector pos;
	SDL_Surface mat;
	particle(vector position,vector dim)
	{
			pos.x=position.x+rand()%(int)dim.x;
			pos.y=position.y+rand()%(int)dim.y;
			pos.z=position.z+rand()%(int)dim.z;
			pos.x=pos.y=pos.z=0;
	}
};
