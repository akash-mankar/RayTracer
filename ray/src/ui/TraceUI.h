//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

// who the hell cares if my identifiers are longer than 255 characters:
#pragma warning(disable : 4786)

#include <math.h>
#include "../vecmath/vec.h"
#include "../vecmath/mat.h"

#include <string>

using std::string;

class RayTracer;

class TraceUI {
public:
	TraceUI()
		: m_nDepth(1), m_nSize(150),treeHeight(10),leafSize(10),m_smoothShade(true),m_procText(false),m_antiAlias(1),m_noiseAlpha(2), m_noiseSpacing(2),divStrategy(2),
		m_displayDebuggingInfo( false ), m_backWidthPosX(0), m_backHeightPosX(0), m_backWidthPosY(0), m_backHeightPosY(0),
		m_backWidthPosZ(0), m_backHeightPosZ(0),m_backWidthNegX(0), m_backHeightNegX(0), m_backWidthNegY(0), m_backHeightNegY(0),
		m_backWidthNegZ(0), m_backHeightNegZ(0),raytracer( 0 )
	{ }

	virtual int		run() = 0;

	// Send an alert to the user in some manner
	virtual void		alert(const string& msg) = 0;

	// setters
	virtual void		setRayTracer( RayTracer* r )
		{ raytracer = r; }

	// accessors:
	int		getSize() const { return m_nSize; }
	int		getDepth() const { return m_nDepth; }
    void 	setUseCubemap( int val){ m_useCubemap |= val; }
    void 	setBackBufPosX(unsigned char* data)	{ m_backBufPosX = data; }
    void 	setBackWidthPosX(int width){ m_backWidthPosX = width; }
    void 	setBackHeightPosX(int height){ m_backHeightPosX = height; }
    void 	setBackBufNegX(unsigned char* data)	{ m_backBufNegX = data; }
    void 	setBackWidthNegX(int width){ m_backWidthNegX = width; }
    void 	setBackHeightNegX(int height){ m_backHeightNegX = height; }
    void 	setBackBufPosY(unsigned char* data)	{ m_backBufPosY = data; }
    void 	setBackWidthPosY(int width){ m_backWidthPosY = width; }
    void 	setBackHeightPosY(int height){ m_backHeightPosY = height; }
    void 	setBackBufNegY(unsigned char* data)	{ m_backBufNegY = data; }
    void 	setBackWidthNegY(int width){ m_backWidthNegY = width; }
    void 	setBackHeightNegY(int height){ m_backHeightNegY = height; }
    void 	setBackBufPosZ(unsigned char* data)	{ m_backBufPosZ = data; }
    void 	setBackWidthPosZ(int width){ m_backWidthPosZ = width; }
    void 	setBackHeightPosZ(int height){ m_backHeightPosZ = height; }
    void 	setBackBufNegZ(unsigned char* data)	{ m_backBufNegZ = data; }
    void 	setBackWidthNegZ(int width){ m_backWidthNegZ = width; }
    void 	setBackHeightNegZ(int height){ m_backHeightNegZ = height; }

	void 	setTreeHeight(int height) { treeHeight = height;}
	int 	getTreeHeight() { return treeHeight;}
	int 	getLeafSize() { return leafSize;}
	int 	getAntiAlias() { return m_antiAlias;}

    bool 	getUseCubemap(){ return (m_useCubemap&0x3F); }
	unsigned char*  getBackBufPosX() const { return m_backBufPosX; }
	int             getBackWidthPosX() const { return m_backWidthPosX; }
	int             getBackHeightPosX() const { return m_backHeightPosX; }
	unsigned char*  getBackBufNegX() const { return m_backBufNegX; }
	int             getBackWidthNegX() const { return m_backWidthNegX; }
	int             getBackHeightNegX() const { return m_backHeightNegX; }


	unsigned char*  getBackBufPosY() const { return m_backBufPosY; }
	int             getBackWidthPosY() const { return m_backWidthPosY; }
	int             getBackHeightPosY() const { return m_backHeightPosY; }
	unsigned char*  getBackBufNegY() const { return m_backBufNegY; }
	int             getBackWidthNegY() const { return m_backWidthNegY; }
	int             getBackHeightNegY() const { return m_backHeightNegY; }

	unsigned char*  getBackBufPosZ() const { return m_backBufPosZ; }
	int             getBackWidthPosZ() const { return m_backWidthPosZ; }
	int             getBackHeightPosZ() const { return m_backHeightPosZ; }
	unsigned char*  getBackBufNegZ() const { return m_backBufNegZ; }
	int             getBackWidthNegZ() const { return m_backWidthNegZ; }
	int             getBackHeightNegZ() const { return m_backHeightNegZ; }
	float 			getNoiseAlpha() const {return m_noiseAlpha;}
	float 			getNoiseSpacing() const {return m_noiseSpacing;}

	bool isSmoothShade() {return m_smoothShade;}
	bool isProcText() {return m_procText;}

	int getDivStrategy() {
		return divStrategy;
	}

protected:
	RayTracer*	raytracer;
	unsigned char* m_backBufPosX;                       // Buffer for the background image
	unsigned char* m_backBufPosY;                       // Buffer for the background image
	unsigned char* m_backBufPosZ;                       // Buffer for the background image
	unsigned char* m_backBufNegX;                       // Buffer for the background image
	unsigned char* m_backBufNegY;                       // Buffer for the background image
	unsigned char* m_backBufNegZ;                       // Buffer for the background image


	int			m_nSize;				// Size of the traced image
	int			m_nDepth;				// Max depth of recursion
	int 		treeHeight;
	int 		leafSize;
	bool 		m_smoothShade;
	bool 		m_procText;
	float 		m_noiseAlpha;
	float 		m_noiseSpacing;
	int			m_antiAlias;
	int 		divStrategy;

	// Determines whether or not to show debugging information
	// for individual rays.  Disabled by default for efficiency
	// reasons.
	bool		m_displayDebuggingInfo;
    bool        m_useCubemap;                // Flagged true if there is a background image loaded
    int 		m_backWidthPosX;
    int 		m_backHeightPosX;
    int 		m_backWidthPosY;
    int 		m_backHeightPosY;
    int 		m_backWidthPosZ;
    int		    m_backHeightPosZ;
    int 		m_backWidthNegX;
    int 		m_backHeightNegX;
    int 		m_backWidthNegY;
    int 		m_backHeightNegY;
    int		    m_backWidthNegZ;
    int 		m_backHeightNegZ;

};

#endif
