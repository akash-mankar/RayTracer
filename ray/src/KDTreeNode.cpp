#include "KDTreeNode.h"
vector <Geometry*> KDTreeNode::getChildren() { 
	return children;
}
void KDTree::setScene(Scene* scene) { 
	this->scene = scene; 
}
