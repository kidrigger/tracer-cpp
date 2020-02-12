#ifndef _PERLIN_H
#define _PERLIN_H

#include <core.h>
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
	float turb(const vec3 &p, int depth = 7) const;
};

#endif /* _PERLIN_H */