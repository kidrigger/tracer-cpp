#include "camera.h"
#include "framebuffer.h"
#include "hitable.h"
#include "material.h"
#include "ray.h"
#include "rng.h"
#include "sphere.h"
#include "stbi_write.h"
#include "world.h"
#include <ThreadPool.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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

int main(int argc, char *argv[]) {
	struct {
		unsigned int WIDTH = 200;
		unsigned int HEIGHT = 100;
		unsigned int NSAMPLES = 10;
		unsigned int MAX_DEPTH = 25;
	} config{};

	std::ios_base::sync_with_stdio(false);
	std::cout.tie(nullptr);

	for (int i = 0; i < argc; i++) {
		if ("-s" == std::string(argv[i])) {
			config.NSAMPLES = atoi(argv[++i]);
		} else if ("-r" == std::string(argv[i])) {
			config.WIDTH = atoi(argv[++i]);
			config.HEIGHT = atoi(argv[++i]);
		} else if ("-d" == std::string(argv[i])) {
			config.MAX_DEPTH = atoi(argv[++i]);
		}
	}

	vec3 eye = vec3(0, 0, 1);
	vec3 target = vec3(0, 0, 0);
	float focal_length = (target - eye).length();
	camera cam(eye, target, vec3(0, 1, 0), 90.0f, (float)config.WIDTH / (float)config.HEIGHT,
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
				w.add<sphere>(vec3(-1, -1, -1) + vec3(rng(), rng(), rng()) - 0.5f, 10.f / 165.f, new lambertian(vec3(0.7f, 0.7f, 0.6f)));
			}
		}
	}

	w.add<sphere>(vec3(1, 0, 0), 0.5f, new metallic(vec3(0.8f), 0.0f));
	w.add<sphere>(vec3(0, 0, 0), 0.5f, new dielectric(1.5f));
	w.add<sphere>(vec3(0, 0, 0), -0.48f, new dielectric(1.5f));

	w.compile();

	auto optimal_threads = std::thread::hardware_concurrency();

	framebuffer<multi_thread_policy> accumulator(config.WIDTH, config.HEIGHT);
	{
		ThreadPool pool(optimal_threads);

		std::cerr << "RESOLUTION = " << config.WIDTH << "x" << config.HEIGHT << std::endl;
		std::cerr << "SAMPLES = " << config.NSAMPLES << std::endl;
		std::cerr << "MAX_DEPTH = " << config.MAX_DEPTH << std::endl;
		std::cerr << "THREADS = " << optimal_threads << std::endl;

		int count = config.NSAMPLES;

		auto cbk = [&count]() {count--; std::cerr << '\r' << count; };
		pool.register_callback(cbk);

		for (int s = 0; s < config.NSAMPLES; s++) {
			pool.enqueue([cam, &config, &accumulator, &w]() {
				framebuffer<single_thread_policy> buffer(config.WIDTH, config.HEIGHT);
				for (int j = 0; j < config.HEIGHT; j++) {
					for (int i = 0; i < config.WIDTH; i++) {
						float u = (i + rng()) / (float)config.WIDTH;
						float v = (j + rng()) / (float)config.HEIGHT;
						ray r = cam.get_ray(u, v);
						buffer[j * config.WIDTH + i] = color(r, &w, config.MAX_DEPTH);
					}
				}
				accumulator += buffer;
			});
		}
	}

	accumulator /= config.NSAMPLES;
	std::vector<uint8_t> img(config.WIDTH * config.HEIGHT * 3);
	int i = 0;
	for (auto &pixel : accumulator) {
		for (int j = 0; j < 3; j++) {
			img[3 * i + j] = static_cast<uint8_t>(255.99f * pixel[j]);
		}
		i++;
	}

	stbi_flip_vertically_on_write(true);
	stbi_write_png("image.png", config.WIDTH, config.HEIGHT, 3, img.data(), 3 * config.WIDTH);

	std::cerr << "\nRender Complete." << std::endl;

	return 0;
}
