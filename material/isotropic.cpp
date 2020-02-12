
#include "isotropic.h"

isotropic::isotropic(const vec3 &color) noexcept
		: color(color) {}

bool isotropic::scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const {
	scattered = ray(hit.p, random_in_unit_sphere());
	attenuation = color;
	return true;
}
