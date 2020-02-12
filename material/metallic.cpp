
#include "metallic.h"

metallic::metallic(const vec3 &albedo, float roughness) noexcept
		: albedo(albedo),
		  roughness(roughness) {}

bool metallic::scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const {
	vec3 target = hit.p + reflect(r.direction(), hit.normal) + roughness * random_in_unit_sphere();
	scattered = ray(hit.p, target - hit.p);
	attenuation = albedo;
	return dot(target - hit.p, hit.normal) > 0;
}
