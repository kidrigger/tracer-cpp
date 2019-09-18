
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
	world() :
			box(vec3(0), vec3(0)) {}
	template <typename T, typename... Ts>
	void add(Ts &&... args) {
		hitlist.push_back(new T(std::forward<Ts>(args)...));
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
