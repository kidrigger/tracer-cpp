
#ifndef _ISOTROPIC_H
#define _ISOTROPIC_H

#include "material.h"

class isotropic : public material {
	vec3 color;

public:
	isotropic(const vec3 &color) noexcept;

	virtual bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const;
};

#endif /* _ISOTROPIC_H */