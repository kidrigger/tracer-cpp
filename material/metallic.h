
#ifndef _METALLIC_H
#define _METALLIC_H

#include "material.h"

class metallic : public material {
protected:
	vec3 albedo;
	float roughness;

public:
	metallic(const vec3 &albedo, float roughness) noexcept;

	virtual bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const;
};

#endif /* _METALLIC_H */