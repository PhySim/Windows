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
template<class T=long double>
class vect
{
public:
	T x,y,z;
	vect(T user_x,T user_y,T user_z)
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
	vect operator/(vect b)
	{
		return (vect){x/b.x , y/b.y , z/b.z};
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
	T operator|(vect b)
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
	T mag()
	{
		return sqrt(x*x+y*y+z*z);
	}
	T separation(vect U)
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
		T mag=temp.mag();
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
template<class T>
vect<T> operator*(long double scalar,vect<T> b)	//overloading * operator to allow multiplication of a scalar with a vector
{
       vect<T> temp;
       temp.x=b.x*scalar;
       temp.y=b.y*scalar;
       temp.z=b.z*scalar;
       return temp;
}
template<class T>
void operator<<(ofstream &fout,vect<T> a)	//overloading if << operator to allo displaying of a vector onto a stream
{
	fout<<"("<<a.x<<","<<a.y<<","<<a.z<<")";
}
template<class T>
vect<T> operator-(vect<T> b)	//overloading of - unary operator to allow a vector to reversed
{
	vect<T> temp;
	temp.x=-b.x;
	temp.y=-b.y;
	temp.z=-b.y;
	return temp;
}
#endif /* VECT_HPP_ */
