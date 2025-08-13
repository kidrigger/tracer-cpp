
#ifndef _LAMBERTIAN_H
#define _LAMBERTIAN_H

#include "material.h"

#include <memory>

class lambertian : public material {
	std::shared_ptr<texture> albedo;
	vec3 emissive;

public:
	lambertian(std::shared_ptr<texture> albedo, const vec3 &emission = vec3(0.0f)) noexcept;

	bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const override;

	vec3 emission(float u, float v, const vec3 &p) const override;
};

#endif /* _LAMBERTIAN_H */
