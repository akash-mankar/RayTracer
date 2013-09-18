// The main ray tracer.

#pragma warning (disable: 4786)

#include "RayTracer.h"

#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"

#include "parser/Tokenizer.h"
#include "parser/Parser.h"

#include "ui/TraceUI.h"
#include <cmath>
#include <algorithm>

extern TraceUI* traceUI;

#include <iostream>
#include <fstream>

using namespace std;

// Use this variable to decide if you want to print out
// debugging messages.  Gets set in the "trace single ray" mode
// in TraceGLWindow, for example.
bool debugMode = false;

void RayTracer::emitPhoton(Vec3d lightPosition,Vec3d direction)
{
	// Clear out the ray cache in the scene for debugging purposes,
	//scene->intersectCache.clear();

    //make a new photon;

	/**
	 * TODO
	 * Bug#1 the energy of the photon is initially 1. Change that to the light's energy
	 */
    Photon *photon = new Photon();
    photon->bounces =0;
    photon->direction = direction;
    photon->position = lightPosition;
    photon->energy = Vec3d(1.0,1.0,1.0);
    tracePhoton(photon);

}

void RayTracer::tracePhoton(Photon* photon) {

	/**
	 * Possible extensions
	 *  TODO:
	 *  1. Improve photon reflection, absorption rules, which intensities get surpressed which don't so on so forth.
	 *  2. Implement shadow photon
	 *  3. Implement refraction in the photons
	 *  4. Change the algorithm to use russian roullete.
	 *
	 *  Distant Dream:
	 *  TODO:
	 *  1. Implement KD tree for the photons, to find them so on so forth.
	 *  2. Find a way to parallelize the emit photon algorithm.
	 *  3. Implement russian roullete scatter. Get better BRDF
	 */
	isect i;
	ray r( photon->position, photon->direction, ray::PHOTON_EMIT );
	if(photon->bounces>3)
		return;

	if( scene->intersect( r, i ) ) {
		const Material &m = i.getMaterial();
		for(int j=0;j<3;j++)
		photon->energy[j]=min(photon->energy[j],m.kd(i)[j]);
		photon->position = r.at(i.t);
		(&m)->addPhoton(photon);

		//reflect the photon
		Vec3d Cosi = i.N*(-1*r.getDirection()*i.N);
		Vec3d Sini = Cosi+r.getDirection();
		Vec3d reflectedRay = Cosi+Sini;
		reflectedRay.normalize();
		Vec3d reflectPosition = r.at(i.t);

		Photon *refPhoton = new Photon();
		refPhoton->position = reflectPosition;
		refPhoton->direction = reflectedRay;
		refPhoton->energy = photon->energy/(sqrt(photon->bounces+1));
		refPhoton->bounces = photon->bounces+1;
		tracePhoton(refPhoton);



		//Take care of Shadow photon and refracted photon

	}

}




// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
Vec3d RayTracer::trace( double x, double y )
{
	// Clear out the ray cache in the scene for debugging purposes,
	//scene->intersectCache.clear();

    ray r( Vec3d(0,0,0), Vec3d(0,0,0), ray::VISIBILITY );

    scene->getCamera().rayThrough( x,y,r );
	Vec3d ret = traceRay( r, Vec3d(1.0,1.0,1.0), 0 );
	ret.clamp();

	return ret;
}
bool gatedSqDist3(Vec3d a, Vec3d b, float sqradius,float &gSqDist){ //Gated Squared Distance
	  float c = a[0] - b[0];          //Efficient When Determining if Thousands of Points
	  float d = c*c;                  //Are Within a Radius of a Point (and Most Are Not!)

	  if (d > sqradius) return false; //Gate 1 - If this dimension alone is larger than
	  c = a[1] - b[1];                //         the search radius, no need to continue
	  d += c*c;

	  if (d > sqradius) return false; //Gate 2
	  c = a[2] - b[2];
	  d += c*c;

	  if (d > sqradius) return false; //Gate 3

	  gSqDist = d;
	  return true ; //Store Squared Distance Itself in Global State
 }


 Vec3d colorPhoton(vector <Photon> &photons,Vec3d location,const isect &i) {
	 float sqRadius=0.8;
	 float exposure=50.0;
	 Vec3d normal = i.N;
	 Vec3d energy = Vec3d(0.0,0.0,0.0);
	 for(int j=0;j<photons.size();j++) {
		 float sqDist;
		 if (gatedSqDist3(location,photons[j].position,sqRadius,sqDist)){

			 float weight = max(0.0, -1*(normal*photons[j].direction));   //Single Photon Diffuse Lighting
			 weight *= (1.0 - sqrt(sqDist)) / exposure;                    //Weight by Photon-Point Distance

			 energy +=  photons[j].energy*weight;

		 }

	 }
	 return energy;
 }


// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
inline Vec3d RayTracer::traceRay( const ray& r, const Vec3d& thresh, int depth)
{
	if(depth >=traceUI->getDepth())
		return Vec3d(0.0,0.0,0.0);

	isect i;

	if( scene->intersect( r, i ) ) {

//#define PHOTON_SHADING
#ifdef PHOTON_SHADING
		const Material& m = i.getMaterial();

		Vec3d location = r.at(i.t);
		return colorPhoton(m.photons,location,i);
#else
		const Material& m = i.getMaterial();
		Vec3d Cosi = i.N*(-1*r.getDirection()*i.N);
		Vec3d Sini = Cosi+r.getDirection();
		Vec3d reflectedRay = Cosi+Sini; 
		reflectedRay.normalize();
		Vec3d reflectPosition = r.at(i.t);
		//Vec3d reflectedIntensity(0.0,0.0,0.0);


		ray refR (reflectPosition,reflectedRay,ray::REFLECTION);
		Vec3d reflectedIntensity(0.0,0.0,0.0);

		 if(m.kr(i).length()!=0.0) {
			 reflectedIntensity = traceRay(refR,thresh,depth+1);
		 }

		bool isIncoming = (i.N*r.getDirection())>0?true:false;
		
		double upIndex,ucIndex; 
		Vec3d normal = i.N; 
		if(!isIncoming) { 
			upIndex = 1.0;  
			ucIndex = m.index(i); 
		}
		else { 
			upIndex = m.index(i); 
			ucIndex = 1.0; 
			normal = -1*normal;
		}
 		
		//do the snell's refraction 
		Vec3d Sint = (upIndex/ucIndex)*Sini; 
		Vec3d refractedIntensity(0.0,0.0,0.0);
		if(Sint.length()<=1.0 && m.kt(i).length()!=0.0) {
		Vec3d Cost = -1 * normal * sqrt(1-(Sint*Sint));
		Vec3d refraction = Sint + Cost; 
		ray refT(reflectPosition,refraction,ray::REFRACTION);
		refractedIntensity = traceRay(refT,thresh,depth+1);
		}

		return m.shade(scene, r, i)+ m.kr(i)%reflectedIntensity + m.kt(i)%refractedIntensity;
#endif
	
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.
        // For cubic mapping, the convention will be the following:
        /*
         *                              ---------
         *                              |               |
         *                              |top    |
         *                              |+z,+x  |
         *     ----------------------------------
         *     |                |               |               |               |
         *     |left    |front  |right  |back   |
         *     |+y,-z   |+y,+x  |+y,+z  |+y,-x  |
         *     ----------------------------------
         *                      |               |
         *                      |bottom |
         *                      |-z,+x  |
         *                      ---------
         */

		if(traceUI->getUseCubemap()) {
		      Vec3d backColor = Vec3d( 1.0, 0.0, 0.0 );  // default to red for debugging
		      Vec3d rayDir    = r.getDirection();  // 3d vector of ray direction
		      rayDir.normalize();
		      float u,v = 0;  // parametric coordinates on texture face

			  // Determine the face of the cubic mapping needed based on the magnitude
			  // of the ray direction that is traveling towards infinity.
			  // If the ray direction has greatest magnitude along the x-axis
			  if (fabs(rayDir[0]) >= fabs(rayDir[1]) && fabs(rayDir[0]) >= fabs(rayDir[2])) {
					  if (rayDir[0] > 0.0f){  // traveling on +x axis (right)
							  u = 1.0f - (rayDir[2] / rayDir[0] + 1.0f) * 0.5f;  // eq. +z axis
							  v = (rayDir[1] / rayDir[0] + 1.0f) * 0.5f;  // eq. +y axis
							  // Read the corresponding texture from the cubic mapping of the background image
							  backColor = readBackground(traceUI->getBackBufPosX(), u, v, traceUI->getBackWidthPosX(), traceUI->getBackHeightPosX());
					  }
					  else if (rayDir[0] < 0.0f){ // traveling on -x axis (left)
							  u = 1.0f - (rayDir[2] / rayDir[0] + 1.0f) * 0.5f;
							  v = 1.0f - (rayDir[1] / rayDir[0] + 1.0f) * 0.5f;
							  backColor = readBackground(traceUI->getBackBufNegX(), u, v, traceUI->getBackWidthNegX(), traceUI->getBackHeightNegX());
					  }
			  } else if ((fabs(rayDir[1]) >= fabs(rayDir[0])) && (fabs(rayDir[1]) >= fabs(rayDir[2]))){  // The ray direction has greatest magnitude along the y-axis
					  if (rayDir[1] > 0.0f) {  // traveling on the +y axis (top)
							  u = (rayDir[0] / rayDir[1] + 1.0f) * 0.5f;
							  v = 1.0f - (rayDir[2]/ rayDir[1] + 1.0f) * 0.5f;
							  backColor = readBackground(traceUI->getBackBufPosY(), u, v, traceUI->getBackWidthPosY(), traceUI->getBackHeightPosY());
					  }
					  else if (rayDir[1] < 0.0f) { // traveling on the -y axis (bottom)
							  u = 1.0f - (rayDir[0] / rayDir[1] + 1.0f) * 0.5f;
							  v = (rayDir[2]/rayDir[1] + 1.0f) * 0.5f;
							  backColor = readBackground(traceUI->getBackBufNegY(), u, v, traceUI->getBackWidthNegY(), traceUI->getBackHeightNegY());
					  }
			  } else if ((fabs(rayDir[2]) >= fabs(rayDir[0])) && (fabs(rayDir[2]) >= fabs(rayDir[1]))){ // The ray direction has greatest magnitude along the z-axis
					  if (rayDir[2] > 0.0f) {  // traveling on the +z axis (front)
							  u = (rayDir[0] / rayDir[2] + 1.0f) * 0.5f;
							  v = (rayDir[1]/rayDir[2] + 1.0f) * 0.5f;
							  backColor = readBackground(traceUI->getBackBufPosZ(), u, v, traceUI->getBackWidthPosZ(), traceUI->getBackHeightPosZ());
					  }
					  else if (rayDir[2] < 0.0f) { // traveling on the -z axis (back)
							  u = (rayDir[0] / rayDir[2] + 1.0f) * 0.5f;
							  v = 1.0f - (rayDir[1] /rayDir[2]+1) * 0.5f;
							  backColor = readBackground(traceUI->getBackBufNegZ(), u, v, traceUI->getBackWidthNegZ(), traceUI->getBackHeightNegZ());
					  }
			  }
			  return backColor;
		}
		else
		return Vec3d( 0.0, 0.0, 0.0 );
	}
}
Vec3d RayTracer::readBackground(unsigned char* buf, float u, float v, int W, int H){
	u = fabsf(u);
	v = fabsf(v);
	double px = u * W;
	double py = v * H;
	int lx = (int)px;
	int gx = lx + 1;
	int ly = (int)py;
	int gy = ly + 1;
	// Billinear interpolation same as Texture map
	Vec3d tmp1 = getPixelAt(lx, ly, buf, W, H)*(gx - px) + getPixelAt(gx,ly, buf, W, H)*(px - lx);
	Vec3d tmp2 = getPixelAt(lx, gy, buf, W, H)*(gx - px) + getPixelAt(gx,gy, buf, W, H)*(px - lx);
	return tmp1*(gy - py) + tmp2*(py - ly);
}

Vec3d RayTracer::getPixelAt( int x, int y, unsigned char* buf, int width, int height) const
{

	if( x >= width )
		x = width - 1;
	if( y >= height )
		y = height - 1;

	// Find the position in the big data array...
	int pos = (y * width + x) * 3;

	return Vec3d( double(buf[pos]) / 255.0,
			double(buf[pos+1]) / 255.0,
			double(buf[pos+2]) / 255.0 );
}


RayTracer::RayTracer()
	: scene( 0 ), buffer( 0 ), buffer_width( 256 ), buffer_height( 256 ), m_bBufferReady( false )
{

}


RayTracer::~RayTracer()
{
	delete scene;
	delete [] buffer;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return sceneLoaded() ? scene->getCamera().getAspectRatio() : 1;
}

bool RayTracer::loadScene( char* fn )
{
	ifstream ifs( fn );
	if( !ifs ) {
		string msg( "Error: couldn't read scene file " );
		msg.append( fn );
		traceUI->alert( msg );
		return false;
	}
	
	// Strip off filename, leaving only the path:
	string path( fn );
	if( path.find_last_of( "\\/" ) == string::npos )
		path = ".";
	else
		path = path.substr(0, path.find_last_of( "\\/" ));

	// Call this with 'true' for debug output from the tokenizer
	Tokenizer tokenizer( ifs, false );
    Parser parser( tokenizer, path );
	try {
		delete scene;
		scene = 0;
		scene = parser.parseScene();
	} 
	catch( SyntaxErrorException& pe ) {
		traceUI->alert( pe.formattedMessage() );
		return false;
	}
	catch( ParserException& pe ) {
		string msg( "Parser: fatal exception " );
		msg.append( pe.message() );
		traceUI->alert( msg );
		return false;
	}
	catch( TextureMapException e ) {
		string msg( "Texture mapping exception: " );
		msg.append( e.message() );
		traceUI->alert( msg );
		return false;
	}

	scene->setTraceUI(traceUI);
	scene->setTreeHeight(traceUI->getTreeHeight());
	scene->setLeafSize(traceUI->getLeafSize());
	scene->initTree();

	if( ! sceneLoaded() )
		return false;

	
	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];


	}
	memset( buffer, 0, w*h*3 );
	m_bBufferReady = true;
}
void RayTracer::emitPhotons() {
	/*
	 * TODO
	 * Improve the way You are emitting photons.
	 * You are rotating 360 degrees in a spehircal manner and then sending out photons. There should be definitley something better.
	 * You want to send photons only in the direction of the light. Specially in the direction of light
	 *
	 *
	 */

#ifdef PHOTON_MAPPING
	if( ! sceneLoaded() )
			return;

	vector<Light*>::const_iterator lightit = scene->beginLights();

	int num_photons = 1000;
	for(;lightit!=scene->endLights();lightit++) {
		//generate 1000 photons per light source
		Light *plight = *lightit;
		Vec3d position = plight->getPosition();
		//for(int n_ph =0 ;n_ph<num_photons;n_ph++)
		{
			//randomize the direction
			Vec3d radius;
			for(double theta = 0;theta<=2*M_PI;theta+=0.01)
				for(double phi = 0;phi<=2*M_PI;phi+=0.01) {
					Vec3d dir = Vec3d(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
					dir.normalize();
					emitPhoton(position,dir);
				}

		}
	}
#endif

}
void RayTracer::tracePixel( int i, int j )
{
	Vec3d col;

	if( ! sceneLoaded() )
		return;
/*
	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);


	//col = trace( x,y );
*/

	double num = traceUI->getAntiAlias();
	int inc=0;
	double x = double(i)-num/2;
	double y = double(j)-num/2;
	for(double t = 0;t<num;t++){
		for(double s=0;s<num;s++) {
			double xt,yt;
			xt = (x+(t/num))/double(buffer_width);
			yt = (y+(s/num))/double(buffer_height);
			col+= trace( xt,yt );
			inc++;
		}
	}

	col/=(num*num);

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}


