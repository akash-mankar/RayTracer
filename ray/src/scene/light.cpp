#include <cmath>

#include "light.h"



using namespace std;

double DirectionalLight::distanceAttenuation( const Vec3d& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


Vec3d DirectionalLight::shadowAttenuation( const Vec3d& P ) const
{
	//return Vec3d(1.0,1.0,1.0);
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	//return Vec3d(1,1,1);
	Vec3d towardsLight = getDirection(P);  
	ray r(P,towardsLight,ray::SHADOW);
	isect i;
	if(scene->intersect(r,i)) {
			if(i.t < RAY_EPSILON) {
					return Vec3d(1,1,1);
			}
			const Material&m = i.getMaterial(); 
			return m.kt(i);
	}
    return Vec3d(1,1,1);

}

Vec3d DirectionalLight::getColor( const Vec3d& P ) const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d DirectionalLight::getDirection( const Vec3d& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const Vec3d& P ) const
{

	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, we assume no attenuation and just return 1.0

	Vec3d dir  = position-P;
	double distance = 0.0;
	distance = dir.length(); 
	
	return min(1.0,1.0/(constantTerm+linearTerm*distance+
						quadraticTerm*pow(distance,2)));

}

Vec3d PointLight::getColor( const Vec3d& P ) const
{
	// Color doesn't depend on P 
	return color;
}

Vec3d PointLight::getDirection( const Vec3d& P ) const
{
	Vec3d ret = position - P;
	ret.normalize();
	return ret;
}


Vec3d PointLight::shadowAttenuation(const Vec3d& P) const
{
	//return Vec3d(1.0,1.0,1.0);
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
//	return Vec3d(1,1,1);	
	Vec3d towardsLight = getDirection(P);  
	ray r(P+RAY_EPSILON*towardsLight,towardsLight,ray::SHADOW);
	isect i;
	if(scene->intersect(r,i)) {

			if(i.N*towardsLight<0.0) { 
				return Vec3d(1,1,1);
			}
 			if(i.t < RAY_EPSILON) {
					return Vec3d(1,1,1);
			}
 			const Material&m = i.getMaterial(); 
			return m.kt(i);  
	}


    return Vec3d(1,1,1);

}
