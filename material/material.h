
#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <hitable.h>
#include <rng.h>
#include <texture.h>

class material {
public:
	virtual bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const = 0;
	virtual vec3 emission(float u, float v, const vec3 &p) const { return vec3(0.0f); }
	virtual ~material() {}
};

#include "dielectric.h"
#include "isotropic.h"
#include "lambertian.h"
#include "metallic.h"

#endif /* _MATERIAL_H */
