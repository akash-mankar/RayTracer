#include <cmath>
#include <float.h>
#include "trimesh.h"
#include <set>

using namespace std;
extern bool debugMode;
int strategy = 2;

Vec3d clampAxis(Vec3d v,int axis) {
	for(int i=0;i<3;i++)
		if(i!=axis)
			v[i]=0.0;
	return v;
}
double cost(Vec3d dividPlane,BoundingBox localbounds,vector <TrimeshFace*> faces,int axis) {

	Vec3d leftCorner = localbounds.getMax();
	Vec3d rightCorner = localbounds.getMin();
	leftCorner[axis]=dividPlane[axis];
	rightCorner[axis]=dividPlane[axis];


	BoundingBox *leftBound = new BoundingBox(localbounds.getMin(),leftCorner);
	BoundingBox *rightBound = new BoundingBox(rightCorner,localbounds.getMax());

	int leftCount =0,rightCount=0;
	for(int j=0;j<faces.size();j++) {
		if(leftBound->intersects(faces[j]->localbounds)) {
			leftCount++;
		}
		if(rightBound->intersects(faces[j]->localbounds)) {
					rightCount++;
		}
	}

	if(leftCount ==0 || rightCount ==0) {
		return localbounds.volume()*faces.size();
	}
	//SAH
	return leftBound->volume()*leftCount+rightBound->volume()*rightCount;


}

Vec3d choose_plane(BoundingBox localBounds,vector <TrimeshFace*> faces,int& axis,int strategy,bool &isLeaf) {
	/* 
	multiple ways to choose a plane. call a cost function 
	 */

	/*
	 * strategy number 1. Divide between the lower and the upper corner. Pick the dimension(axis) which is largest.
	   Do something dumb
	  just choose the maximum axis and divide it into two 
	 */
	if(strategy == 1) {
		Vec3d len = localBounds.getMax() - localBounds.getMin();
		if(len[0]>len[1])  {
			axis = (len[0]>len[2])?0:2;
		}
		else {
			axis = (len[1]>len[2])?1:2;
		}
		Vec3d divPlane = localBounds.getMin()+(len*0.5);
		for(int i=0;i<3;i++)
			if(i!=axis)
				divPlane[i]=0.0;

		return divPlane;
	}else {
		int spaces=40;
		Vec3d incV = (localBounds.getMax()-localBounds.getMin())/spaces;
		double bestCost=localBounds.volume()*faces.size();
		Vec3d dividPlane;
		isLeaf = true;
		for(int i=0;i<3;i++) {
			for(int j=1;j<spaces;j++) {
				Vec3d plane = clampAxis(localBounds.getMin()+j*incV,i);
				double localCost = cost(plane,localBounds,faces,i);
				if(localCost < bestCost) {
					axis = i;
					bestCost = localCost;
					dividPlane = plane;
					isLeaf = false;
				}
			}
		}
		return dividPlane;

	}

}

void make_tree(KDTreeNode* node,int MAX_DEPTH,int MIN_LEAF_SIZE) {
	node->left = NULL;
	node->right = NULL;

	vector <TrimeshFace*> faces = node->faces;
	BoundingBox localbounds = *(node->mybounds);

	if(node->depth >=MAX_DEPTH || node->faces.size()<=MIN_LEAF_SIZE) {
		node->isLeaf = true;
		return;
	}

	int bestAxis;
	bool isLeaf;
	Vec3d dividPlane = choose_plane(localbounds,faces,bestAxis,strategy,isLeaf);

	if(isLeaf) {
		node->isLeaf = true;
		return;
	}

	Vec3d leftCorner = localbounds.getMax();
	Vec3d rightCorner = localbounds.getMin();
	leftCorner[bestAxis]=dividPlane[bestAxis];
	rightCorner[bestAxis]=dividPlane[bestAxis];


	BoundingBox *leftBound = new BoundingBox(localbounds.getMin(),leftCorner);
	BoundingBox *rightBound = new BoundingBox(rightCorner,localbounds.getMax());


	node->left = new KDTreeNode();
	node->left->mybounds = leftBound;
	node->left->depth = node->depth+1;
	node->right = new KDTreeNode();
	node->right->mybounds = rightBound;
	node->right->depth = node->depth+1;
	for(int i=0;i<faces.size();i++) {
		bool isecLeft = node->left->mybounds->intersects(faces[i]->localbounds);
		bool isecRight = node->right->mybounds->intersects(faces[i]->localbounds);

		if(isecLeft && isecRight) {
			node->left->addFace(faces[i]);
			node->right->addFace(faces[i]);
		}
		else {
			if(isecLeft)
				node->left->addFace(faces[i]);
			else if(isecRight)
				node->right->addFace(faces[i]);
		}

		}
		make_tree(node->left,MAX_DEPTH,MIN_LEAF_SIZE);
		make_tree(node->right,MAX_DEPTH,MIN_LEAF_SIZE);

}

inline bool intersectLeaf(KDTreeNode* node,const ray&r,isect& i,map<TrimeshFace*,int> &faceMap) {
	double tmin,tmax;
	bool have_one = false;
	for(int j=0;j<node->faces.size();j++) {
		isect cur;
		//if(!node->faces[j]->localbounds.intersect(r,tmin,tmax)) {
		//	return false;
		//}
	//	if(faceMap[node->faces[j]]==NULL)
		if(node->faces[j]->localbounds.intersect(r,tmin,tmax) && node->faces[j]->intersectLocal( r, cur ))
		{
		//	faceMap[node->faces[j]]=1;

			if( !have_one || (cur.t < i.t) )
			{
				i = cur;
				have_one = true;
			}
		}
	}
	if( !have_one ) i.setT(1000.0);
	return have_one;
}

inline bool intersectTree(KDTreeNode* node, const ray&r, isect&i,map<TrimeshFace*,int> faceMap) {
	double tmin,tmax;
	if(node->mybounds->intersect(r,tmin,tmax)) {
		if(node->isLeaf) {
			return intersectLeaf(node,r,i,faceMap);
		}
		else {
			isect ileft,iRight;

			double tleftMin,tleftMax,trightMin,tRightMax;

			bool isLeft = node->left->mybounds->intersect(r,tleftMin,tleftMax);
			bool isRight = node->right->mybounds->intersect(r,trightMin,tRightMax);
			//cout<<tleftMin<<" "<<tleftMax<<endl;
			//cout<<trightMin<<endl;
			if(isLeft && isRight) {
				if(tleftMin < trightMin) {
					bool isecLeft = intersectTree(node->left,r,i,faceMap);
					if(isecLeft) {
						return true;
					}
					else {
						return intersectTree(node->right,r,i,faceMap);
					}

				}
				else {
					//right left
					bool isecRight = intersectTree(node->right,r,i,faceMap);
					if(isecRight) {
						return true;
					}
					else {
						return intersectTree(node->left,r,i,faceMap);
					}
				}
			}
			else if(isLeft) {
				//left
				return intersectTree(node->left,r,i,faceMap);
			}
			else if(isRight) {
				//right
				return intersectTree(node->right,r,i,faceMap);
			}
			else {
				i.setT(1000.0);
				return false;
			}

		}

	}
	i.setT(1000.0);
	return false;

}

void Trimesh::initTree() {
	strategy = traceUI->getDivStrategy();
	root = new KDTreeNode();  
	this->ComputeLocalBoundingBox();
	root->mybounds = new BoundingBox(localBounds.getMin(),localBounds.getMax());
	for(int i=0;i<faces.size();i++) { 
		faces[i]->ComputeLocalBoundingBox();
		root->addFace(faces[i]); 
	}
	root->depth=0;
	make_tree(root,treeHeight,leafSize);

}



Trimesh::~Trimesh()
{
	for( Materials::iterator i = materials.begin(); i != materials.end(); ++i )
		delete *i;
}

// must add vertices, normals, and materials IN ORDER
void Trimesh::addVertex( const Vec3d &v )
{
	vertices.push_back( v );
}

void Trimesh::addMaterial( Material *m )
{
	materials.push_back( m );
}

void Trimesh::addNormal( const Vec3d &n )
{
	normals.push_back( n );
}

// Returns false if the vertices a,b,c don't all exist
bool Trimesh::addFace( int a, int b, int c )
{
	int vcnt = vertices.size();

	if( a >= vcnt || b >= vcnt || c >= vcnt ) return false;

	TrimeshFace *newFace = new TrimeshFace( scene, new Material(*this->material), this, a, b, c );
	newFace->setTransform(this->transform);
	faces.push_back( newFace );
	return true;
}





char *
Trimesh::doubleCheck()
// Check to make sure that if we have per-vertex materials or normals
// they are the right number.
{
	if( !materials.empty() && materials.size() != vertices.size() )
		return "Bad Trimesh: Wrong number of materials.";
	if( !normals.empty() && normals.size() != vertices.size() )
		return "Bad Trimesh: Wrong number of normals.";

	return 0;
}
void Trimesh::printTrimesh() {
	cout<<ComputeLocalBoundingBox().getMin()<<" "<<ComputeLocalBoundingBox().getMax()<<endl;
	typedef Faces::const_iterator iter;
	for(iter j=faces.begin();j!=faces.end();j++) {
		cout<<(*j)->localbounds.getMin()<<" "<<(*j)->localbounds.getMax()<<endl;
	}
	cout<<"END"<<endl;
}

bool Trimesh::intersectLocal(const ray&r, isect&i) const
{

	typedef Faces::const_iterator iter;
	map<TrimeshFace*,int> faceMap;
	/*
	for( iter j = faces.begin(); j != faces.end(); ++j ) {
		faceMap[*j]=0;
	}*/


	return intersectTree(root,r,i,faceMap);


	double tmin = 0.0;
	double tmax = 0.0;

	bool have_one = false;

	for( iter j = faces.begin(); j != faces.end(); ++j ) {
		isect cur;
		if((*j)->intersectLocal( r, cur ) )
		{
			if( !have_one || (cur.t < i.t) )
			{
				i = cur;
				have_one = true;
			}
		}
	}

	if( !have_one ) i.setT(1000.0);
	return have_one;
}

// Intersect ray r with the triangle abc.  If it hits returns true,
// and puts the t parameter, barycentric coordinates, normal, object id,
// and object material in the isect object
bool TrimeshFace::intersectLocal( const ray& r, isect& i ) const
{
#ifdef MULLER
	const Vec3d& a = parent->vertices[ids[0]];
	const Vec3d& b = parent->vertices[ids[1]];
	const Vec3d& c = parent->vertices[ids[2]];

	const Vec3d& dir = r.getDirection();

	Vec3d pvec = r.getDirection() ^ edge2;
	double det = edge1*pvec;



	if(det > -1*RAY_EPSILON && det < RAY_EPSILON) 
		return false; 

	double invDet = 1.0/det; 
	Vec3d tvec = r.getPosition()-a; 
	double u = (tvec*pvec)*invDet; 
	if(u<0.0 || u>1.0) 
		return false;

	Vec3d qvec = tvec^edge1; 
	double v = (dir*qvec)*invDet; 
	if(v<0.0 || u+v>1.0) 
		return false; 

	double t = (edge2*qvec)*invDet;  



	i.t = t;
	i.N = normal;
	if(parent->traceUI->isSmoothShade() && parent->vertNorms) {
		Vec3d Normal = (1-u-v)*parent->normals[ids[0]] + u*(parent->normals[ids[1]])+v*(parent->normals[ids[2]]);
		Normal.normalize();
		i.N = Normal;
	}

	if(parent->materials.size()!=0) {

		Material* a = parent->materials[ids[0]];
		Material* b = parent->materials[ids[1]];
		Material* c = parent->materials[ids[2]];
		Material d;
		d+=(1-u-v)*(*a);
		d+=u*(*b);
		d+=v*(*c);
		i.setMaterial(d);
	}
	else
		i.setMaterial(getMaterial());
	i.setObject(this);

	Vec3d P = r.at(i.t);
	if(debugMode) {
		cout<<a<<" "<<b<<" "<<c<<endl;
	}
	return true;
#else
	    const Vec3d& a = parent->vertices[ids[0]];
	    const Vec3d& b = parent->vertices[ids[1]];
	    const Vec3d& c = parent->vertices[ids[2]];

	    // YOUR CODE HERE
	    Vec3d p = r.getPosition();
	    Vec3d d = r.getDirection();
	    Vec3d N;

	 // Normal Direction
	    double tmp= d*normal; // cos theta
	    if(tmp == 0){
	         return false;
	    }
	    else if(tmp > 0){
	      N = - normal;
	    }
	    else{
	     N = normal;
	    }

	    double t = (N * a - N * p) / (N * d);

	    if(t < RAY_EPSILON)
	        return false;

	    Vec3d m = r.at(t);

	    int k = -1;
	    //int k = 2;
	    double maxcomponent = -1;
		for(int index = 0; index < 3; index++){
				if(abs(N[index]) > maxcomponent){
						maxcomponent = abs(N[index]);
						k = index;
				}
		}

		double divisor = ((b - a) ^ (c - a))[k];
		if(divisor == 0.0){
				return false;
		}
	//	cout << "divisor = "<< divisor << endl;
		double u = ((m - a) ^ (c - a))[k] / divisor;
		double v = ((b - a) ^ (m - a))[k] / divisor;
		double w = 1 - u - v;

	//	cout << "u = " << u << " v = "<< v << "w =" << w << endl;
	//	cout << "t = " << t << endl;

		Vec2d Alpha, Beta, Gamma;
	//	double t_u,t_v,t_w;
	//	t_u =u;t_v=v;t_w=w;
	//	u=v=w=1;
		if (k == 2)
		{
			Alpha[0] = a[0]; Alpha[1] = a[1];
			Beta[0] = b[0]; Beta[1] = b[1];
			Gamma[0] = c[0]; Gamma[1] = c[1];
		}
		else if (k == 1)
		{
			Alpha[0] = a[0]; Alpha[1] = a[2];
			Beta[0] = b[0]; Beta[1] = b[2];
			Gamma[0] = c[0]; Gamma[1] = c[2];
		}
		else if (k == 0)
		{
			Alpha[0] = a[1]; Alpha[1] = a[2];
			Beta[0] = b[1]; Beta[1] = b[2];
			Gamma[0] = c[1]; Gamma[1] = c[2];
		}

		//textural coordinates of the vertices
		// to keep it between 0 and 1
		Vec2d minV;
		minV[0]=min(min(Alpha[0],Beta[0]),Gamma[0]);
		minV[1]=min(min(Alpha[1],Beta[1]),Gamma[1]);
		minV[0]=-1*minV[0];
		minV[1]=-1*minV[1];

		Alpha+=minV;
		Beta+=minV;
		Gamma+=minV;

		Vec2d maxV;
		maxV[0]=max(max(Alpha[0],Beta[0]),Gamma[0]);
		maxV[1]=max(max(Alpha[1],Beta[1]),Gamma[1]);
		for(int l=0;l<2;l++) {
		Alpha[l]/=maxV[l];
		Beta[l]/=maxV[l];
		Gamma[l]/=maxV[l];
		}

		//u=t_u;
		//v=t_v;
		//w=t_w;
		Vec2d Final;
		for(int l=0;l<2;l++) {
		Final[l]= w*Alpha[l] + u*Beta[l]+ v*Gamma[l];
		}
		//cout << "Alpha =" << Alpha << "Beta =" << Beta << "Gamma = "<< Gamma << endl ;
		//cout << "Final[0] = " << Final[0] << "Final[1] = " << Final[1] << endl;
		//	Final = normalize(Final);
		if( u >= 0 && u <= 1 && v >= 0 && v <= 1 && w >= 0 && w <= 1){
			   i.setT(t);
			   i.setObject(this);
			   i.setBary(w,u,v);
			   //   i.setN(normal);

		   if(parent->normals.empty()){
			  i.setN(normal);
		  }
		  else{
						// Phong interpolation
						const Vec3d& na = parent->normals[ids[0]];
						const Vec3d& nb = parent->normals[ids[1]];
						const Vec3d& nc = parent->normals[ids[2]];

						i.setN(na*w + nb*u + nc*v); // w = alpha , u = Beta, v = Gamma
						i.setUVCoordinates(Final);
						/*	double q = u - w;
						double f =  u - v;
						cout << "Q =" << q << endl;
						cout<< "F = " << f << endl; */
		 }

			 if(!parent->materials.empty()){
										const Vec3d& da = parent->materials[ids[0]]->kd(i);
										const Vec3d& db = parent->materials[ids[1]]->kd(i);
										const Vec3d& dc = parent->materials[ids[2]]->kd(i);

										const Vec3d& sa = parent->materials[ids[0]]->ks(i);
										const Vec3d& sb = parent->materials[ids[1]]->ks(i);
										const Vec3d& sc = parent->materials[ids[2]]->ks(i);

										Material mat = *(parent->material);
										mat.setDiffuse(da*w + db*u + dc*v);
										mat.setSpecular(sa*w + sb*u + sc*v);
										i.setMaterial(mat);
								}
								 /*if( debugMode )
								 {
									 cout << "a = " << a << endl;
									 cout << "b = " << b << endl;
									 cout << "c = " << c << endl;
								 }*/
				  return true;
			 }

		return false;

#endif
}

void Trimesh::generateNormals()
// Once you've loaded all the verts and faces, we can generate per
// vertex normals by averaging the normals of the neighboring faces.
{
	int cnt = vertices.size();
	normals.resize( cnt );
	int *numFaces = new int[ cnt ]; // the number of faces assoc. with each vertex
	memset( numFaces, 0, sizeof(int)*cnt );

	for( Faces::iterator fi = faces.begin(); fi != faces.end(); ++fi )
	{
		Vec3d faceNormal = (**fi).getNormal();

		for( int i = 0; i < 3; ++i )
		{
			normals[(**fi)[i]] += faceNormal;
			++numFaces[(**fi)[i]];
		}
	}

	for( int i = 0; i < cnt; ++i )
	{
		if( numFaces[i] )
			normals[i]  /= numFaces[i];
	}

	delete [] numFaces;
	vertNorms = true;
}

