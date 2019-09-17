
#ifndef _WORLD_H
#define _WORLD_H

#include "hitable.h"
#include <utility>
#include <vector>

class world : public hitable {
	std::vector<hitable *> hitlist;

public:
	world() {}
	template <typename T, typename... Ts>
	void add(Ts &&... args) {
		hitlist.push_back(new T(std::forward<Ts>(args)...));
	}
	virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
	virtual ~world() {
		for (hitable *obj : hitlist) {
			delete obj;
		}
	}
};

#endif /* _WORLD_H */
