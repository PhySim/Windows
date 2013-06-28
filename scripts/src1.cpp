#include <iostream>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Headers/physics.h"
using namespace std;


int main( int argc, char* args[] )
{
	int MaX=2;
	OBJECT** slot=new OBJECT*[2];
	SDL_Init( SDL_INIT_EVERYTHING );
	SDL_Surface *screen=SDL_SetVideoMode(720,480,32,SDL_SWSURFACE);
	SDL_WM_SetCaption("test...",NULL);

	SDL_Surface *dot=IMG_Load("images/dot.png");
	dot=SDL_DisplayFormat(dot);
	SDL_SetColorKey(dot,SDL_SRCCOLORKEY,SDL_MapRGB(dot->format,0,0xFF,0xFF));

	SDL_Rect pos;

	SDL_Event myevent;

	SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0,0xFF,0xFF));
	SDL_Flip(screen);

	for(int i=0;i<MaX;i++)
	{
		slot[i]=gen.object();
		slot[i]->x=rand()%800;
		slot[i]->y=rand()%480;
	}
	SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0xFF,0xFF,0xFF));
	SDL_Flip(screen);
	while(1)
	{
		MaX++;
		if(SDL_PollEvent(&myevent))
		{
			if(myevent.type==SDL_KEYDOWN)
			{
				if(myevent.key.keysym.sym==SDLK_UP)
				{
					//ob1->vy--;
				}
				if(myevent.key.keysym.sym==SDLK_DOWN)
				{
					//ob1->vy++;
				}
			}
			if(myevent.type==SDL_QUIT)
			{
				SDL_Quit();
				return 0;
			}
		}
		SDL_Delay(100);
		SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0xFF,0xFF,0xFF));
		for(int i=0;i<MaX;i++)
		{
			slot[i]->vy+=0.98;
			slot[i]->pup();
			pos.x=slot[i]->x;
			pos.y=slot[i]->y;
			SDL_BlitSurface(dot,NULL,screen,&pos);
		}
		SDL_Flip(screen);
	}
	SDL_FreeSurface(screen);
	SDL_FreeSurface(dot);

	SDL_Delay(2000);
}
