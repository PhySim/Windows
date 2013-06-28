//#include<iostream.h>
#include<conio.h>
#include<windows.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>

using namespace std;
//***********************************************************************************************************************************************************************************************************************************
//i made changes :
//fx is now m
//fy is now e
//i added a new gravity equation
//changed the explode function


bool Go=true;
int N=0,RN=0,Char=0,sel=0;
const int x_0=5,y_0=5,xb=950,yb=480;	//sets the limits/boundries of the objects
long double zmx,zmy,mx=0,my=0;//top left draw co-ordinates(depends on zoom level) and mouse co-ordinates
double T=1;	//the number of virtual seconds that will be run in the program for every second in the real world(basically controls the rate of the program)
double Tf=16; //controls the frame rate of the program
int min_delay=5;
int f=0,maxfps=1000/min_delay,loop=0;			//frame counting variable
double k=0,genrate=1,zm=1,zoomrate=1.1,navrate=0.1;	//starting object number	,	objects to generate per second...
double G= 6.67398 * pow(10,-11),ep0=8.854187817*pow(10,-12),mnd=5;
const int SYSTEM=-1,BALL=0,EARTH=1,MOON=2,PROTON=3,ELECTRON=4,CERES=5;int DEFAULTOB=BALL;
const int deleted=-127677,attached=-127766;
POINT p;

long double arcsin(long double t)
{
	int asd = rand()%200;
	cout<<asd;
	long double ret=0;
   for(int i=0;i<9;i++)
   {
   		int d=1;
		for(int j=2;j<=i*2;j+=2)
      		if(j%2==0)
      			d*=j;
		d*=(i*2+1);
		ret+=pow(t,(i*2+1))/d;
   }
   return ret;
}
class MEDIUM
{
	public:
	long double fr,ep;
}vaccum={0,1},oil={0.01,1};
MEDIUM medium(oil);
class OBJECT;
class OBJECT
{
	public:
		long double x,y,vx,vy,ax,ay,m,fx,fy,q,e,size;
		long double r,I,tta,om,al,af;
		long double dso;
		OBJECT* ad;
		OBJECT* sys;
		int type,s;
		COLORREF c;
		void reset()
		{
					s=N;
					sys=NULL;
					x=0;
					y=0;
					vx=0;
					vy=0;
					ax=0;
					ay=0;
					m=1;
					fx=0;
					fy=0;
					q=0;
					e=1;
					size=1;
					c=RGB(rand()%255,rand()%255,rand()%255);
					type=BALL;
		}
		void randompos()
		{
			x=xb*0.9-rand()%(int)(xb*0.9);
			y=yb*0.9-rand()%(int)(xb*0.9);
		}
		void randomcol()
		{
			c=RGB(rand()%200,rand()%200,rand()%200);
		}
		void randomize()
		{
			randompos();
			randomcol();
		}
		void addvel(long double velx=0,long double vely=0)
		{
			vx+=velx;
			vy+=vely;
		}
		void addforce(long double forx=0,long double fory=0)
		{
			fx+=forx;
			fy+=fory;
		}
		long double ds(OBJECT a)     //check distance between 2 objects
		{
			if(sqrt(pow((x-a.x),2)+pow((y-a.y),2))==0)
			{
				return dso=pow(-12,10);
			}
			else
			{
				return dso=sqrt(pow((x-a.x),2)+pow((y-a.y),2));
      		}
		}
		void pup()       	//physics updation function:takes care of the basic motion of the object
		{
			ay=T*fy/m;

			ax=T*fx/m;
			vy+=ay*T;//vy+=ay	//(differential) acceleration along y axis(the 5th column of the row passed to the function) is added to velocity along y axis(column 3)

			vx+=ax*T;//vx+=ax	//(differential) acceleration along x axis(the 4th column of the row passed to the function) is added to velocity along x axis(column 2)

			vy+=ay*T;//xy+=vy	//(differential) velocity along y axis(the 3rd column of the row passed to the function) is added to position along y axis(column 1)

			x+=vx*T;//xx+=vx	//(differential) velocity along y axis(the 2nd column of the row passed to the function) is added to position along y axis(column 0)
			
			y+=vy*T;//you get the point...

			if(!isattached())//only object that are not attached have to be reset(cuz for attached objects, their system resets these values in force_resolution())
			{
				fx=0;	//reset for next frame...
				fy=0;
				ax=0;
				ay=0;
			}
			if(issystem())
			{
				al=T*af/I;
				om+=al*T;
				tta+=om*T;
				//repercussion();
			}
		}	  
		int bnd()       //TO CHECK IF OBJECT (or in this program, the row of the global character array 'A' passed to this function has its first two values (its co-ordinates) within the boundries specified
		{
			if(x<x_0)
   			return 1;

			else if(y<y_0)
   			return 2;		 		//x axis boundry and y axis boundry
			
			else if(x>xb)
			return 3;
			
			else if(y>yb)
			return 4;
			
			else
   			return 0;

		}
		int rev(int n=0)	//the direction to be reversed is specified by the bnd() and then it is reversed in that direction || n=deleted is set as a default value so that we know when a value has not been passed
		{
			int lp=0;
			while(1)
			{
				if(n==0)
					n=bnd();
				lp++;
				switch(n)
				{
					case 1:                 //revering the x axis velocity and seting the x position
						vx*=-e;
						x=x_0-(x-x_0);
						if(lp>9)
							x=x_0;
						break;
					case 2:                //revering the y axis velocity and seting the y position
						vy*=-e;
						y= y_0 -(y - y_0);
						if(lp>9)
							y=y_0;
						break;
					case 3:               //revering the x axis velocity and seting the x position
						vx*=-e;
						x=xb-x+xb;
						if(lp>9)
							x=xb;
						break;
					case 4:              //revering the y axis velocity and seting the y position
						vy*=-e;
						y=yb-(y-yb);
						if(lp>9)
							y=yb;
						break;
					default:
						return 0;
					break;
				}
				n=bnd();
			}
			return 1;
		}
		void grv(OBJECT b)
		{
			double g=((G*m*b.m)/dso/dso); // F=GMm/r     <----no r-square because it is in a 2d environment (in 3d environment r-square is used)
			fx+=g*((b.x-x)/dso);
			fy+=g*((b.y-y)/dso);
		}
		void elstat(OBJECT b)
		{
			long double f=-(q*b.q)/((4*3.14159*ep0)*(dso*dso));
			//cout<<'\n'<<f;
			fx+=f*((b.x-x)/dso);
			fy+=f*((b.y-y)/dso);
		}
		void fric()
		{
			fx-=m*vx*fabs(vx)*medium.fr;
			fy-=m*vy*fabs(vy)*medium.fr;
		}
		void col(OBJECT &b)
		{
			double a2=((m-b.m)*vx)/(m+b.m) + 2*b.m*b.vx/(m+b.m), a3 = ((m-b.m)*vy)/(m+b.m) + 2*b.m*b.vy/(m+b.m), b2 = ((b.m-m)*b.vx)/(m+b.m) + 2*m*vx/(m+b.m), b3 = ((b.m-m)*b.vy)/(m+b.m) + 2*m*vy/(m+b.m);
			vx= a2;
			vy= a3;
			b.vx= b2;
			b.vy= b3;
		}
		void com(OBJECT &);
		void expl(int);
		void is(OBJECT a)
		{
			m=a.m;
			q=a.q;
			e=a.e;
			c=a.c;
			type=a.type;
		}

		int N,Na;
		OBJECT** member;
		void systemize()
		{
			type=SYSTEM;
			m=0;s=0;
			N=10;Na=0;
			member=new OBJECT*[N];
		}
		int search(OBJECT* a)
		{
			for(int i=0;i<Na;i++)
			{
				if(member[i]==a)
					return i;
			}
			return -1;
		}
		bool issystem()
		{
			return (type==SYSTEM);
		}
		bool isattached()
		{
			return (type==attached);
		}
		bool isobject()
		{
			return !(issystem()||isattached());
		}	//prone to errors!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		bool overflow()
		{
			return (Na>=N);
		}
		void rebuild()
		{
			OBJECT** newarray=new OBJECT*[2*N];
			for(int i=0;i<N;i++)
				newarray[i]=member[i];
			
			N*=2;
			member=newarray;
		}
		void attach(OBJECT* a);
		void calc_center_of_mass()
		{
		x=0;y=0;m=0;
		for(int i=0;i<Na;i++)
		{
			x+=member[i]->x*member[i]->m;
			y+=member[i]->y*member[i]->m;
			m+=member[i]->m;
		}
		x/=m;
		y/=m;
		}

		void calc_my_local_position()
		{
			if(isattached())
			{
				r=sqrt((sys->x-x)*(sys->x-x)+(sys->y-y)*(sys->y-y));
				tta=arcsin((sys->y-y)/r);
			}
		}
		void calc_local_position()
		{
			if(issystem())
			{
				for(int i=0;i<Na;i++)
				{
					member[i]->r=sqrt((x-member[i]->x)*(x-member[i]->x)+(y-member[i]->y)*(y-member[i]->y));
               if(member[i]->r)
						member[i]->tta=arcsin((y-member[i]->y)/member[i]->r);
               else member[i]->tta=0;
               cout<<"r="<<member[i]->r<<','<<"tta="<<member[i]->tta<<'\n';
               system("pause");
				}
			}
		}
		void calc_moment_of_inertia()
		{
			I=0;
			calc_local_position();
			for(int i=0;i<Na;i++)
			{
				I+=member[i]->I=member[i]->m*member[i]->r*member[i]->r;
			}
		}
		void force_resolution()
		{
			fx=fy=0;
			long double m1,m2,tan,f,F;//,T;
			for(int i=0;i<Na;i++)
			{
					if(x-(member[i]->x))
						m1=(y-member[i]->y)/(x-member[i]->x);
					else m1=pow(10,10);
					if(member[i]->fx)
							m2=member[i]->fy/member[i]->fx;
					else m2=pow(10,10);
					tan=(m2-m1)/(1+m1*m2);
					f=sqrt(member[i]->fx*member[i]->fx+member[i]->fy*member[i]->fy);

					F=f/pow((1+tan*tan),0.5);
					fx=F/pow((1+tan*tan),0.5);
					fy=F/pow((1+1.0/tan/tan),0.5);

					af=f/pow((1+1.0/tan/tan),0.5)*sqrt((x-member[i]->x)*(x-member[i]->x)+(y-member[i]->y)*(y-member[i]->y));	//force x perpendicular distance

					member[i]->fx=member[i]->fy=0;
			}
			cout<<"m1="<<m1<<','<<"m2="<<m2<<"tan="<<tan<<','<<"f="<<f<<','<<"F="<<F<<','<<"fx="<<fx<<','<<"fy="<<fy<<','<<"T="<<af<<'\n';
			getch();
		}
		void repercussion()
		{
			for(int i=0;i<Na;i++)
			{
				cout<<"________"<<member[i]->x<<','<<member[i]->y<<'\n';
				member[i]->x=x+r*cos(tta);
				member[i]->y=y+r*sin(tta);
				cout<<"________"<<member[i]->x<<','<<member[i]->y<<'\n';
			}
		}
		void del(OBJECT* a)
		{
			int temp=search(a);
			if(temp!=-1)
			{
				del(member[temp]);
			}
		}
		OBJECT()
		{
			type=BALL;reset();
		}
}*A[1000],ball,earth,moon,proton,electron,ceres,defaultob,defaultsy(ball),*clicked,*t;

class GENERATOR
{
	public:

	OBJECT* object(OBJECT a=defaultob)
	{
		for(int i=0;i<N;i++)
		{
			if(A[i]==NULL)
			{
				A[i]=new OBJECT(a);
				A[i]->randomize();
				A[i]->ad=A[i];
				A[i]->s=i;
				RN++;
				return A[i];
			}
		}
		A[N++]=new OBJECT(a);
		A[N-1]->randomize();
		A[N-1]->ad=A[N-1];
		A[N-1]->s=N-1;
		RN++;
		return A[N-1];
	}
	void object(OBJECT* &a,OBJECT b=defaultob)         	//default values are assigned in a new row of the global array, the row number is passed to the function (function behaves as though a new object is generated)
	{
		a=new OBJECT(b);
		RN++;
	}
	void autoobject(OBJECT a);

	OBJECT* system()
	{
		OBJECT* temp=object();
		temp->ad=temp;
		temp->systemize();
		return temp;
	}
	OBJECT* system(OBJECT* &a)
	{
		a=new OBJECT();
		a->ad=a;
		a->systemize();
		return a;
	}
}gen;
void GENERATOR::autoobject(OBJECT a=defaultob)
{
	while(f>=k)
      {
      		object(a);
			k+=Tf/genrate;
      }
}

void kill(OBJECT &a)
{
	a.s=deleted;
	RN--;
}
void del(OBJECT* &a)
{
	delete a;
	a=NULL;
	RN--;
}
void OBJECT::com(OBJECT &b)  //a function that combines two objects to one on contact
{
	m+=b.m;
	vx=(m*vx+b.m*b.vx)/(m);
	vy=(m*vy+b.m*b.vy)/(m);
	kill(b);
}
void OBJECT::attach(OBJECT *a)
{
	if(!issystem())
	{
		gen.object(a);
		systemize();
	}
	member[Na++]=a;
	a->type=attached;
	a->sys=ad;
	x+=(a->x-x)/(Na);
	y+=(a->y-x)/(Na);
	a->calc_my_local_position();
	if(overflow())
	{
		cout<<"Rebuilding...";
		rebuild();
		system("pause");
	}
}

void expl(OBJECT &a,float parts=10)		
{
			a.m/=parts;
			long double tt=sqrt((2*3.14159)/parts);
  			long double l=0.5*mnd/(pow((1-cos(tt)),2)+pow(sin(tt),2));
			for(int i=1;i<parts-1;i++)
			{
				OBJECT &b=*gen.object(a);					//generates a particle (with default parameters)
				b.y=a.y+=a.vy+=sin((2*3.14159/(parts))*i)*l;
				b.x=a.x+=a.vx+=cos((2*3.14159/(parts))*i)*l;
				a.vx/=4;
				a.vy/=4;
			}
			a.x+=sin((2*3.14159/(parts)))*l;
			a.vx+=l/2;
}

class DEBUG
{
	public:
	long double dssq(OBJECT a,OBJECT b)     //check distance between 2 objects
	{
		return (pow((a.x-b.x),2)+pow((a.y-b.y),2));         //refers to the global data variable (A[][])
	}
	int sl(int d)
	{
		while(sel>(-d)&&sel<(N-d))
			{
				if(A[sel]==NULL)
					sel+=d;
				else if((*A[sel]).s==deleted)
					sel+=d;
				else return 1;
			}
		return 0;
	}
}debug;
long double ds(OBJECT a,OBJECT b)     //check distance between 2 objects
{
	if(pow((a.x-b.x),2)+pow((a.y-b.y),2))
		return sqrt(pow((a.x-b.x),2)+pow((a.y-b.y),2));         //refers to the global data variable (A[][])
	else
		return 0.000000001;
}
void dup(OBJECT a,OBJECT b)
{
	b.x=a.x;b.y=a.y;
	b.vx=a.vx;b.vy=a.vy;
	b.ax=a.ax;b.ay=a.ay;
	b.fx=a.fx;b.fy=a.fy;
	b.m=a.m;
	b.e=a.e;
}
void grv(OBJECT a, OBJECT b)
{
	double g=(G*a.m*a.m)/sqrt((pow((a.x-b.x),2)+pow((a.y-b.y),2))); // F=GMm/r     <----no r-square because it is in a 2d environment (in 3d environment r-square is used)
	a.fx+=g*((a.x-b.x)/sqrt((pow((a.x-b.x),2)+pow((a.y-b.y),2))));
	a.fy+=g*((a.y-b.y)/sqrt((pow((a.x-b.x),2)+pow((a.y-b.y),2))));
}
void rpl(OBJECT a, OBJECT b)
{
	double f=50*(G*a.m*b.m)/pow((pow((a.x-b.x),2)+pow((a.y-b.y),2)),2);
	a.fx-=(f)*((a.x-b.x)/sqrt((pow((a.x-b.x),2)+pow((a.y-b.y),2))));
	a.fy-=(f)*((a.y-b.y)/sqrt((pow((a.x-b.x),2)+pow((a.y-b.y),2))));
}
int bnd(OBJECT a)       //TO CHECK IF OBJECT (or in this program, the row of the global character array 'A' passed to this function has its first two values (its co-ordinates) within the boundries specified
{

   if(a.x<x_0)
   	return 1;

   else if(a.y<y_0)
   	return 2;		 		//x axis boundry and y axis boundry

	else if(a.x>xb)
   	return 3;

   else if(a.y>yb)
      return 4;

   else
   	return 0;

}
int rev(OBJECT a,int n=0)	//the direction to be reversed is specified by the bnd() and then it is reversed in that direction || n=deleted is set as a default value so that we know when a value has not been passed

{

	if(n==0)			//if the value of n is deleted as done by the default variable, then it means that a specific direction of reversal has not been specified and the function has to pick the direction of reversal automatically so it is sent to the bnd()
		n=bnd(a);
	while((n=bnd(a)))
	switch(n)
	{

		case 1:                 //revering the x axis velocity and seting the x position to 1
      		a.vx*=-a.e;
			a.x=x_0-(a.x-x_0);
		break;

		case 2:                //revering the y axis velocity and seting the y position to 1
			a.vy*=-a.e;
      		a.y= y_0 -(a.y - y_0);
		break;

		case 3:               //revering the x axis velocity and seting the x position to 80
      		a.vx*=-a.e;
			a.x=xb-a.x+xb;
		break;

		case 4:              //revering the y axis velocity and seting the y position to 23
			a.vy*=-a.e;
			a.y=yb-(a.y-yb);
		break;

      default:
      return 0;
   }

   return 1;



}
void col(OBJECT a,OBJECT b)
{
	double a2=((a.m-b.m)*a.vx)/(a.m+b.m) + 2*b.m*b.vx/(a.m+b.m), a3 = ((a.m-b.m)*a.vy)/(a.m+b.m) + 2*b.m*b.vy/(a.m+b.m), b2 = ((b.m-a.m)*b.vx)/(a.m+b.m) + 2*a.m*a.vx/(a.m+b.m), b3 = ((b.m-a.m)*b.vy)/(a.m+b.m) + 2*a.m*a.vy/(a.m+b.m);
   a.vx = a2;
   a.vy = a3;
   b.vx = b2;
   b.vy = b3;

}
void com(OBJECT a,OBJECT b)  //a function that combines two objects to one on contact

{
	
	if((a.m+b.m)==0)
   {
		a.m+=0.0001;
   }
   a.vx=(a.m*a.vx+b.m*b.vx)/(a.m+b.m);
   
   a.vy=(a.m*b.vy+a.m*b.vy)/(a.m+b.m);

   a.m+=b.m;

   //del(s-1);

}


void user()
{
	Char=0;
   for(int i=0;i<256;i++)
   	if(GetAsyncKeyState(i)==-32767)
   		Char=i;
   switch(Char)
   {
   case 1:
	   clicked=gen.object();
	   clicked->x=mx;clicked->y=my;
	break;
   	case 27:
   	Go=false;        //stops program
   	break;

   	case 37:
		if(A[sel]!=NULL)
  	 		A[sel]->addforce(-9.8*A[sel]->m);                   //coNtrols object speed
   	break;
	case 38:
   		if(A[sel]!=NULL)
  	 		A[sel]->addforce(0,-9.8*A[sel]->m);                   //coNtrols object speed
   	break;
   	case 39:
   		if(A[sel]!=NULL)
  	 		A[sel]->addforce(9.8*A[sel]->m);                   //coNtrols object speed
	break;
   	case 40:
   		if(A[sel]!=NULL)
  	 		A[sel]->addforce(0,9.8*A[sel]->m);                   //coNtrols object speed
   	break;
	case '0':
		defaultob=ball;
		DEFAULTOB=BALL;
	break;
	case '1':
		defaultob=earth;
		DEFAULTOB=EARTH;
	break;
	case '2':
		defaultob=moon;
		DEFAULTOB=MOON;
	break;
	case '3':
		defaultob=proton;
		DEFAULTOB=PROTON;
	break;
	case '4':
		defaultob=electron;
		DEFAULTOB=ELECTRON;
	break;
   	case 71:
   	gen.object();    //geNerates object
	break;
	case 'A':
		zmx+=navrate*xb/pow(zm,1/2);
	break;
	case 'W':
		zmy+=navrate*yb/pow(zm,1/2);
	break;
	case 'D':
		zmx-=navrate*xb/pow(zm,1/2);
	break;
	case 'S':
		zmy-=navrate*yb/pow(zm,1/2);
	break;
	case 107:
   	Tf*=2;         						//speeds up fps
	T/=2;
   	break;
   	case 109:
   	Tf*=0.5;                         //slows down fps
	T*=2;
   	break;
	case 33:
		T*=2;						//speeds up virtual time
	break;
	case 34:
		T/=2;						//slows down virtual time
	break;
	case 35:
		if(sel>0)
		sel--;
		if(!debug.sl(-1))
			debug.sl(1);
	break;
	case 36:
		if(sel<N-1)
			sel++;
		debug.sl(1);
		debug.sl(-1);
	break;
	case 46:
		if(A[sel]!=NULL)
		{
			if(A[sel]->s!=deleted)
			{
				if(A[sel]->sys!=NULL)
				{
					A[sel]->sys->del(A[sel]);
				}
            del(A[sel]);
				debug.sl(-1);
				debug.sl(1);
			}
		}
	break;
	case 222:
		zm*=zoomrate;
		zmx-=(xb-xb/zoomrate)/2;
		zmy-=(yb-yb/zoomrate)/2;
	break;
	case 186:
		zmx+=(xb-xb/zoomrate)/2;
		zmy+=(yb-yb/zoomrate)/2;
		zm/=zoomrate;
	break;
   }
}

void physim_initialize()
{
	printf("initialising...");
	srand(SDL_GetTicks());//randomize();
	N=0;
	RN=0;
	maxfps=1000/min_delay;
	ShowCursor(FALSE);

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Surface *screen=SDL_SetVideoMode(xb,yb,32,SDL_SWSURFACE);

	SDL_WM_SetCaption("PhySim	-	By Reuben John, Aayush Naik, Divanshu Jain",NULL);//SetConsoleTitle("PhySim	-	By Reuben John, Aayush Naik, Divanshu Jain");

					//----------------------------
					ball.vx=rand()%10-5;
					ball.vy=rand()%10/100-0.05;

					ball.ax=0;
					ball.ay=0;

					ball.m=1;

					ball.fx=0;
					ball.fy=0;
					ball.size=0.1;
					ball.q=0;

					ball.e=1;

					ball.c=RGB(rand()%255,rand()%255,rand()%255);
					ball.type=BALL;
					//-----------------------------
					defaultsy.x=0;
					defaultsy.y=0;
					defaultsy.vx=0;
					defaultsy.m=1;
					defaultsy.fx=0;
					defaultsy.fy=0;
					defaultsy.size=1;
					defaultsy.q=1;
					defaultsy.e=1;
					defaultsy.c=RGB(rand()%255,rand()%255,rand()%255);
					defaultsy.type=SYSTEM;

					//----------------------------
					earth.vx=0;
					earth.vy=0;
					earth.ax=0;
					earth.ay=0;
					earth.m=5.97219 * pow(10,24);
					earth.fx=0;
					earth.fy=0;
					earth.size=1.2742*pow(7,10);
					earth.q=0;
					earth.e=1;
					earth.c=RGB(rand()%255,rand()%255,rand()%255);
					earth.type=EARTH;
					//----------------------------
					moon.vx=0;
					moon.vy=0;
					moon.ax=0;
					moon.ay=0;
					moon.m=895.8*pow(22,10);
					moon.fx=0;
					moon.fy=0;
					moon.size=3.4748*pow(6,10);
					moon.q=0;
					moon.e=1;
					moon.c=RGB(rand()%55,rand()%55,rand()%55);
					moon.type=MOON;
					//----------------------------
					ceres.ax=0;
					ceres.ay=0;

					ceres.m=895.8*pow(22,10);

					ceres.fx=0;
					ceres.fy=0;
					ceres.size=9.3*pow(5,10);
					ceres.q=0;

					ceres.e=1;

					ceres.c=RGB(rand()%55,rand()%55,rand()%55);
					ceres.type=CERES;
					//----------------------------
					proton.vx=0;
					proton.vy=0;

					proton.ax=0;
					proton.ay=0;

					proton.m=1.67262158  * pow(10,-27);

					proton.fx=0;
					proton.fy=0;
					proton.size=1.11*pow(-15,10);
					proton.q=1.60217646*pow(10,-19);

					proton.e=1;

					proton.c=RGB(rand()%255,rand()%255,rand()%255);
					proton.type=PROTON;
					//----------------------------
					electron.vx=0;
					electron.vy=0;

					electron.ax=0;
					electron.ay=0;

					electron.m=9.10938188 * pow(10,-31);

					electron.fx=0;
					electron.fy=0;
					electron.size=5.635880578916*pow(-15,10);
					electron.q=-1.60217646*pow(10,-19);

					electron.e=1;
					electron.c=RGB(rand()%255,rand()%255,rand()%255);
					electron.type=ELECTRON;
					//----------------------------
}
