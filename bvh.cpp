
#include "bvh.h"

#include <limits>

bool bvh::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
	try {
		if (root) {
			return root->hit(r, t_min, t_max, rec);
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return false;
	}
	return false;
}

const aabb &bvh::get_aabb() const {
	return root->get_aabb();
}

bvh_node::bvh_node(const std::vector<hitable *>::iterator &beg, const std::vector<hitable *>::iterator &end) {
	auto size = (end - beg);
	if (size == 3) {
		lchild = new bvh_node(*beg);
		rchild = new bvh_node(beg + 1, end);
	} else if (size == 2) {
		lchild = new bvh_node(*beg);
		rchild = new bvh_node(*(beg + 1));
	} else {
		int axis = static_cast<int>(2.99f * rng());
		std::sort(beg, end, [axis](hitable *a, hitable *b) {
			return a->get_aabb().get_min()[axis] < b->get_aabb().get_min()[axis];
		});
		auto mid_idx = size / 2;
		auto mid = beg + mid_idx;
		lchild = new bvh_node(beg, mid);
		rchild = new bvh_node(mid, end);
	}
	box = aabb::combine(lchild->get_aabb(), rchild->get_aabb());
}

bool bvh_node::hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
	if (!box.hit(r, t_min, t_max)) {
		return false;
	}

	hit_record l_rec;
	hit_record r_rec;
	bool hit_left = lchild->hit(r, t_min, t_max, l_rec);
	bool hit_right = (rchild ? rchild->hit(r, t_min, t_max, r_rec) : false);

	if (hit_left && hit_right) {
		rec = l_rec.t < r_rec.t ? l_rec : r_rec;
		return true;
	} else if (hit_left) {
		rec = l_rec;
		return true;
	} else if (hit_right) {
		rec = r_rec;
		return true;
	} else {
		return false;
	}
}
