/*
 * vect.hpp
 *
 *  Created on: Jul 14, 2013
 *      Author: Reuben
 */

#ifndef VECT_HPP_
#define VECT_HPP_

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
		x=-x;
		y=-y;
		z=-z;
		return (vect){x,y,z};
	}
	vect add(vect b)
	{
		vect temp;
		temp.x=x+=b.x;
		temp.y=y+=b.y;
		temp.x=z+=b.z;
		return temp;
	}
	vect subtract(vect b)
	{
		x-=b.x;
		y-=b.y;
		z-=b.z;
		return (vect){x,y,z};
	}
	vect multiply(long double scaler)
	{
		vect temp;
		temp.x=x*=scaler;
		temp.y=y*=scaler;
		temp.z=z*=scaler;
		return temp;
	}
	long double multiply_dot(vect b)
	{
		return x*b.x+y*b.y+z*b.z;
	}
	vect multiply_cross(vect b)
	{
		x=y*b.z-z*b.y;
		y=-(x*b.z-b.x*z);
		z=x*b.y-b.x*y;
		return (vect){x,y,z};
	}
};

vect reverse(vect a)
{
	vect temp;
	temp.x=a.x=-a.x;
	temp.y=a.y=-a.y;
	temp.z=a.z=-a.z;
	return temp;
}
vect add(vect a,vect b)
{
	vect temp;
	temp.x=a.x+=b.x;
	temp.y=a.y+=b.y;
	temp.x=a.z+=b.z;
	return temp;
}
vect subtract(vect a,vect b)
{
	vect temp;
	temp.x=a.x-=b.x;
	temp.y=a.y-=b.y;
	temp.z=a.z-=b.z;
	return temp;
}
vect multiply(vect a,long double scalar)
{
	vect temp;
	temp.x=a.x*=scalar;
	temp.y=a.y*=scalar;
	temp.z=a.z*=scalar;
	return temp;
}
long double multiply_dot(vect a,vect b)
{
	return a.x*b.x+a.y*b.y+a.z*b.z;
}
vect multiply_cross(vect a,vect b)
{
	vect temp;
	temp.x=a.x=a.y*b.z-a.z*b.y;
	temp.y=a.y=-(a.x*b.z-b.x*a.z);
	temp.z=a.z=a.x*b.y-b.x*a.y;
	return temp;
}

#endif /* VECT_HPP_ */
