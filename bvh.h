#ifndef _BVH_H
#define _BVH_H

#include "hitable.h"

class aabb {
	vec3 A;
	vec3 B;

public:
	aabb(const vec3 &minp, const vec3 &maxp) :
			A(min(minp, maxp)),
			B(max(minp, maxp)) {}
	bool hit(const ray &r, float t_min, float t_max) const;
};

class bvh_node {
	bvh_node *parent{ nullptr };
	bvh_node *lchild{ nullptr };
	bvh_node *rchild{ nullptr };

public:
	bvh_node()
};

#endif /* _BVH_H */
