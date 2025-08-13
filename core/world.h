
#ifndef _WORLD_H
#define _WORLD_H

#include "bvh.h"
#include <utility>
#include <vector>

class world : public hitable {
	std::vector<std::shared_ptr<hitable> > hitlist;
	aabb box;
	bvh tree;

public:
	world() noexcept
			: box(vec3(0), vec3(0)) {}

	void add(std::shared_ptr<hitable> object) {
		hitlist.emplace_back(std::move(object));
	}

	virtual const aabb &get_aabb() const { return box; }

	void compile() {
		tree = bvh(hitlist.begin(), hitlist.end());
	}

	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
};

#endif /* _WORLD_H */
