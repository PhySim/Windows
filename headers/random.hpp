/*
 * random.hpp
 *
 *  Created on: Jul 15, 2013
 *      Author: Reuben
 */

#include <math.h>
#include <stdlib.h>

#ifndef RANDOM_HPP_
#define RANDOM_HPP_

int random(int a,int b)
{
	int d=b-a;
	return a+rand()%d;
}
long double random(long double a,long double b)
{
	long double d=b-a;
	if(d<0)
		d=-d;

	if(log(d)>7)
		return a+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
//(else
		return a+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));
}
vect random(vect a,vect b)
{
	vect c;

	long double d;
	d=b.x-a.x;
			if(d<0)
				d=-d;

			if(log(d)>7)
				c.x=a.x+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
			else
				c.x=a.x+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));
	d=b.y-a.y;
			if(d<0)
				d=-d;

			if(log(d)>7)
				c.y=a.y+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
			else
				c.y=a.y+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));
	d=b.z-a.z;
			if(d<0)
				d=-d;

			if(log(d)>7)
				c.z=a.z+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
			else
				c.z=a.z+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));

	return c;
}


#endif /* RANDOM_HPP_ */
