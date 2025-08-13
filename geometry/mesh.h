
#ifndef _MESH_H
#define _MESH_H

#include "shape.h"

class triangle {
	vec3 vert0;
	vec3 vert1;
	vec3 vert2;

	vec3 get_normal(float u, float v) const {
		return normalize(cross(vert0 - vert1, vert2 - vert1));
	}

public:
	triangle(vec3 p0, vec3 p1, vec3 p2) :
			vert0(p0), vert1(p1), vert2(p2) {
	}

	bool hit(const ray &r, float t_min, float t_max, hit_record &rec) const {
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
			rec.t = t;
			rec.u = u;
			rec.v = v;
			rec.normal = get_normal(u, v);
			rec.p = r.origin() + r.direction() * t;
			return true;
		} else // This means that there is a line intersection but not a ray intersection.
			return false;
	}
};

class mesh : public shape {

	std::vector<triangle> polys;
	aabb box;

public:
	mesh(const std::vector<vec3> &vertices, const std::vector<int> &index, std::shared_ptr<material> mat) :
			shape(std::move(mat)) {
		assert(vertices.size() >= 3);
		assert(index.size() % 3 == 0);
		polys.reserve(index.size() / 3);

		auto it = index.begin();
		while (it != index.end()) {
			polys.emplace_back(vertices[*it], vertices[*(it + 1)], vertices[*(it + 2)]);

			it += 3;
		}

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

		hit_record hit;
		for (auto &tri : polys) {
			if (tri.hit(r, t_min, t_max, hit)) {
				if (hit.t < rec.t) {
					rec = hit;
				}
			}
		}

		rec.mat = mat();
		rec.obj = this;

		return hit.t < t_max;
	}

	const aabb &get_aabb() const override {
		return box;
	}
};

#endif // _MESH_H
