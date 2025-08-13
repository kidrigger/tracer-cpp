#ifndef _BVH_H
#define _BVH_H

#include "hitable.h"
#include "rng.h"
#include <algorithm>
#include <deque>
#include <memory>
#include <vector>

class bvh;

class bvh_node : public hitable {
	std::shared_ptr<hitable> lchild;
	std::shared_ptr<hitable> rchild;
	aabb box;

	using hitlist_iterator = typename std::vector<std::shared_ptr<hitable> >::iterator;

public:
	bvh_node(std::shared_ptr<hitable> hitter) :
			lchild(std::move(hitter)),
			box(lchild->get_aabb()) {}
	bvh_node(const hitlist_iterator &beg, const hitlist_iterator &end);
	bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override;
	const aabb &get_aabb() const override { return box; }
};

class bvh : public hitable {
	std::unique_ptr<bvh_node> root;

	using hitlist_iterator = typename std::vector<std::shared_ptr<hitable> >::iterator;

public:
	bvh() {}
	bvh(const hitlist_iterator &beg, const hitlist_iterator &end) {
		root = std::make_unique<bvh_node>(beg, end);
	}

	const aabb &get_aabb() const override;
	bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override;
};

#endif /* _BVH_H */
