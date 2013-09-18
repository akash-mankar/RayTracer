#include "ray.h"
#include "material.h"
#include "light.h"
#include "perlin.h"
#include <cmath>

#include "../fileio/bitmap.h"
#include "../fileio/pngimage.h"

#include "../ui/TraceUI.h"
using namespace std;
extern bool debugMode;
extern TraceUI* traceUI;

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
Vec3d Material::shade( Scene *scene, const ray& r, isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
	// You will need to call both distanceAttenuation() and shadowAttenuation()
	// somewhere in your code in order to compute shadows and light falloff.
	/*if( debugMode )
		std::cout << "Debugging Phong code..." << std::endl;
	 */
	// When you're iterating through the lights,
	// you'll want to use code that looks something
	// like this:
	//
	// for ( vector<Light*>::const_iterator litr = scene->beginLights(); 
	// 		litr != scene->endLights(); 
	// 		++litr )
	// {
	// 		Light* pLight = *litr;
	// 		.
	// 		.
	// 		.
	// }
#ifdef NIKUNJ
	Vec3d shadeVector;  
	shadeVector+=ke(i);  
	vector<Light*>::const_iterator it;
	Vec3d specFactor;
	Vec3d diffuseFactor;
	for(it=scene->beginLights();it!=scene->endLights();it++) { 
		Light* pLight = *it; 
		Vec3d lightColor = pLight->getColor(r.getDirection()); 
		Vec3d lightDirection = pLight->getDirection(r.at(i.t));
		lightDirection.normalize();

		double diffComp = (lightDirection*i.N);
		Vec3d attenuation = pLight->shadowAttenuation(r.at(i.t))*
				pLight->distanceAttenuation(r.at(i.t));
		if(diffComp < 0.0) 
			diffComp = 0.0;
		//diffuse component 
		diffuseFactor+=(diffComp*lightColor%attenuation);

		Vec3d reflection = 2*(lightDirection*i.N)*i.N - lightDirection;
		reflection.normalize();
		Vec3d viewer = r.getDirection(); 
		double reflComp = -1*viewer*reflection;
		if(reflComp < 0.0) 
			reflComp = 0.0;
		else {
			reflComp =pow(reflComp,shininess(i));
		}
		specFactor+=(reflComp*lightColor%attenuation);


	}

	specFactor.clamp(); 
	diffuseFactor.clamp(); 
	for(int j=0;j<=2;j++) {
		shadeVector[j]+=ka(i)[j]*scene->ambient()[j];
		shadeVector[j]+=kd(i)[j]*diffuseFactor[j];
		shadeVector[j]+=ks(i)[j]*specFactor[j];
	}
	return shadeVector;
#endif


	// Final shade color of lights
	Vec3d  vShade;
	Vec3d P;

	P =  r.at(i.t);
	// Emmissivity Component

	  vShade.n[0] = ke(i)[0];
	  vShade.n[1] = ke(i)[1];
	  vShade.n[2] = ke(i)[2];

	// Procedural Textures
    if(traceUI->isProcText())
	{
    	double alpha =  traceUI->getNoiseAlpha();
    	double Beta  =  traceUI->getNoiseSpacing();
		double Perlin  = PerlinNoise3D(P[0],P[1],P[2],alpha,Beta, 5 );
		vShade[0] =  fabs(Perlin); //+Perlin1 +Perlin2);
		vShade[1] =  fabs(Perlin); //+Perlin1 +Perlin2);
		vShade[2] =  fabs(Perlin); //+Perlin1+Perlin2);

	  	if (vShade[0] >1.0f)
		vShade[0] = 1.0f;
		if (vShade[1] >1.0f)
				vShade[1] =1.0f;
		if (vShade[2] >1.0f)
				vShade[2] =1.0f;

	}
	// Ambient Component
	// Ambient already interpolated using alpha,beta and gamma in trimesh.cpp
	// Not interpolated for any other object or surface apart from trimesh
	vShade.n[0] += ka(i)[0]*scene->ambient()[0];
	vShade.n[1] += ka(i)[1]*scene->ambient()[1];
	vShade.n[2] += ka(i)[2]*scene->ambient()[2];

	Vec3d color;


	Vec3d vecBump = getMapValue(i);

	for ( vector<Light*>::const_iterator litr = scene->beginLights();litr != scene->endLights();++litr )
	{

		Light* pLight = *litr;
		Vec3d vDirLight;
		vDirLight = pLight->getDirection(r.at(i.t));

		// Attenuation component
		// To be added to each of the components of specular and Diffuse lights.
		Vec3d vecAtten;
		vecAtten = pLight->shadowAttenuation(r.at(i.t));
		vecAtten = vecAtten*pLight->distanceAttenuation(r.at(i.t));

		// Diffuse Component

		bool bumptrue = isBumpMap();
		double dNL;

		if(bumptrue)
			dNL	 = vecBump  * vDirLight;
		else
			dNL = i.N * vDirLight;

		// When a ray is coming towards the object at an angle = 90 or more than 90 deg.
		// it douesn't count. at 90, the ray is parallel and more than that we don't care
		// refer slides, shading diffuse reflectors.
		if (dNL<0)	// Diffuse term becomes zero when more than 90
			dNL=0;

		vShade[0] += kd(i)[0]*pLight->getColor(r.at(i.t))[0]*dNL*vecAtten[0];
		vShade[1] += kd(i)[1]*pLight->getColor(r.at(i.t))[1]*dNL*vecAtten[1];
		vShade[2] += kd(i)[2]*pLight->getColor(r.at(i.t))[2]*dNL*vecAtten[2];

		// Specular Component
		Vec3d vecR;
		vecR = 2*(i.N*vDirLight)*i.N - vDirLight; // Formula for reflection angle
		vecR.normalize(); // Unit Vector
		// R is going away from the point.
		// While viewing direction is towards the point.
		// Dot product would be cos theta and we should negate the value of the dot product
		// to get the appropriate value for cos theta.

		double dRV = -1*(vecR*r.getDirection());
		if (dRV<0)
			dRV=0;

		dRV=pow(dRV,shininess(i)); // ns -> specular component/shininess

		vShade[0]	+=	ks(i)[0]*pLight->getColor(r.at(i.t))[0]*dRV*vecAtten[0];
		vShade[1]	+=	ks(i)[1]*pLight->getColor(r.at(i.t))[1]*dRV*vecAtten[1];
		vShade[2]	+=	ks(i)[2]*pLight->getColor(r.at(i.t))[2]*dRV*vecAtten[2];

	}
	if(debugMode)
		cout<< "vShade w/Bump =" << vShade << endl;
//	vShade.normalize();
	return vShade;

}


TextureMap::TextureMap( string filename ) {

	int start = filename.find_last_of('.');
	int end = filename.size() - 1;
	if (start >= 0 && start < end) {
		string ext = filename.substr(start, end);
		if (!ext.compare(".png")) {
			png_cleanup(1);
			if (!png_init(filename.c_str(), width, height)) {
				double gamma = 2.2;
				int channels, rowBytes;
				unsigned char* indata = png_get_image(gamma, channels, rowBytes);
				int bufsize = rowBytes * height;
				data = new unsigned char[bufsize];
				for (int j = 0; j < height; j++)
					for (int i = 0; i < rowBytes; i += channels)
						for (int k = 0; k < channels; k++){
							*(data + k + i + j * rowBytes) = *(indata + k + i + (height - j - 1) * rowBytes);
						}
				png_cleanup(1);
			}
		}
		else
			if (!ext.compare(".bmp")) data = readBMP(filename.c_str(), width, height);
			else data = NULL;
	} else data = NULL;
	if (data == NULL) {
		width = 0;
		height = 0;
		string error("Unable to load texture map '");
		error.append(filename);
		error.append("' .");
		throw TextureMapException(error);
	}else {
		normalData = new double[width*height*3];

	}

}

// For use in Normal Map
void TextureMap::calcGradient() {

	for(int y=0;y<height;y++) {
		for(int x=0;x<width;x++) {
			Vec3d upValue  	 =  getPixelAt(x, y+1);
			Vec3d downValue	 =  getPixelAt(x, y-1);
			Vec3d leftValue	 =  getPixelAt(x-1, y);
			Vec3d rightValue =  getPixelAt(x+1, y);

			float up = (upValue[0]+upValue[1]+upValue[2])/3.;
			float down = (downValue[0]+downValue[1]+downValue[2])/3.;
			float left = (leftValue[0]+leftValue[1]+leftValue[2])/3.;
			float right = (rightValue[0]+rightValue[1]+rightValue[2])/3.;
			/*cout << "up= "<< up;
	        cout << " down= "<< down;
	        cout << " left= "<< left;
	        cout << " right=" << right << endl;
*/
			double XGradient = right + left;
			double YGradient = down + up;
			int pos = 3*(y*width + x);
			//  cout << "pos="<< pos <<endl;
			normalData[pos]     = XGradient + XGradient;
			normalData[pos + 1] = YGradient + YGradient;
			normalData[pos + 2] = 1.0;
			//cout<<normalData[pos]<<" "<<normalData[pos+1]<<" "<<normalData[pos+2]<<"*********"<<endl;

		}
	}
	/*
		Value at X,Y is a three vector value
		 int pos = (y * width + x) * 3;
		 so you want to make the value at data[pos] = Xgradient
		 value at pos +1 = YGradient
		 value at pos +2 = 1;

	 */
}

Vec3d TextureMap::getNormalMappedValue( const Vec2d& coord ) const
{
	if(data == NULL){
		cout << "is Data NULL ?" << endl;
		return Vec3d(1.0, 1.0, 1.0);
	}
	else{
		int x= coord[0]*width;
		int y = coord[1]*height;
		int pos = 3*(x+y*width);

		Vec3d color ;
		for(int i=0;i<=2;i++)
		color[i]=normalData[pos+i];

		return color;
	}
}
Vec3d TextureMap::getMappedValue( const Vec2d& coord ) const
{

	// YOUR CODE HERE

	// In order to add texture mapping support to the
	// raytracer, you need to implement this function.
	// What this function should do is convert from
	// parametric space which is the unit square
	// [0, 1] x [0, 1] in 2-space to bitmap coordinates,
	// and use these to perform bilinear interpolation
	// of the values.
	//  cout<<coord[0]<<" "<<coord[1]<<" ";


	// Billd
	if(data == NULL){
		return Vec3d(1.0, 1.0, 1.0);
	}
	else{
		double px = coord[0] * width;
		double py = coord[1] * height;
		int lx = (int)px;
		int gx = lx + 1;
		int ly = (int)py;
		int gy = ly + 1;
		Vec3d tmp1 = getPixelAt(lx, ly)*(gx - px) + getPixelAt(gx,ly)*(px - lx);
		Vec3d tmp2 = getPixelAt(lx, gy)*(gx - px) + getPixelAt(gx,gy)*(px - lx);
		return tmp1*(gy - py) + tmp2*(py - ly);
	}

}


Vec3d TextureMap::getPixelAt( int x, int y ) const
{
	// This keeps it from crashing if it can't load
	// the texture, but the person tries to render anyway.
	if (0 == data)
		return Vec3d(1.0, 1.0, 1.0);

	if( x >= width )
		x = width - 1;
	if( y >= height )
		y = height - 1;

	// Find the position in the big data array...
	int pos = (y * width + x) * 3;

	return Vec3d( double(data[pos]) / 255.0,
			double(data[pos+1]) / 255.0,
			double(data[pos+2]) / 255.0 );
}


Vec3d MaterialParameter::getNormalMapValue( const isect& i ) const
{
	if(_normalMap)
		return _normalMap->getNormalMappedValue( i.uvCoordinates );
	else
	{
		//cout << "GetNormalMapValue::Normal Map is null" << endl;
		return Vec3d(1.0,1.0,1.0);
	}
}
void MaterialParameter::setNormalMapValue()
{
	if(_normalMap)
	{
		_normalMap->calcGradient();
	}
	else
		cout << "SetNormalMapValue ::NormalMap is NULL" <<endl;
}

Vec3d MaterialParameter::value( const isect& is ) const
{

	if( 0 != _textureMap)
		return _textureMap->getMappedValue( is.uvCoordinates );
	else
		return _value;
}

double MaterialParameter::intensityValue( const isect& is ) const
{
	if( 0 != _textureMap )
	{
		Vec3d value( _textureMap->getMappedValue( is.uvCoordinates ) );
		return (0.299 * value[0]) + (0.587 * value[1]) + (0.114 * value[2]);
	}
	else
		return (0.299 * _value[0]) + (0.587 * _value[1]) + (0.114 * _value[2]);
}
