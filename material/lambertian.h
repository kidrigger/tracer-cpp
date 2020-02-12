
#ifndef _LAMBERTIAN_H
#define _LAMBERTIAN_H

#include "material.h"

class lambertian : public material {
protected:
	texture *albedo;
	vec3 emissive;

public:
	lambertian(texture *albedo, const vec3 &emission = vec3(0.0f)) noexcept;

	virtual bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const;

	virtual vec3 emission(float u, float v, const vec3 &p) const;

	virtual ~lambertian();
};

#endif /* _LAMBERTIAN_H */