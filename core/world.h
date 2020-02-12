
#ifndef _WORLD_H
#define _WORLD_H

#include "bvh.h"
#include <utility>
#include <vector>

class world : public hitable {
	std::vector<hitable *> hitlist;
	aabb box;
	bvh tree;

public:
	world() noexcept
			: box(vec3(0), vec3(0)) {}

	void add(hitable *object) {
		hitlist.push_back(object);
	}

	virtual const aabb &get_aabb() const { return box; }

	void compile() {
		tree = bvh(hitlist.begin(), hitlist.end());
	}

	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;

	virtual ~world() {
		for (hitable *obj : hitlist) {
			delete obj;
		}
	}
};

#endif /* _WORLD_H */
