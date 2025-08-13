
#include "sphere.h"

bool sphere::hit(const ray &r, float tmin, float tmax, hit_record &rec) const {
	vec3 oc = r.origin() - C;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - R * R;
	float disc = b * b - a * c;
	if (disc > 0) {
		float temp = (-b - sqrt(b * b - a * c)) / a;
		if (temp < tmax && temp >= tmin) {
			rec.t = temp;
			rec.p = r[rec.t];
			rec.normal = (rec.p - C) / R;
			rec.mat = mat();
			rec.obj = static_cast<const hitable *>(this);
			return true;
		}
		temp = (-b + sqrt(b * b - a * c)) / a;
		if (temp < tmax && temp >= tmin) {
			rec.t = temp;
			rec.p = r[rec.t];
			rec.normal = (rec.p - C) / R;
			rec.mat = mat();
			rec.obj = static_cast<const hitable *>(this);
			return true;
		}
	}
	return false;
}

sphere::sphere(const vec3 &cen, float r, std::shared_ptr<material> mat) :
		shape(std::move(mat)), C(cen), R(r), box(cen - r, cen + r) {}

const aabb &sphere::get_aabb() const {
	return box;
}
