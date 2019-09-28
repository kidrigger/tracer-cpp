
#ifndef _SPHERE_H
#define _SPHERE_H

#include "hitable.h"
#include "material.h"

class sphere : public hitable {
protected:
	vec3 C;
	float R;
	material *mat;
	aabb box;

public:
	sphere(const vec3 &cen, float r, material *mat) :
			C(cen),
			R(r),
			mat(mat),
			box(cen - r, cen + r) {}
	virtual ~sphere() { delete mat; }
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
	virtual const aabb &get_aabb() const { return box; }
	const vec3 &center() const { return C; }
	vec3 &center() { return C; }
	float radius() const { return R; }
	float &radius() { return R; }
};

#endif /* _SPHERE_H */
