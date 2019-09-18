
#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "hitable.h"
#include "rng.h"

class material {
public:
	virtual bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const = 0;
	virtual ~material() {}
};

template <typename TEXTURE>
class lambertian : public material {
	TEXTURE albedo;

public:
	lambertian(const TEXTURE &albedo) :
			albedo(albedo) {}
	virtual bool scatter(
			const ray &r, const hit_record &hit,
			vec3 &attenuation, ray &scattered) const {
		vec3 target = hit.p + hit.normal + random_in_unit_sphere();
		scattered = ray(hit.p, target - hit.p);
		attenuation = albedo.sample(0, 0, hit.p);
		return true;
	}
};

class metallic : public material {
	vec3 albedo;
	float roughness;

public:
	metallic(const vec3 &albedo, float roughness) :
			albedo(albedo),
			roughness(roughness) {}
	virtual bool scatter(
			const ray &r, const hit_record &hit,
			vec3 &attenuation, ray &scattered) const {
		vec3 target = hit.p + reflect(r.direction(), hit.normal) + roughness * random_in_unit_sphere();
		scattered = ray(hit.p, target - hit.p);
		attenuation = albedo;
		return dot(target - hit.p, hit.normal) > 0;
	}
};

class dielectric : public material {
	float refractive_index;

public:
	dielectric(float refractive_index) :
			refractive_index(refractive_index) {}
	virtual bool scatter(
			const ray &r, const hit_record &hit,
			vec3 &attenuation, ray &scattered) const {
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

private:
	float schlick(float cosine, float ref_idx) const {
		float r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1 - r0) * pow((1 - cosine), 5);
	}
};

#endif /* _MATERIAL_H */
