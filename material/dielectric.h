
#ifndef _DIELECTRIC_H
#define _DIELECTRIC_H

#include "material.h"

class dielectric : public material {
	float refractive_index;
  
	float schlick(float cosine, float ref_idx) const;

public:
	dielectric(float refractive_index) noexcept;
	bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const override;
};

#endif /* _DIELECTRIC_H */
