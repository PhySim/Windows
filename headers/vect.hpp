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

/**
 * class that represents a mathematical vector quantity
 * it also has various overloaded operators that allows for various procedures on it
 */
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
		long double mag=temp.mag();
		if(mag)
			return temp/temp.mag();
		else
			return (vect){0,0,0};
	}
	vect reset()
	{
		x=y=z=0.0;
		return *this;
	}
};
vect operator*(long double scalar,vect b)	//overloading * poerator to allow multiplication of a scalar with a vector
{
       vect temp;
       temp.x=b.x*scalar;
       temp.y=b.y*scalar;
       temp.z=b.z*scalar;
       return temp;
}
void operator<<(ofstream &fout,vect a)	//overloading if << operator to allo displaying of a vector onto a stream
{
	fout<<"("<<a.x<<","<<a.y<<","<<a.z<<")";
}
vect operator-(vect b)	//overloading of - unary operator to allow a vector to reversed
{
	vect temp;
	temp.x=-b.x;
	temp.y=-b.y;
	temp.z=-b.y;
	return temp;
}
long double mag(vect b)	//returns the magintude of a vector that is passed to it
{
	return sqrt((b.x*b.x+b.y*b.y+b.z*b.z));
}
#endif /* VECT_HPP_ */
