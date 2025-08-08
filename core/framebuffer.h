#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <algorithm>
#include <cassert>
#include <mutex>
#include <span>
#include <vector>

#include "vec3.h"

class framebuffer {
private:
	uint width;
	uint height;
	std::vector<vec3> data_vector;

public:
	framebuffer(uint width, uint height) :
			width(width),
			height(height),
			data_vector(width * height) {}

	size_t size() const { return data_vector.size(); }
	vec3 *data() { return data_vector.data(); }
	auto begin() { return data_vector.begin(); }
	auto end() { return data_vector.end(); }

	uint get_width() { return width; }
	uint get_height() { return height; }

	vec3 &operator[](int i) { return data_vector[i]; }
	vec3 &operator()(uint i, uint j) {
		assert(i <= width and j <= height);
		return data_vector[i + width * j];
	}
};

class tile {
private:
	uint begin_x;
	uint begin_y;
	uint end_x;
	uint end_y;
	vec3 *data_ptr;
	uint row_width;

public:
	tile(uint width, uint height, uint offset_x, uint offset_y, uint row_width, vec3 *data) :
			begin_x(offset_x),
			begin_y(offset_y),
			end_x(width + offset_x),
			end_y(height + offset_y),
			data_ptr(data),
			row_width(row_width) {}

	uint get_width() { return end_x - begin_x; }
	uint get_height() { return end_y - begin_y; }

	std::tuple<uint, uint> true_index(uint i, uint j) {
		assert(i <= get_width() and j <= get_height());

		return { begin_x + i, begin_y + j };
	}

	vec3 &operator()(uint i, uint j) {
		auto [x, y] = true_index(i, j);

		return data_ptr[x + row_width * y];
	}
};

inline std::vector<tile> create_tiles(framebuffer &fb, uint width, uint height) {
	uint w = fb.get_width();
	uint h = fb.get_height();
	uint x_count = (w % width) ? (w / width) + 1 : (w / width);
	uint y_count = (h % height) ? (h / height) + 1 : (h / height);

	vec3 *data = fb.data();
	std::vector<tile> tiles;
	tiles.reserve(x_count * y_count);
	for (uint i = 0; i < x_count; i++) {
		for (uint j = 0; j < y_count; j++) {
			auto tile_width = std::min(width, w - i * width);
			auto tile_height = std::min(height, h - j * height);
			tiles.emplace_back(tile_width, tile_height, i * width, j * width, w, data);
		}
	}
	return tiles;
}

#endif /* _FRAMEBUFFER_H */
