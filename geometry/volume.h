
#ifndef _VOLUME_H
#define _VOLUME_H

#include "shape.h"

class constant_medium : public hitable {
	shape *volume;
	float density;

public:
	constant_medium(shape *volume, float density) :
			volume(volume), density(density) {}

	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const {
		hit_record rec1, rec2;
		if (volume->hit(r, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), rec1)) {
			if (volume->hit(r, rec1.t + 0.00001f, std::numeric_limits<float>::max(), rec2)) {
				if (rec1.t < tmin)
					rec1.t = tmin;
				if (rec2.t > tmax)
					rec2.t = tmax;
				if (rec1.t >= rec2.t)
					return false;
				if (rec1.t < 0)
					rec1.t = 0;
				float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
				float hit_distance = -(1.f / density) * log(rng());
				if (hit_distance < distance_inside_boundary) {
					rec.t = rec1.t + hit_distance / r.direction().length();
					rec.p = r.point_at(rec.t);
					rec.normal = vec3(1, 0, 0);
					rec.mat = volume->mat();
					return true;
				}
			}
		}
		return false;
	}

	virtual const aabb &get_aabb() const { return volume->get_aabb(); }

	virtual ~constant_medium() {
		delete volume;
	}
};

#endif /* _VOLUME_H */
