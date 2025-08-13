
#ifndef _MESH_H
#define _MESH_H

#include "core/bvh.h"
#include "core/hitable.h"
#include "shape.h"
#include <memory>

class triangle : public hitable {
	vec3 vert0;
	vec3 vert1;
	vec3 vert2;
	aabb box;

	vec3 get_normal(float u, float v) const {
		return normalize(cross(vert0 - vert1, vert2 - vert1));
	}

public:
	triangle(vec3 p0, vec3 p1, vec3 p2) :
			vert0(p0), vert1(p1), vert2(p2), box(min(p0, min(p1, p2)), max(p0, max(p1, p2))) {
	}

	bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override {
		vec3 vertex0 = vert0;
		vec3 vertex1 = vert1;
		vec3 vertex2 = vert2;
		vec3 edge1, edge2, h, s, q;
		float a, f, u, v;
		edge1 = vertex1 - vertex0;
		edge2 = vertex2 - vertex0;
		h = cross(r.direction(), edge2);
		a = dot(edge1, h);
		if (a > -F32_EPSILON && a < F32_EPSILON)
			return false; // This ray is parallel to this triangle.
		f = 1.0 / a;
		s = r.origin() - vert0;
		u = f * dot(s, h);
		if (u < 0.0 || u > 1.0)
			return false;
		q = cross(s, edge1);
		v = f * dot(r.direction(), q);
		if (v < 0.0 || u + v > 1.0)
			return false;
		// At this stage we can compute t to find out where the intersection point is on the line.
		float t = f * dot(edge2, q);
		if (t > F32_EPSILON) // ray intersection
		{
			vec3 n = get_normal(u, v);
			if (dot(n, r.direction()) > 0) {
              n = -n;
            }
			rec.t = t;
			rec.u = u;
			rec.v = v;
			rec.normal = n;
			rec.p = r.origin() + r.direction() * t;
			return true;
		} else // This means that there is a line intersection but not a ray intersection.
			return false;
	}

	const aabb &get_aabb() const override {
		return box;
	};
};

class mesh : public shape {

	std::vector<std::shared_ptr<hitable> > polys;
	bvh blas;
	aabb box;

public:
	mesh(const std::vector<vec3> &vertices, const std::vector<int> &index, std::shared_ptr<material> mat) :
			shape(std::move(mat)) {
		assert(vertices.size() >= 3);
		assert(index.size() % 3 == 0);
		polys.reserve(index.size() / 3);

		auto it = index.begin();
		while (it != index.end()) {
			polys.emplace_back(std::make_shared<triangle>(vertices[*it], vertices[*(it + 1)], vertices[*(it + 2)]));

			it += 3;
		}

		blas = bvh(polys.begin(), polys.end());

		vec3 min_bound = vertices[0];
		vec3 max_bound = vertices[0];

		for (auto &vert : vertices) {
			min_bound = min(min_bound, vert);
			max_bound = max(max_bound, vert);
		}

		box = aabb(min_bound, max_bound);
	}

	bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const override {

		rec.t = t_max + 0.1f;

		if (!blas.hit(r, t_min, t_max, rec)) {
          return false;
        }

		rec.mat = mat();
		rec.obj = this;

		return true;
	}

	const aabb &get_aabb() const override {
		return box;
	}
};

#endif // _MESH_H
