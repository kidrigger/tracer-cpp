#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "vec3.h"

class texture {
public:
	virtual vec3 sample(float u, float v, const vec3 &point) const = 0;
};

class constant_texture : public texture {
	vec3 color;

public:
	constant_texture(const vec3 &color) :
			color(color) {}
	virtual vec3 sample(float u, float v, const vec3 &point) const {
		return color;
	}
};

class checker_texture : public texture {
	vec3 color_odd;
	vec3 color_even;
	vec3 scaling_factor;

public:
	checker_texture(const vec3 &odd, const vec3 &even, const vec3 &scaling = vec3(1.0f)) :
			color_odd(odd),
			color_even(even),
			scaling_factor(scaling) {}
	virtual vec3 sample(float u, float v, const vec3 &point) const {
		float val = sin(point.x() * scaling_factor.x()) * sin(point.y() * scaling_factor.y()) * sin(point.z() * scaling_factor.z());
		return (val < 0 ? color_even : color_odd);
	}
};

#endif /* _TEXTURE_H */