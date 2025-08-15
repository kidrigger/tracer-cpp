#ifndef _SKYBOX_H
#define _SKYBOX_H

#include "core/constants.h"
#include "core/prelude.h"
#include "thirdparty/stb_image.h"
#include <cmath>

class skybox {
	vec3 *data;
	int width;
	int height;

	vec3 sample_spherical(const vec3 &v) const {
		vec3 inv_tan = vec3(0.1591f, 0.3183f, 0.0f); // (1/2PI, 1/PI)
		vec3 uv = vec3(atan2(v.x(), -v.z()), asin(-v.y()), 0.0f); // (-PI, -PI/2) to (PI, PI/2)
		uv *= inv_tan; // (-1/2, -1/2) to (1/2, 1/2)
		uv += 0.5f; // (0, 0) to (1, 1)
		return uv;
	}

	vec3 sample_exact(int x, int y) const {
		// X wraps.
		int i = (x + width) % width;
		// Y mirrors
		int j = ((y / height) % 2 == 0) ? (y + height) % height : height - (y + height) % height;

		return data[i + width * j];
	}

public:
	skybox(const char *load_from) {
		int nc;
		data = (vec3 *)stbi_loadf(load_from, &width, &height, &nc, 3);
	}

	vec3 sample(const vec3 &direction) const {
		vec3 uv = sample_spherical(direction.normalized());

		float pixx = (width - 1) * uv.x();
		float pixy = (height - 1) * uv.y();
		float cpixx = ceil(pixx);
		float fpixx = floor(pixx);
		float cpixy = ceil(pixy);
		float fpixy = floor(pixy);

		// max distance between floor and ceil is 1.0
		// So distance from x to ceil is weight of floor.
		float wfx = abs(cpixx - pixx);
		float wfy = abs(cpixy - pixy);
		// And from x to floor is weight of ceil
		float wcx = abs(fpixx - pixx);
		float wcy = abs(fpixy - pixy);
		// Edge case, pixx is integral.
		// So both cpixx and fpixx are equal to pixx
		// Same with pixy

		bool integral_x = 1.0f - (wfx + wcx) < F32_EPSILON;
		bool integral_y = 1.0f - (wfy + wcy) < F32_EPSILON;
		if (integral_x && integral_y) {
			return sample_exact((int)pixx, (int)pixy);
		} else if (integral_x) {
			return wfy * sample_exact((int)pixx, (int)fpixy) + wcy * sample_exact((int)pixx, (int)cpixy);
		} else if (integral_y) {
			return wfx * sample_exact((int)fpixx, (int)pixy) + wcx * sample_exact((int)cpixx, (int)pixy);
		} else {
			// normal case
			float wff = sqrt(wfx * wfx + wfy * wfy);
			float wcf = sqrt(wcx * wcx + wfy * wfy);
			float wfc = sqrt(wfx * wfx + wcy * wcy);
			float wcc = sqrt(wcx * wcx + wcy * wcy);

			vec3 ff = wff * sample_exact((int)fpixx, (int)fpixy);
			vec3 cf = wcf * sample_exact((int)cpixx, (int)fpixy);
			vec3 fc = wfc * sample_exact((int)fpixx, (int)cpixy);
			vec3 cc = wcc * sample_exact((int)cpixx, (int)cpixy);

			return ff + cf + fc + cc;
		}
	}

	~skybox() {
      stbi_image_free(data);
    }
};

#endif // _SKYBOX_H
