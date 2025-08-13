
#ifndef _METALLIC_H
#define _METALLIC_H

#include "material.h"

class metallic : public material {
	vec3 albedo;
	float roughness;

public:
	metallic(const vec3 &albedo, float roughness) noexcept;

	bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const override;
};

#endif /* _METALLIC_H */
