#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <ThreadPool.h>

#include <vector>

#include <ThreadPool.h>

#include "camera.h"
#include "hitable.h"
#include "material.h"
#include "ray.h"
#include "sphere.h"
#include "world.h"

#include "rng.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbi_write.h"

vec3 color(const ray &r, const hitable *world, int depth) {
	hit_record rec;
	if (depth == 0) {
		return vec3(0.0f);
	}
	if (world->hit(r, 0.001f, MAXFLOAT, rec)) {
		vec3 attenuation;
		ray scattered;
		if (rec.mat->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth - 1);
		} else {
			return vec3(0.0f);
		}
	} else {
		vec3 unit_dir = normalize(r.direction());
		float t = 0.5f * (unit_dir.y() + 1.0f);
		return (1.0f - t) * vec3(1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
	}
}

class single_thread_policy {
public:
	void lock(){};
	void unlock(){};
};

class multi_thread_policy {
	std::mutex locke;

public:
	void lock() { locke.lock(); }
	void unlock() { locke.unlock(); }
};

template <class thread_policy>
class framebuffer : public thread_policy {
private:
	int width;
	int height;
	std::vector<vec3> data_vector;

public:
	framebuffer(int width, int height) :
			width(width),
			height(height),
			data_vector(width * height) {}

	size_t size() const { return data_vector.size(); }
	auto begin() { return data_vector.begin(); }
	auto end() { return data_vector.end(); }
	auto data() { return data_vector.data(); }

	vec3 &operator[](int i) { return data_vector[i]; }
	vec3 &operator()(int i, int j) { return data_vector[i + width * j]; }
	template <typename TP>
	[[nodiscard]] std::vector<vec3> operator+(framebuffer<TP> &other) {
		this->lock();
		std::vector<vec3> res(other.size());
		std::transform(data_vector.begin(), data_vector.end(), other.begin(),
				res.begin(), std::plus<vec3>());
		this->unlock();
		return res;
	}
	template <typename TP>
	std::vector<vec3> &operator+=(framebuffer<TP> &other) {
		this->lock();
		std::transform(data_vector.begin(), data_vector.end(), other.begin(),
				data_vector.begin(), std::plus<vec3>());
		this->unlock();
		return data_vector;
	}
};

template <typename TP>
void operator/=(framebuffer<TP> &buffer, float scalar) {
	for (auto &element : buffer) {
		element /= scalar;
	}
}

int main(int argc, char *argv[]) {
	unsigned int WIDTH = 200, HEIGHT = 100;
	unsigned int NSAMPLES = 10;

	std::ios_base::sync_with_stdio(false);
	std::cout.tie(nullptr);

	for (int i = 0; i < argc; i++) {
		if ("-s" == std::string(argv[i])) {
			NSAMPLES = atoi(argv[++i]);
		} else if ("-r" == std::string(argv[i])) {
			WIDTH = atoi(argv[++i]);
			HEIGHT = atoi(argv[++i]);
		}
	}

	vec3 eye = vec3(0, 0, -1);
	vec3 target = vec3(0, 0, 0);
	float focal_length = (target - eye).length();
	camera cam(eye, target, vec3(0, 1, 0), 60.0f, (float)WIDTH / (float)HEIGHT,
			0.0f, focal_length);

	world w;
	// w.add<sphere>(vec3(-1, 0, -1), 0.5f, new dielectric(1.5f));
	// w.add<sphere>(vec3(0, 0, -1), 0.5f, new lambertian(vec3(0.1f, 0.2f, 0.5f)));
	// w.add<sphere>(vec3(1, 0, -1), 0.5f,
	// 		new metallic(vec3(0.8f, 0.6f, 0.2f), 0.2f));
	// w.add<sphere>(vec3(0, -100.5f, -1), 100.0f,
	// 		new lambertian(vec3(0.8f, 0.8f, 0.0f)));

	for (int i = -5; i < 5; i++) {
		for (int j = -5; j < 5; j++) {
			for (int k = -5; k < 5; k++) {
				w.add<sphere>(vec3(i + 0.5f, j + 0.5f, k + 0.5f), 0.25f, new metallic(vec3(0.8f), 0.0f));
			}
		}
	}

	w.compile();

	auto optimal_threads = std::thread::hardware_concurrency();

	framebuffer<multi_thread_policy> accumulator(WIDTH, HEIGHT);
	{
		ThreadPool pool(optimal_threads);

		std::cerr << "RESOLUTION = " << WIDTH << "x" << HEIGHT << std::endl;
		std::cerr << "SAMPLES = " << NSAMPLES << std::endl;
		std::cerr << "THREADS = " << optimal_threads << std::endl;

		int count = NSAMPLES;

		auto cbk = [&count]() {count--; std::cerr << '\r' << count; };
		pool.register_callback(cbk);

		for (int s = 0; s < NSAMPLES; s++) {
			pool.enqueue([cam, HEIGHT, WIDTH, &accumulator, &w]() {
				framebuffer<single_thread_policy> buffer(WIDTH, HEIGHT);
				for (int j = 0; j < HEIGHT; j++) {
					for (int i = 0; i < WIDTH; i++) {
						float u = (i + rng()) / (float)WIDTH;
						float v = (j + rng()) / (float)HEIGHT;
						ray r = cam.get_ray(u, v);
						buffer[j * WIDTH + i] = color(r, &w, 25);
					}
				}
				accumulator += buffer;
			});
		}
	}

	accumulator /= NSAMPLES;
	std::vector<uint8_t> img(WIDTH * HEIGHT * 3);
	int i = 0;
	for (auto &pixel : accumulator) {
		for (int j = 0; j < 3; j++) {
			img[3 * i + j] = static_cast<uint8_t>(255.99f * pixel[j]);
		}
		i++;
	}

	stbi_flip_vertically_on_write(true);
	stbi_write_png("image.png", WIDTH, HEIGHT, 3, img.data(), 3 * WIDTH);

	return 0;
}
