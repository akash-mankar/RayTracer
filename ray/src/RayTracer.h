#ifndef __RAYTRACER_H__
#define __RAYTRACER_H__

// The main ray tracer.

#include "scene/ray.h"

class Scene;
#define NUM_PHOTONS = 1000
class RayTracer
{
public:
    RayTracer();
    ~RayTracer();

    Vec3d trace( double x, double y );
    void emitPhoton(Vec3d lightPosition,Vec3d direction);

	void tracePhoton(Photon *photon );
    Vec3d traceRay( const ray& r, const Vec3d& thresh, int depth );


	void getBuffer( unsigned char *&buf, int &w, int &h );
	double aspectRatio();
	void traceSetup( int w, int h );
	void tracePixel( int i, int j );
	void emitPhotons();
	bool loadScene( char* fn );

	bool sceneLoaded() { return scene != 0; }
	Vec3d readBackground(unsigned char* buf, float u, float v, int Width, int Height);
	Vec3d getPixelAt( int x, int y, unsigned char* buf, int width, int height ) const;
    void setReady( bool ready )
      { m_bBufferReady = ready; }
    bool isReady() const
      { return m_bBufferReady; }

	const Scene& getScene() { return *scene; }

	void setDepth(int depth) {
		m_nDepth = depth;
	}

private:
	unsigned char *buffer;
	int buffer_width, buffer_height;
	int bufferSize;
	Scene* scene;
	int m_nDepth;
    bool m_bBufferReady;


};

#endif // __RAYTRACER_H__
