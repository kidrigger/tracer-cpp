
#include "bvh.h"

#include <limits>

bool aabb::hit(const ray &r, float t_min, float t_max) const {

	float lower_bound = t_min;
	float upper_bound = t_max;

	for (int i = 0; i < 3; i++) {
		float t0 = std::min((A[i] - r.origin()[i]) / r.direction()[i], (B[i] - r.origin()[i]) / r.direction()[i]);
		float t1 = std::max((A[i] - r.origin()[i]) / r.direction()[i], (B[i] - r.origin()[i]) / r.direction()[i]);

		lower_bound = std::max(lower_bound, t0);
		upper_bound = std::min(upper_bound, t1);
		if (lower_bound > upper_bound) {
			return false;
		}
	}

	return true;
}
