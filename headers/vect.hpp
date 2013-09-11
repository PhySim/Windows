/*
 * vect.hpp
 *
 *  Created on: Jul 14, 2013
 *      Author: Reuben
 */

#ifndef VECT_HPP_
#define VECT_HPP_

#include <math.h>

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
		temp.x=z+b.z;
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
		vect temp;
		temp.x=x+=b.x;
		temp.y=y+=b.y;
		temp.x=z+=b.z;
		return temp;
	}
	vect operator-=(vect b)
	{
		vect temp;
		temp.x=x-=b.x;
		temp.y=y-=b.y;
		temp.z=z-=b.z;
		return temp;
	}
	vect operator*=(vect b)
	{
		vect temp;
		temp.x=x*=b.x;
		temp.y=y*=b.y;
		temp.z=z*=b.z;
		return temp;
	}
	vect operator*=(long double scalar)
	{
		vect temp;
		temp.x=x*=scalar;
		temp.y=y*=scalar;
		temp.z=z*=scalar;
		return temp;
	}
	vect operator/=(long double scalar)
	{
		vect temp;
		temp.x/=scalar;
		temp.y/=scalar;
		temp.z/=scalar;
		return temp;
	}
	long double mag()
	{
		return sqrt(x*x+y*y+z*z);
	}
	long double dist(vect U)
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
};
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
