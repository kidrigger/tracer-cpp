
#ifndef _HITABLE_H
#define _HITABLE_H

#include "aabb.h"
#include "ray.h"

class material;
class hitable;

struct hit_record {
	float t;
	vec3 p;
	float u;
	float v;
	vec3 normal;
	const material *mat;
	const hitable *obj;
};

class hitable {
public:
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const = 0;
	virtual const aabb &get_aabb() const = 0;
	virtual ~hitable() {}
};

#endif /* _HITABLE_H */
