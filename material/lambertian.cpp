
#include "lambertian.h"

lambertian::lambertian(texture *albedo, const vec3 &emission) noexcept
		: albedo(albedo),
		  emissive(emission) {}

bool lambertian::scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const {
	vec3 target = hit.p + hit.normal + random_in_unit_sphere();
	scattered = ray(hit.p, target - hit.p);
	attenuation = albedo->sample(0, 0, hit.p);
	return true;
}

vec3 lambertian::emission(float u, float v, const vec3 &p) const {
	return emissive;
}

lambertian::~lambertian() {
	delete albedo;
}
