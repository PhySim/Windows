#include "SDL/SDL.h"
#include <headers/physics.h>
#include<stdio.h>


//add mouse support
//add zoom support
//add system of object concept
//add medium concept

HWND hwnd = FindWindowA("ConsoleWindowClass",NULL);
HDC hdc=GetDC(hwnd);
void zoom();
void menu();
void mouse_update();
void main()
{
	Tf=16;
   T=1.0/8;
   genrate=0.01;
   physim_initialize();
   mnd=10;
   menu();
   defaultob=ball;
   DEFAULTOB=BALL;
   zmx=0;zmy=0;
   OBJECT* temp=gen.object();
   temp->x=200;temp->y=200;
   temp->vx=temp->vy=0;
   OBJECT* temp2=gen.object();
   temp2->x=250;temp2->y=250;
   temp2->vx=temp2->vy=0;
   OBJECT* temp3=gen.system();
   temp3->attach(temp);
   temp3->attach(temp2);

   while(Go)  	//limiting the program runtime duration to prevent lags during compile time
   {
   	mouse_update();
      if(Tf>maxfps)
      	goto simulate;
      else if(loop%int(maxfps/Tf)==0)
      {
      	simulate:
         //gen.autoobject();
         for(int i=0;i<N;i++)
         {
            if(A[i]!=NULL)
            {
               if(A[i]->s!=deleted)
               {
                  //main thread..............................................
                  for(int j=0;j<N;j++)
                  {
                     if(i!=j&&A[j]!=NULL)
                     {
                        if((*A[j]).s!=deleted&&A[j]->type!=attached&&A[j]->type!=SYSTEM)
                        {
                           //interaction main thread...........................
                           if(A[i]->ds(*A[j])<20)
                           {
                           	//A[i]->col(*A[j]);

                              goto next;
                           }
                           	//A[i]->grv(*A[j]);
                           	//A[i]->elstat(*A[j]);

                           //end of interaction main thread....................
                        }
                     }
                     next:;
                  }
                  //A[i]->fric();

                  //if(A[i]->m>10)
                  	//expl(*A[i],40);

                  	if(A[i]->issystem())
                     {
                     	A[i]->calc_center_of_mass();
                        A[i]->calc_moment_of_inertia();
                     }
                     if(A[i]->m)
                     {
                     	if(!A[i]->isattached()&&!A[i]->issystem())
                        {
                           A[i]->rev();
                           A[i]->pup();
                        }
                        if(!A[i]->issystem())
                        {
                        	A[i]->addforce(0,9.8*A[i]->m);
                        }
                     	if(A[i]->issystem())
                     	{
                     		A[i]->force_resolution();
                           A[i]->pup();
                           //A[i]->repercussion();
                     	}
                     }

                  //end of main thread.......................................
               }
            }
         }
         //clear screen and display points
         SDL_FillRect(screen,&screen->clip_rect,SDL_MapRGB(screen->format,0xFF,oxFF,oxFF)));//paint screen white
         for(int i=0;i<N;i++)
            if(A[i]!=NULL)
            {
               if(A[i]->s!=deleted)//&&A[i]->x>xb/zm&&A[i]->x<x0/zm)
               {
                  if(i==sel)
                     Rectangle(hdc,A[i]->x*zm+zmx,A[i]->y*zm+zmy,(A[i]->x)*zm+10+zmx,(A[i]->y)*zm+10+zmy);
                  else
                  {
                     SetPixel(hdc,A[i]->x*zm+zmx,A[i]->y*zm+zmy,A[i]->c);
                     SetPixel(hdc,A[i]->x*zm+1+zmx,A[i]->y*zm+zmy,A[i]->c);
                     SetPixel(hdc,A[i]->x*zm+zmx,A[i]->y*zm+1+zmy,A[i]->c);
                     SetPixel(hdc,A[i]->x*zm+1+zmx,A[i]->y*zm+1+zmy,A[i]->c);
                  }
               }
            }
            ++f;
      }
      Sleep(min_delay);
      user();
//      cout<<A[sel]->m;
      gotoxy(1,25);printf("%2\i Particles Type:%i Frame:%-5i Lapse=10^%-3.1f Zoom=10^%-3.1f Mouse(%-3.2f,%-3.2f)",RN,DEFAULTOB,f,(log(T)),log(zm),double(mx),double(my));
      gotoxy(1,1);
		if(A[sel]!=NULL)
      {
      	cout<<"\n\n\n"<<A[sel]->fx<<"	,	"<<A[sel]->fy<<'\n';
      }
      ++loop;			//increment frame number
   }
}
void menu(){
	printf("   started!");
   printf("\nKeys:\n\nG		-	Generate new object\n+\-(in numberpad)-	to modify framerate\npageup&pagedown	-	to modify simulation speed\n");
   printf(";/'		-	zoom control\n");
   printf("Home&end	-	used to select next&previous objects\nDelete	  	-	Deletes currently selected object\n\n");
   system("pause");
   clrscr();
}
void mouse_update(){
	if (GetCursorPos(&p)){
    		//cursor position now in p.x and p.y
      	ScreenToClient(hwnd,&p);
         //p.x and p.y are now relative to hwnd's client(window) area
         mx=p.x;my=p.y;
         mx-=zmx;my-=zmy;
         mx/=zm;my/=zm;
	}
}
void zoom(){
	zm*=2;
	for(int y=0;y<yb;y+=10){
		for(int x=0;x<10;x++)
			SetPixel(hdc,x,y,RGB(250,0,0));}
   for(int x=0;x<xb;x+=10){
   	for(int y=yb;y<yb+10;y++)
			SetPixel(hdc,x,y,RGB(50,250,250));
   }
}
