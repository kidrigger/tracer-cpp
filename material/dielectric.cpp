
#include "dielectric.h"

dielectric::dielectric(float refractive_index) noexcept
		: refractive_index(refractive_index) {}

bool dielectric::scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const {
	float ni_over_nr;
	vec3 outward_normal;
	float cosine;
	if (dot(r.direction(), hit.normal) > 0) {
		outward_normal = -hit.normal;
		ni_over_nr = refractive_index;
		cosine = refractive_index * dot(r.direction(), hit.normal) / r.direction().length();
	} else {
		outward_normal = hit.normal;
		ni_over_nr = 1.0f / refractive_index;
		cosine = -dot(r.direction(), hit.normal) / r.direction().length();
	}
	attenuation = vec3(1.0f);
	auto [dir, tir] = refract(r.direction(), outward_normal, ni_over_nr);
	if (!tir) {
		if (rng() < schlick(cosine, refractive_index)) {
			dir = reflect(r.direction(), hit.normal);
		}
	}
	scattered = ray(hit.p, dir);
	return true;
}

float dielectric::schlick(float cosine, float ref_idx) const {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}
