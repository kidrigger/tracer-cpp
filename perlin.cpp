#include "perlin.h"
#include "rng.h"
#include <algorithm>

static std::vector<vec3> ranvec_gen() {
	std::vector<vec3> retval(256);
	for (vec3 &i : retval) {
		i = normalize(random_in_unit_sphere());
	}
	return retval;
}

static std::vector<int> perm_gen() {
	std::vector<int> retval(256);
	for (int i = 0; i < 256; i++) {
		retval[i] = i;
	}
	std::random_shuffle(retval.begin(), retval.end());
	return retval;
}

std::vector<vec3> perlin::ranvec = ranvec_gen();
std::vector<int> perlin::perm_x = perm_gen();
std::vector<int> perlin::perm_y = perm_gen();
std::vector<int> perlin::perm_z = perm_gen();

inline float perlin_interp(vec3 c[2][2][2], float u, float v, float w) {
	float uu = u * u * (3 - 2 * u);
	float vv = v * v * (3 - 2 * v);
	float ww = w * w * (3 - 2 * w);

	float accum = 0;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				vec3 weight_v(u - i, v - j, w - k);
				accum += (i * uu + (1 - i) * (1 - uu)) *
						 (j * vv + (1 - j) * (1 - vv)) *
						 (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
			}
		}
	}
	return accum;
}

float perlin::noise(const vec3 &p) const {
	float u = p.x() - floor(p.x());
	float v = p.y() - floor(p.y());
	float w = p.z() - floor(p.z());

	int i = floor(p.x());
	int j = floor(p.y());
	int k = floor(p.z());
	vec3 c[2][2][2];
	for (int di = 0; di < 2; di++) {
		for (int dj = 0; dj < 2; dj++) {
			for (int dk = 0; dk < 2; dk++) {
				c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];
			}
		}
	}
	float f = perlin_interp(c, u, v, w);
	return f;
}

float perlin::turb(const vec3 &p, int depth) const {
	float accum = 0;
	vec3 temp_p = p;
	float weight = 1.0f;
	for (int i = 0; i < depth; i++) {
		accum += weight * noise(temp_p);
		weight *= 0.5f;
		temp_p *= 2.0f;
	}
	return std::abs(accum);
}
