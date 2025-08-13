
#ifndef _SPHERE_H
#define _SPHERE_H

#include "material/material.h"
#include "shape.h"

class sphere : public shape {
	vec3 C;
	float R;
	aabb box;

public:
	sphere(const vec3 &cen, float r, std::shared_ptr<material> mat);

	bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const override;

	const aabb &get_aabb() const override;

	inline const vec3 &center() const { return C; }
	inline float radius() const { return R; }
};

#endif /* _SPHERE_H */
