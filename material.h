
#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "hitable.h"
#include "rng.h"

class material {
public:
	virtual bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const = 0;
	virtual vec3 emission(float u, float v, const vec3 &p) const { return vec3(0.0f); }
	virtual ~material() {}
};

template <typename TEXTURE>
class lambertian : public material {
	TEXTURE albedo;
	vec3 emissive;

public:
	lambertian(const TEXTURE &albedo, const vec3 &emission = vec3(0.0f)) :
			albedo(albedo),
			emissive(emission) {}
	virtual bool scatter(
			const ray &r, const hit_record &hit,
			vec3 &attenuation, ray &scattered) const {
		vec3 target = hit.p + hit.normal + random_in_unit_sphere();
		scattered = ray(hit.p, target - hit.p);
		attenuation = albedo.sample(0, 0, hit.p);
		return true;
	}

	virtual vec3 emission(float u, float v, const vec3 &p) const { return emissive; }
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

template <typename TEXTURE>
class isotropic : public material {
	TEXTURE color;

public:
	isotropic(const TEXTURE &color) :
			color(color) {}
	virtual bool scatter(const ray &r, const hit_record &hit, vec3 &attenuation, ray &scattered) const {
		scattered = ray(hit.p, random_in_unit_sphere());
		attenuation = color.sample(0, 0, hit.p);
		return true;
	}
};

template <typename VOLUME>
class constant_medium : public VOLUME {
	float density;

public:
	template <typename... Ts>
	constant_medium(float density, Ts... args) :
			density(density),
			VOLUME(std::forward<Ts>(args)...) {}
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const {
		hit_record rec1, rec2;
		if (VOLUME::hit(r, -std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), rec1)) {
			if (VOLUME::hit(r, rec1.t + 0.00001f, std::numeric_limits<float>::max(), rec2)) {
				if (rec1.t < tmin)
					rec1.t = tmin;
				if (rec2.t > tmax)
					rec2.t = tmax;
				if (rec1.t >= rec2.t)
					return false;
				if (rec1.t < 0)
					rec1.t = 0;
				float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
				float hit_distance = -(1.f / density) * log(rng());
				if (hit_distance < distance_inside_boundary) {
					rec.t = rec1.t + hit_distance / r.direction().length();
					rec.p = r.point_at(rec.t);
					rec.normal = vec3(1, 0, 0);
					rec.mat = this->mat;
					return true;
				}
			}
		}
		return false;
	}
};

#endif /* _MATERIAL_H */
