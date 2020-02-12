
#ifndef _RAY_H
#define _RAY_H

#include "vec3.h"

class ray {
	vec3 A;
	vec3 B;

public:
	ray() {}
	ray(const vec3 &a, const vec3 &b) :
			A(a),
			B(b) {}
	inline const vec3 &origin() const { return A; }
	inline const vec3 &direction() const { return B; }
	inline vec3 &origin() { return A; }
	inline vec3 &direction() { return B; }
	inline vec3 point_at(float t) const { return A + t * B; }
	inline vec3 operator[](float t) const { return A + t * B; }
};

#endif /* _RAY_H */
