#ifndef _AABB_H
#define _AABB_H

#include "ray.h"

class aabb {
	vec3 A;
	vec3 B;

public:
	aabb() :
			A(0),
			B(0) {}
	aabb(const vec3 &minp, const vec3 &maxp) :
			A(min(minp, maxp)),
			B(max(minp, maxp)) {}
	bool hit(const ray &r, float t_min, float t_max) const;

	const vec3 &get_min() const {
		return A;
	}

	const vec3 &get_max() const {
		return B;
	}

	static aabb combine(const aabb &a, const aabb &b) {
		return aabb(min(a.A, b.A), max(a.B, b.B));
	}
};

#endif /* _AABB_H */