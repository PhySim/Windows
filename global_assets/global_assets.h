/*
 * SDL_class.h
 *
 *  Created on: Feb 5, 2014
 *      Author: Reuben
 */


#include <windows.h>
#include <math.h>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_mixer.h"

#include "vect.hpp"
#include "framer.hpp"
#include "graphic_string.hpp"

#ifndef SDL_CLASS_H_
#define SDL_CLASS_H_

struct FONT
{
	string font_loc;
	unsigned int size;
	TTF_Font* font;
	FONT(const char* U_font_loc,unsigned int U_font_size,TTF_Font* U_font=NULL)
	{
		font_loc=U_font_loc;
		size=U_font_size;
		if(font)
			font=U_font;
		else
			TTF_OpenFont(U_font_loc,U_font_size);
	}
};

class FONT_POCKET
{
	vector<FONT>fonts;
	FONT* default_font;
public:
	TTF_Font* new_font(const char* U_font_loc,unsigned int U_font_size)
	{
		for(unsigned int i=0;i<fonts.size();i++)
		{
			if(fonts[i].size==U_font_size)
			{
				if(fonts[i].font_loc.compare(U_font_loc)==0)
				{
					return fonts[i].font;
				}
			}
		}
		TTF_Font* font=TTF_OpenFont(U_font_loc,U_font_size);
		if(font)
		{
			if(!default_font)
				default_font=new FONT(U_font_loc,U_font_size,font);
			fonts.push_back(FONT(U_font_loc,U_font_size,font));
			ofstream fout("logs/allocation log.txt",ios::app);
			fout<<"Font "<<U_font_loc<<" "<<U_font_size<<" ("<<fonts[fonts.size()-1].font<<") loaded into slot "<<(fonts.size()-1)<<'\n';
			fout.close();
			return fonts[fonts.size()-1].font;
		}
		return NULL;
	}
	TTF_Font* new_font()
	{
		if(default_font)
			return default_font->font;
		else
			return NULL;
	}
	FONT_POCKET()
	{
		default_font=NULL;
	}
	~FONT_POCKET()
	{
		ofstream fout("logs/allocation log.txt",ios::app);
		fout<<"\nEmptying font pocket("<<fonts.size()<<" fonts to close):";
		for(unsigned int i=fonts.size()-1;i>=0;i--)
		{
			if(fonts[i].font)
			{
				try
				{
					TTF_CloseFont(fonts[i].font);
					fout<<"Font "<<i<<" closed\n";
				}
				catch(...)
				{
					fout<<"Closing font "<<i<<" failed!\n";
				}
			}
		}
		fout.close();
	}
};

const char* millisecond_fotmatted_string(int milliseconds)
{
	string result;
	bool negative=milliseconds<0;
	if(negative)
		milliseconds=-milliseconds;
	int seconds = (int) (milliseconds / 1000) % 60 ;
	int minutes = (int) ((milliseconds / (1000*60)) % 60);
	int hours   = (int) ((milliseconds / (1000*60*60)) % 24);

	int remaining_milliseconds= (int) (milliseconds) % 1000 ;

	char C[4]="";
	itoa(hours,C,10);
	if(strlen(C)==1)
		result="0";
	result+=C;result+=":";
	itoa(minutes,C,10);
	if(strlen(C)==1)
		result+="0";
	result+=C;result+=":";
	itoa(seconds,C,10);
	if(strlen(C)==1)
		result+="0";
	result+=C;result+=".";
	itoa(remaining_milliseconds,C,10);
	if(strlen(C)==1)
		result+="0";
	result+=C;

	//----------------------------
	result.erase(0,3);
	if(negative)
	{
		result.insert(0,1,'-');
	}
	return result.c_str();
}

const char* decimal_format_string(double d,const char* format="%8.2f")
{
	string temp;
	char U[30];
	sprintf(U,format,d);
	temp.assign(U);
	return temp.c_str();
}


/**
 * function used to load an image from the specified location into an SDL_Surface pointer and return it
 */
SDL_Surface* loadimage(string filename)
{
	SDL_Surface* tex=IMG_Load(filename.c_str());
	if(tex!=NULL)
	{
		tex=SDL_DisplayFormat(tex);
		if(tex!=NULL)
		{
			SDL_SetColorKey(tex,SDL_SRCCOLORKEY,SDL_MapRGB(tex->format,0,0xFF,0xFF));
			if(tex==NULL)
				debugger.found("loadimage()","SDL_SetColorKey() failed");
		}
		else
			debugger.found("loadimage()","SDL_DisplayFormat() failed");
	}
		else
			debugger.found("loadimage()","IMG_Load() failed");
	return tex;
}
/**
 * function that copies an SDL_Surface passed to it and returns the SDL_Surface pointer to the newly created SDL_Surface
 */
SDL_Surface* copy_surface(SDL_Surface* source)
{
	return SDL_ConvertSurface(source, source->format, source->flags);
}

class SDL_2D
{
public:
	SDL_2D &SDL_2D_obj;
   SDL_Surface* scr;
   vect<> scr_origin,scr_dim;
   SDL_Event event;
   HWND hwnd;
   bool quit,windowed;
	FONT_POCKET font_pocket;
   SDL_2D(SDL_Surface* screen)
	:
		SDL_2D_obj(*this)
   {
	   quit=false;
	   windowed=true;
	   hwnd=FindWindowA("ConsoleWindowClass",NULL);
	   ShowWindow(hwnd,false);
	   SDL_Init(SDL_INIT_EVERYTHING);
	   SDL_EnableUNICODE(SDL_ENABLE);
	   TTF_Init();
	   Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 );
	   Mix_Init(MIX_INIT_MP3);
	   scr=screen;
	   if(screen)
		   scr_dim=(vect<>){(long double)screen->w,(long double)screen->h,(long double)screen->format->BitsPerPixel};
   }
   ~SDL_2D()
   {
		   Mix_Quit();
		   TTF_Quit();
		   SDL_Quit();
   }
   void wait()
   {
		   bool stop=false;
		   while(SDL_PollEvent(&event));
		   SDL_Event e;
		   while(!quit&&!stop)
		   {
				   if(SDL_WaitEvent(&e))
				   {
						   if(e.type==SDL_KEYDOWN)
								   stop=true;
						   if(e.type==SDL_QUIT)
								   quit=true;
				   }
		   }
   }
   void pause(unsigned int pause_time)
   {
		   timer T;
		   T.start();
		   while(SDL_PollEvent(&event));
		   SDL_Event e;
		   while(!quit&&T.elapse()<pause_time)
		   {
				   if(SDL_PollEvent(&e))
				   {
						   if(e.type==SDL_QUIT)
								   quit=true;
				   }
				   SDL_Delay(1);
		   }
   }
   int poll_event()
   {
		   return SDL_PollEvent(&event);
   }
   int wait_event()
   {
		   return SDL_WaitEvent(&event);
   }
   void SDL_handle_events()
   {
		   if(event.type==SDL_QUIT)
		   {
				   quit=true;
		   }
   }
   void update()
   {
		   if(scr)
				   SDL_Flip(scr);
   }
   bool toggle_fullscreen(vect<> screen_dimensions)
   {
	   if( windowed == true )
	   {
		   scr = SDL_SetVideoMode( screen_dimensions.x, screen_dimensions.y, screen_dimensions.z, SDL_SWSURFACE | SDL_FULLSCREEN);

		   if(scr)
		   {
				   return windowed = false;
		   }
		   else
		   {
				   return false;
		   }
	   }
	   else
	   {
		   scr = SDL_SetVideoMode( screen_dimensions.x, screen_dimensions.y , screen_dimensions.z, SDL_SWSURFACE | SDL_RESIZABLE);
		   if(scr)
		   {
				   return windowed = true;
		   }
		   else
		   {
				   return false;
		   }
	   }
	   return false;
   }
   /**
	* changes the color of the pixel at (x,y) of the SDL_Surface that is passed using the color that is passed
	*/
   void put_pixel(int x, int y, SDL_Color color)
   {
	   //Convert the pixels to 32 bit
	   Uint32 *pixels = (Uint32 *)scr->pixels;
	   Uint32 pixel=SDL_MapRGB(scr->format,255, 0, 0);
	   //Set the pixel
	   pixels[ ( y * scr->w ) + x ] = pixel;
   }
   /**
	* draws a line to the SDL_Surface that is passed from coordinates (x1,y1) to (x2,y2)
	* using the color that is passed int the SDL_Color variable
	* (by simply calls the put_pixel() many times)
	*/
   void draw_line(float x1, float y1, float x2, float y2, const SDL_Color& color )
       {
       	if( SDL_MUSTLOCK( scr ) )
       	{
       		//Lock the surface
       		SDL_LockSurface( scr );
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
			put_pixel(y,x, color);
		   }
		   else
		   {
			put_pixel(x,y, color);
		   }

		   error -= dy;
		   if(error < 0)
		   {
			   y += ystep;
			   error += dx;
		   }
	   }
	   if( SDL_MUSTLOCK( scr ) )
	   {
		   SDL_UnlockSurface( scr );
	   }
   }
	/**
	 * used to apply an SDL_Surface onto the scr(SDL_Surface variable that represents the screen)
	 * at the coordinates given by pos
	 */
	void applysurface(SDL_Surface* image,vect<> pos=(vect<>){0,0,0} )
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
	 */
	void applysurface(SDL_Surface* image,SDL_Rect rect)
	{
		if(scr==NULL)
			debugger.found("applysurface()","::scr is pointing to NULL");
		else
		{
			SDL_BlitSurface(image,NULL,scr,&rect);
		}
	}
	/**
	 * used to apply an SDL_Surface onto the scr(SDL_Surface variable that represents the screen)
	 * at the coordinates given by pos
	 * after being rotated by a user_angle
	 * and scaled up by a factor of 'zoom'
	 */
	void applysurface(SDL_Surface* image,vect<> pos,vect<> user_angle,double zoom)
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
};

class SDL_3D:public SDL_2D
{
public:
	vect<> world_dim,camera_pos;
	float aov;
	SDL_3D(SDL_Surface* screen):SDL_2D(screen)
	{
		change_AngleOfView();
		change_world_dimensions((vect<>){100,100,100});
		vect<> change_cameraPosition(world_dim/2);
	}
	float AngleOfView()
	{
		return aov;
	}
	vect<> cameraPosition()	//accesses camera_pos
	{
		return camera_pos;
	}
	vect<> change_world_dimensions(vect<> world_dimensions)
	{
		return world_dim=world_dimensions;
	}
	vect<> change_camera_position(vect<> camera_position)
	{
		return camera_pos=camera_position;
	}
	float change_AngleOfView(float AOV=M_PI/4)
	{
		return aov=AOV;
	}
	/**
	 * gets information about the camera from the PHYSIM reference (P)
	 * and returns a vector representing the apparent 2D coordinates (on the screen)
	 * where the user should observe it
	 */
	vect<> apparent_pos_of(vect<> pos)
	{
		vect<> relPos=(pos-camera_pos);
		return scr_dim/2+(scr_dim/2)*relPos/(relPos.z*tan(AngleOfView()));
	}
	/*similar but inverse of apparent_pos_of
	i.e. this one accepts an apparent position and
	the desired distance from the screen and
	returns the real position*/
	vect<> real_position_of(vect<> apparentPos,long double l)
	{
		vect<> real_pos=((apparentPos-scr_dim/2)/(scr_dim/2))*(l*tan(AngleOfView()));
		real_pos.z=l;

		real_pos+=camera_pos;
		return real_pos;
	}
	/**
	 * Similar to the Drawline() exept it accepts vect<> variables as coordinates instead of (float,float)
	 */
	void draw_vect_line(vect<> a,vect<> b,SDL_Color color)
	{
		vect<> appPos_a=apparent_pos_of(a),appPos_b=apparent_pos_of(b);
		if(
			appPos_a.x>scr->clip_rect.x+50
			&&appPos_b.x<scr->clip_rect.x+scr->clip_rect.w-50
			&&appPos_a.y>scr->clip_rect.y+50
			&&appPos_b.y<scr->clip_rect.y+scr->clip_rect.h-50
			)
			draw_line(appPos_a.x,appPos_a.y,appPos_b.x,appPos_b.y,color);
	}
};

class GRAPHIC_STRING
{
protected:
	SDL_2D &SDL_2D_obj;
	SDL_Rect rect;
	SDL_Surface* image;
	TTF_Font* font;
	SDL_Color col;
	string all;
	unsigned int update_interval;
	timer imagetimer;
	bool fresh_data;
	void general_set(const char* new_string,bool force_render=false)
	{
		all.assign(new_string);
		render_image(force_render);
	}
public:
	SDL_Rect rectangle()
	{
		return rect;
	}
	virtual void operator=(const char* text)
	{
		general_set(text,all.empty());
	}
	virtual void operator=(string newstring)
	{
		general_set(newstring.c_str(),all.empty());
	}
	virtual void operator=(int i)
	{
		char U[10];
		itoa(i,U,10);
		general_set(U,all.empty());
	}
	virtual void operator=(double d)
	{
		general_set(decimal_format_string(d),all.empty());
	}
	void set_time(int milliseconds)
	{
		all.clear();
		bool negative=milliseconds<0;
		if(negative)
			milliseconds=-milliseconds;
		int seconds = (int) (milliseconds / 1000) % 60 ;
		int minutes = (int) ((milliseconds / (1000*60)) % 60);
		int hours   = (int) ((milliseconds / (1000*60*60)) % 24);

		int remaining_milliseconds= (int) (milliseconds) % 1000 ;

		char C[4]="";
		itoa(hours,C,10);
		if(strlen(C)==1)
			all="0";
		all+=C;all+=":";
		itoa(minutes,C,10);
		if(strlen(C)==1)
			all+="0";
		all+=C;all+=":";
		itoa(seconds,C,10);
		if(strlen(C)==1)
			all+="0";
		all+=C;all+=".";
		itoa(remaining_milliseconds,C,10);
		if(strlen(C)==1)
			all+="0";
		all+=C;

		//----------------------------
		all.erase(0,3);
		if(negative)
		{
			all.insert(0,1,'-');
		}
	}
	bool operator==(const char* text)
	{
		return all.compare(text)==0;
	}
	bool operator!=(const char* text)
	{
		return !(all.compare(text)==0);
	}
	void set_font(TTF_Font * Font)
	{
		ofstream fout("logs/log.txt",ios::app);
		fout<<"GRAPHIC_STRING::set_font received font : "<<Font<<'\n';
		fout.close();
		if(Font)
		{
			font=Font;
			render_image(1);
		}
	}
	void set_position(unsigned int x,unsigned int y)
	{
			rect=((SDL_Rect){x,y});
	}
	void set_position(vect<> pos)
	{
		set_position(pos.x,pos.y);
	}
	void set_color(short unsigned int r=0,short unsigned int g=0,short unsigned int b=0)
	{
		col=(SDL_Color){r,g,b};
	}
	void set_update_interval(unsigned int Update_interval)
	{
		update_interval=Update_interval;
	}
	GRAPHIC_STRING(SDL_2D &P,TTF_Font* Font,unsigned int Update_interval);
	void render_image(bool forced=false)
	{
		if(imagetimer.elapse()>update_interval||forced)
		{
			if(image)
			{
				try
				{
					SDL_FreeSurface(image);
				}
				catch(...)
				{
					ofstream fout("logs/log.txt",ios::app);
					fout<<"GRAPHIC_STRING::render_image failed to free surface\n";
					fout.close();
				}
			}
			image=NULL;
			if(font)
			{
				if(!all.empty())
				{
					image=TTF_RenderText_Solid(font,all.c_str(),col);
					rect.w=image->clip_rect.w;
					rect.h=image->clip_rect.h;
				}
				else
				{
					ofstream fout("logs/log.txt",ios::app);
					fout<<"GRAPHIC_STRING::render_image string empty\n";
					fout.close();
					rect.w=rect.h=0;
					image=NULL;
				}
			}
			else
			{
				ofstream fout("logs/log.txt",ios::app);
				fout<<"GRAPHIC_STRING::render_image attempted NULL font Rendering\n";
				fout.close();
			}
			imagetimer.reset();
			imagetimer.start();
		}
	}
	void display(bool demand_rendering=false)
	{
		render_image(demand_rendering);
		if(image)
			SDL_2D_obj.applysurface(image,rect);
		else
		{
			ofstream fout("logs/log.txt",ios::app);
			fout<<"GRAPHIC_STRING::display image NULL access\n";
			fout.close();
		}
	}
	virtual ~GRAPHIC_STRING()
	{
		ofstream fout("logs/allocation log.txt",ios::app);
		fout<<"GRAPHIC_STRING destroyed\n";
		fout.close();
		if(image)
			SDL_FreeSurface(image);
		all.clear();
	}
};

class GRAPHIC_STRING_INPUT:public GRAPHIC_STRING
{
	char new_char;
	timer imagetimer;
	timer start,repeat;
	unsigned int start_time,repeat_time;
	bool done;
	SDL_Color TextColor;
	void reset()
	{
		done=false;
		fresh_data=false;
		all.clear();
		if(image)
		{
			SDL_FreeSurface(image);
			image=NULL;
			rect.w=rect.h=0;
		}
		imagetimer.reset();
		imagetimer.start();

	}
public:
	GRAPHIC_STRING_INPUT(SDL_2D P,TTF_Font* U_font,unsigned int UstartT,unsigned int UrepeatT,unsigned int Ugraphic_update_interval);
	~GRAPHIC_STRING_INPUT()
	{
		SDL_FreeSurface(image);
	}
	void operator=(const char* text)
	{
		GRAPHIC_STRING::operator=(text);
	}
	void operator=(string newstring)
	{
		GRAPHIC_STRING::operator=(newstring);
	}
	void operator=(int i)
	{
		GRAPHIC_STRING::operator=(i);
	}
	void operator=(double d)
	{
		GRAPHIC_STRING::operator=(d);
	}
	bool completed()
	{
		return done;
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
						//do nothing
					break;
					case SDLK_BACKSPACE:
						if(all.size()>0)
							all.erase(all.size()-1);
					break;
					case SDLK_RETURN:
						finished();
					break;
					default:
						all+=new_char;
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
	const char* get()
	{
		return all.c_str();
	}
	void finished(bool U=true)
	{
		done=U;
		if(!U)
		{
			reset();
		}
	}
};

GRAPHIC_STRING::GRAPHIC_STRING(SDL_2D &P,TTF_Font* Font=NULL,unsigned int Update_interval=100):SDL_2D_obj(P)
{
	image=NULL;
	rect.w=rect.h=0;
	font=Font;
	col=(SDL_Color){0,0,0};
	set_position(10,10);
	update_interval=Update_interval;
	fresh_data=false;
	all.clear();
	imagetimer.start();
}
GRAPHIC_STRING_INPUT::GRAPHIC_STRING_INPUT(SDL_2D P,TTF_Font* U_font=NULL,unsigned int UstartT=500,unsigned int UrepeatT=25,unsigned int Ugraphic_update_interval=50)
:
		GRAPHIC_STRING(P,U_font,Ugraphic_update_interval)
{
	start_time=UstartT;repeat_time=UrepeatT;
	new_char=0;
	finished();
}

#endif /* SDL_CLASS_H_ */
