#include "vec3.h"

vec3 vec3::operator+(const vec3 &v) const {
	return vec3(v[0] + e[0], v[1] + e[1], v[2] + e[2]);
}

vec3 vec3::operator-(const vec3 &v) const {
	return vec3(e[0] - v[0], e[1] - v[1], e[2] - v[2]);
}

vec3 vec3::operator*(const vec3 &v) const {
	return vec3(v[0] * e[0], v[1] * e[1], v[2] * e[2]);
}

vec3 vec3::operator/(const vec3 &v) const {
	return vec3(e[0] / v[0], e[1] / v[1], e[2] / v[2]);
}

vec3 vec3::operator*(float f) const {
	return vec3(e[0] * f, e[1] * f, e[2] * f);
}

vec3 vec3::operator/(float f) const {
	return vec3(e[0] / f, e[1] / f, e[2] / f);
}

const vec3 &vec3::operator+=(const vec3 &v) {
	e[0] += v[0];
	e[1] += v[1];
	e[2] += v[2];
	return *this;
}

const vec3 &vec3::operator-=(const vec3 &v) {
	e[0] -= v[0];
	e[1] -= v[1];
	e[2] -= v[2];
	return *this;
}

const vec3 &vec3::operator*=(const vec3 &v) {
	e[0] *= v[0];
	e[1] *= v[1];
	e[2] *= v[2];
	return *this;
}

const vec3 &vec3::operator/=(const vec3 &v) {
	e[0] /= v[0];
	e[1] /= v[1];
	e[2] /= v[2];
	return *this;
}

const vec3 &vec3::operator*=(float f) {
	e[0] *= f;
	e[1] *= f;
	e[2] *= f;
	return *this;
}

const vec3 &vec3::operator/=(float f) {
	e[0] /= f;
	e[1] /= f;
	e[2] /= f;
	return *this;
}
