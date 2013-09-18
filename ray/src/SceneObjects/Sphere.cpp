#include <cmath>

#include "Sphere.h"

using namespace std;


bool Sphere::intersectLocal( const ray& r, isect& i ) const
{
	Vec3d v = -r.getPosition();
	double b = v * r.getDirection();
	double discriminant = b*b - v*v + 1;

	if( discriminant < 0.0 ) {
		return false;
	}

	discriminant = sqrt( discriminant );
	double t2 = b + discriminant;

	if( t2 <= RAY_EPSILON ) {
		return false;
	}

	i.obj = this;

	double t1 = b - discriminant;
	
	if( t1 > RAY_EPSILON ) {
		i.t = t1;
		i.N = r.at( t1 );
		i.N.normalize();
	} else {
		i.t = t2;
		i.N = r.at( t2 );
		i.N.normalize();
	}

	Vec3d P = r.at(i.t);
	//i.setUVCoordinates(Vec2d(0.5-(atan2(P[2],P[0])/(2*3.14)),0.5-(2.0 * asin(P[1])/(2*3.14))));
	//cout<<i.uvCoordinates[0]<<" "<<i.uvCoordinates[1]<<endl;
	double x = P[0];
	double y = P[1];
	double z = P[2];
	double len = sqrt(x * x + y * y + z * z);

	if (len == 0.0)
		return true;
	else
	{
		x /= len;
		y /= len;
		z /= len;
	}

	/* Determine its angle from the x-z plane. */
	double phi = 0.5 + asin(y) / M_PI; /* This will be from 0 to 1 */
	double theta = 0.0;

	/* Determine its angle from the point (1, 0, 0) in the x-z plane. */
	len = x * x + z * z;

	if (len > RAY_EPSILON)
	{
		len = sqrt(len);
		if (z == 0.0)
		{
			if (x > 0)
				theta = 0.0;
			else
				theta = M_PI;
		}
		else
		{
			theta = acos(x / len);
			if (z < 0.0)
				theta = 2*M_PI - theta;
		}

		theta /= 2*M_PI;  /* This will be from 0 to 1 */
	}
	else
		/* This point is at one of the poles. Any value of xcoord will be ok... */
		theta = 0;

	i.setUVCoordinates(Vec2d(theta,phi));
	return true;
}

