#ifndef _PERLIN_H
#define _PERLIN_H

#include "rng.h"
#include "vec3.h"
#include <cassert>
#include <iostream>
#include <vector>

class perlin {
	static std::vector<vec3> ranvec;
	static std::vector<int> perm_x;
	static std::vector<int> perm_y;
	static std::vector<int> perm_z;

public:
	float noise(const vec3 &p) const;
};

#endif /* _PERLIN_H */