
#include "world.h"

bool world::hit(const ray &r, float tmin, float tmax, hit_record &rec) const {

	bool hit = false;
	// hit = tree.hit(r, tmin, tmax, rec);
	float closest = tmax;
	for (const hitable *obj : hitlist) {
		hit_record temprec;
		if (obj->hit(r, tmin, tmax, temprec)) {
			if (temprec.t < closest) {
				rec = temprec;
				closest = temprec.t;
				hit = true;
			}
		}
	}
	return hit;
}
