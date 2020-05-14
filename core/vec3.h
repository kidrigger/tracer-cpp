
#ifndef _VEC3_H
#define _VEC3_H

#include <cmath>
#include <istream>
#include <ostream>
#include <tuple>

class vec3 {
	float e[3];

public:
	vec3() {}
	vec3(float x) :
			vec3(x, x, x) {}
	vec3(float x, float y, float z) {
		e[0] = x;
		e[1] = y;
		e[2] = z;
	}
	inline float x() const { return e[0]; }
	inline float y() const { return e[1]; }
	inline float z() const { return e[2]; }
	inline float r() const { return e[0]; }
	inline float g() const { return e[1]; }
	inline float b() const { return e[2]; }
	inline float &x() { return e[0]; }
	inline float &y() { return e[1]; }
	inline float &z() { return e[2]; }
	inline float &r() { return e[0]; }
	inline float &g() { return e[1]; }
	inline float &b() { return e[2]; }

	const vec3 &operator=(const vec3 &v) {
		for (int i = 0; i < 3; i++) {
			e[i] = v.e[i];
		}
		return *this;
	}

	inline const vec3 &operator+() const { return *this; }
	inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
	inline float operator[](int i) const { return e[i]; }
	inline float &operator[](int i) { return e[i]; }

	vec3 operator+(const vec3 &v) const;
	vec3 operator-(const vec3 &v) const;
	vec3 operator*(const vec3 &v) const;
	vec3 operator/(const vec3 &v) const;
	vec3 operator*(float f) const;
	vec3 operator/(float f) const;
	const vec3 &operator+=(const vec3 &v);
	const vec3 &operator-=(const vec3 &v);
	const vec3 &operator*=(const vec3 &v);
	const vec3 &operator/=(const vec3 &v);
	const vec3 &operator*=(float f);
	const vec3 &operator/=(float f);

	inline float sqr_length() const {
		return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
	}

	inline float length() const { return sqrt(sqr_length()); }

	inline vec3 normalized() const { return *this * (1.0f / length()); }

	inline const vec3 &normalize() { return *this *= (1.0f / length()); }
};

inline vec3 operator*(float f, const vec3 &vec) {
	return vec * f;
}

inline vec3 operator/(float f, const vec3 &vec) {
	return vec3(f / vec.x(), f / vec.y(), f / vec.z());
}

inline vec3 normalize(const vec3 &vec) {
	return vec.normalized();
}

inline float dot(const vec3 &a, const vec3 &b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline vec3 pow(const vec3 &a, float ex) {
	return vec3(powf32(a.x(), ex), powf32(a.y(), ex), powf32(a.z(), ex));
}

inline vec3 cross(const vec3 &a, const vec3 &b) {
	return vec3(a[1] * b[2] - a[2] * b[1], -a[0] * b[2] + a[2] * b[0],
			a[0] * b[1] - a[1] * b[0]);
}

inline vec3 min(const vec3 &a, const vec3 &b) {
	return vec3(std::min(a[0], b[0]), std::min(a[1], b[1]), std::min(a[2], b[2]));
}

inline vec3 max(const vec3 &a, const vec3 &b) {
	return vec3(std::max(a[0], b[0]), std::max(a[1], b[1]), std::max(a[2], b[2]));
}

inline std::istream &operator>>(std::istream &in, vec3 &vec) {
	return in >> vec[0] >> vec[1] >> vec[2];
}

inline std::ostream &operator<<(std::ostream &out, vec3 &vec) {
	return out << vec[0] << ' ' << vec[1] << ' ' << vec[2];
}

inline vec3 reflect(const vec3 &incident, const vec3 &normal) {
	vec3 unit_normal = normalize(normal);
	return incident - 2 * unit_normal * dot(unit_normal, incident);
}

inline std::tuple<vec3, bool> refract(const vec3 &incident, const vec3 &normal,
		float ni_over_nr) {
	vec3 uv = incident.normalized();
	float dt = dot(uv, normal);
	float discriminant = 1.0 - ni_over_nr * ni_over_nr * (1 - dt * dt);
	if (discriminant > 0) {
		return std::make_tuple(
				ni_over_nr * (uv - normal * dt) - normal * sqrt(discriminant), false);
	} else {
		return std::make_tuple(reflect(incident, normal), true);
	}
}

#endif /* _VEC3_H */
