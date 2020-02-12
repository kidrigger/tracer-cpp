
#ifndef _DIELECTRIC_H
#define _DIELECTRIC_H

#include "metallic.h"

class dielectric : public material {
	float refractive_index;

public:
	dielectric(float refractive_index) noexcept;
	virtual bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const;

private:
	float schlick(float cosine, float ref_idx) const;
};

#endif /* _DIELECTRIC_H */
