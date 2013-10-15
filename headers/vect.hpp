/*
 * vect.hpp
 *
 *  Created on: Jul 14, 2013
 *      Author: Reuben
 */

#ifndef VECT_HPP_
#define VECT_HPP_

#include <math.h>
#include <fstream>

using namespace std;

class vect
{
public:
	long double x,y,z;
	vect(long double user_x,long double user_y,long double user_z)
	{
		x=user_x;
		y=user_y;
		z=user_z;
	}
	vect()
	{
		x=y=z=0;
	}
	vect reverse()
	{
		vect temp;
		temp.x=-x;
		temp.y=-y;
		temp.z=-z;
		return temp;
	}
	vect operator=(vect b)
	{
		x=b.x;
		y=b.y;
		z=b.z;
		return *this;
	}
	bool operator==(vect b)
	{
		return (x==b.x&&y==b.y&&z==b.z);
	}
	bool operator!=(vect b)
	{
		return (x!=b.x&&y!=b.y&&z!=b.z);
	}
	vect operator+(vect b)
	{
		vect temp;
		temp.x=x+b.x;
		temp.y=y+b.y;
		temp.z=z+b.z;
		return temp;
	}
	vect operator-(vect b)
	{
		vect temp;
		temp.x=x-b.x;
		temp.y=y-b.y;
		temp.z=z-b.z;
		return temp;
	}
	vect operator*(vect b)
	{
		vect temp;
		temp.x=x*b.x;
		temp.y=y*b.y;
		temp.z=z*b.z;
		return temp;
	}
	vect operator*(long double scalar)
	{
		vect temp;
		temp.x=x*scalar;
		temp.y=y*scalar;
		temp.z=z*scalar;
		return temp;
	}
	vect operator/(long double scalar)
	{
		vect temp;
		temp.x=x/scalar;
		temp.y=y/scalar;
		temp.z=z/scalar;
		return temp;
	}
	long double operator|(vect b)
	{
		return x*b.x+y*b.y+z*b.z;
	}
	vect operator^(vect b)
	{
		vect temp;
		temp.x=y*b.z-z*b.y;
		temp.y=-(x*b.z-b.x*z);
		temp.z=x*b.y-b.x*y;
		return temp;
	}
	vect operator+=(vect b)
	{
		x+=b.x;
		y+=b.y;
		z+=b.z;
		return *this;
	}
	vect operator-=(vect b)
	{
		x-=b.x;
		y-=b.y;
		z-=b.z;
		return *this;
	}
	vect operator*=(vect b)
	{
		x*=b.x;
		y*=b.y;
		z*=b.z;
		return *this;
	}
	vect operator*=(long double scalar)
	{
		x*=scalar;
		y*=scalar;
		z*=scalar;
		return *this;
	}
	vect operator/=(long double scalar)
	{
		x/=scalar;
		y/=scalar;
		z/=scalar;
		return *this;
	}
	long double mag()
	{
		return sqrt(x*x+y*y+z*z);
	}
	long double separation(vect U)
	{
		vect temp(*this-U);
		return sqrt(temp.x*temp.x+temp.y*temp.y+temp.z*temp.z);
	}
	vect dir()
	{
		vect temp;
		temp.x=x;
		temp.y=y;
		temp.z=z;
		return temp/temp.mag();
	}
	vect rotate_about(vect U_rotation)
	{
		vect ax=U_rotation.dir()*sin(U_rotation.mag());	//ax :- axis of rotation
		long double ang=cos(U_rotation.mag());			//ang :- amount the vector is rotated about the axis
		vect rotated_pos;
		vect &p=*this;	//reference used to just shorten the length of the following lines
		//position after rotating about the axis is calculated as:
		rotated_pos.x = ((1-2*ax.y*ax.y-2*ax.z*ax.z)*p.x) + (2*(ax.x*ax.y+ang*ax.z)*p.y) + (2*(ax.x*ax.z-ang*ax.y)*p.z);
		rotated_pos.y = (2*(ax.x*ax.y-ang*ax.z)*p.x) + ((1-2*ax.x*ax.x-2*ax.z*ax.z)*p.y) + (2*(ax.y*ax.z+ang*ax.x)*p.z);
		rotated_pos.z = (2*(ax.x*ax.z-ang*ax.y)*p.x) + (2*(ax.y*ax.z+ang*ax.x)*p.y) + ((1-2*ax.x*ax.x-2*ax.y*ax.y)*p.z);
		return *this=rotated_pos;
	}
	vect reset()
	{
		x=y=z=0.0;
		return *this;
	}
};
vect rotated(vect vector,vect rotation)
{
	vect ax=rotation.dir()*sin(rotation.mag());	//ax :- axis of rotation
	long double ang=cos(rotation.mag());			//ang :- amount the vector is rotated about the axis
	vect rotated_pos;
	vect &p=vector;	//reference used to just shorten the length of the following lines
	//position after rotating about the axis is calculated as:
	rotated_pos.x = ((1-2*ax.y*ax.y-2*ax.z*ax.z)*p.x) + (2*(ax.x*ax.y+ang*ax.z)*p.y) + (2*(ax.x*ax.z-ang*ax.y)*p.z);
	rotated_pos.y = (2*(ax.x*ax.y-ang*ax.z)*p.x) + ((1-2*ax.x*ax.x-2*ax.z*ax.z)*p.y) + (2*(ax.y*ax.z+ang*ax.x)*p.z);
	rotated_pos.z = (2*(ax.x*ax.z-ang*ax.y)*p.x) + (2*(ax.y*ax.z+ang*ax.x)*p.y) + ((1-2*ax.x*ax.x-2*ax.y*ax.y)*p.z);
	return rotated_pos;
}
void operator<<(ofstream &fout,vect a)
{
	fout<<"("<<a.x<<","<<a.y<<","<<a.z<<")";
}
vect operator-(vect b)
{
	vect temp;
	temp.x=-b.x;
	temp.y=-b.y;
	temp.z=-b.y;
	return temp;
}
long double mag(vect b)
{
	return sqrt((b.x*b.x+b.y*b.y+b.z*b.z));
}
#endif /* VECT_HPP_ */
