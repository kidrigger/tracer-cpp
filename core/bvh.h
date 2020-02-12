#ifndef _BVH_H
#define _BVH_H

#include "hitable.h"
#include "rng.h"
#include <algorithm>
#include <deque>
#include <vector>

class bvh;

class bvh_node : public hitable {
	hitable *lchild{ nullptr };
	hitable *rchild{ nullptr };
	aabb box;

public:
	bvh_node(hitable *hitter) :
			lchild(hitter),
			box(hitter->get_aabb()) {}
	bvh_node(const std::vector<hitable *>::iterator &beg, const std::vector<hitable *>::iterator &end);
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;
	~bvh_node() {
		if (rchild) {
			delete lchild;
			delete rchild;
		}
	}
	const aabb &get_aabb() const { return box; }
};

class bvh : public hitable {
private:
	bvh_node *root{ nullptr };

public:
	bvh() {}
	bvh(const std::vector<hitable *>::iterator &beg, const std::vector<hitable *>::iterator &end) {
		root = new bvh_node(beg, end);
	}
	bvh(const bvh &other) = delete;
	bvh &operator=(const bvh &other) = delete;
	bvh(bvh &&other) {
		std::swap(root, other.root);
	}
	bvh &operator=(bvh &&other) {
		if (this == &other) {
			return *this;
		}
		std::swap(root, other.root);
		return *this;
	}

	virtual const aabb &get_aabb() const;
	virtual bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const;

	virtual ~bvh() {
		delete root;
	}
};

#endif /* _BVH_H */
