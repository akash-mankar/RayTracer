#ifndef _KDTREE_NODE_H_ 
#define _KDTREE_NODE_H_
#include "vecmath/vec.h"
#include "scene/scene.h"
#include "scene/ray.h"
#include "scene/bbox.h"
#include <vector>
#include <iostream>
using namespace std;

class KDTreeNode { 
	public: 
		bool isLeaf(); 	
		vector <Geometry*> getChildren(); 
		void setMyself(BoundingBox* myself) { 
			this->myself = myself; 
		}
		const BoundingBox* getMyself() { 
			return myself; 
		}
	//	bool intersects(const Vec3d& point) const { 
//			return myself->intersects(point); 
//		}
		bool intersect(const ray& r, double& tMin, double& tMax) const { 
			return myself->intersect(r,tMin,tMax); 	
		}
		const BoundingBox* myself; 	
	private: 
		vector <Geometry*> children;  
		vector <BoundingBox*> boxes; 
	
};

class KDTree { 
	public: 
	 	KDTreeNode* getRoot(); 
		void setScene(Scene* scene);
	private:
		vector <Geometry*> totalObjects; 
		Scene* scene; 
 
};
#endif
