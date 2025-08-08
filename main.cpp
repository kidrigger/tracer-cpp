#include "stb/stbi_write.h"

#include "core/prelude.h"
#include "geometry/prelude.h"
#include "material/prelude.h"
#include "textures/prelude.h"

#include <ThreadPool.h>

constexpr float EXPOSURE = 4.5f;
constexpr float GAMMA = 2.2f;

vec3 Uncharted2Tonemap(const vec3 &color) {
	constexpr float A = 0.15f;
	constexpr float B = 0.5f;
	constexpr float C = 0.1f;
	constexpr float D = 0.2f;
	constexpr float E = 0.02f;
	constexpr float F = 0.30f;
	constexpr float W = 11.2f;
	return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

vec3 tonemap(const vec3 &color) {
	vec3 col = Uncharted2Tonemap(color * EXPOSURE);
	col = col * (1.0f / Uncharted2Tonemap(vec3(11.2f)));
	return col;
}

vec3 color(const ray &r, const hitable *world, int depth) {
	hit_record rec;
	if (depth == 0) {
		return vec3(0.0f);
	}
	if (world->hit(r, 0.001f, MAXFLOAT, rec)) {
		vec3 attenuation;
		ray scattered;
		vec3 emission = rec.mat->emission(rec.u, rec.v, rec.p);
		if (rec.mat->scatter(r, rec, attenuation, scattered)) {
			return emission + attenuation * color(scattered, world, depth - 1);
		} else {
			return emission;
		}
	} else {
		vec3 unit_dir = normalize(r.direction());
		float t = 0.5f * (unit_dir.y() + 1.0f);
		return (1.0f - t) * vec3(1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
		// return vec3(0.0f);
	}
}

int main(int argc, char *argv[]) {
	struct {
		unsigned int WIDTH = 640;
		unsigned int HEIGHT = 480;
		unsigned int NSAMPLES = 10;
		unsigned int MAX_DEPTH = 5;
	} config{};

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

	vec3 eye = vec3(0.5f, 1.0f, -1.5f);
	vec3 target = vec3(0, 0, 0);
	float focal_length = (target - eye).length();
	camera cam(eye, target, vec3(0, 1, 0), 60.0f, (float)config.WIDTH / (float)config.HEIGHT,
			0.0f, focal_length);

	std::vector<vec3> verts = {
		vec3{ 0.5f, 0.5f, 0.4f }, // TR
		vec3{ -0.5f, 0.5f, 0.4f }, // TL
		vec3{ -0.5f, -0.5f, 0.4f }, // BL
		vec3{ 0.5f, -0.5f, 0.4f }, // BR
	};
	std::vector<int> idx = {
		0,
		1,
		2,
		0,
		2,
		3,
	};

	world wrld;
	// w.add(new constant_medium(new sphere(vec3(0, 0, 0), 2.f, new isotropic(vec3(0.3f))), 0.7f));
	wrld.add(new sphere(vec3(-1, 0, 0), 0.5f, new dielectric(1.5f)));
	wrld.add(new sphere(vec3(-1, 0, 0), 0.45f, new lambertian(new constant_texture(vec3(0.8f, 0.2f, 0.1f)))));
	wrld.add(new sphere(vec3(0, 0, 0), 0.2f, new lambertian(new constant_texture(vec3(0.8f)), vec3(10.0f))));
	wrld.add(new sphere(vec3(1, 0, 0), 0.5f, new metallic(vec3(0.8f, 0.6f, 0.2f), 0.2f)));
	wrld.add(new sphere(vec3(0, -100.5f, 0), 100.0f, new lambertian(new checker_texture(vec3(1.0f), vec3(0.0f), vec3(10.0f)))));
	wrld.add(new mesh(verts, idx, new metallic(vec3(1.0f), 0.0f)));

	for (int i = -5; i < 5; i++) {
		for (int j = -5; j < 5; j++) {
			for (int k = -5; k < 5; k++) {
				wrld.add(new sphere(vec3(0, 0, -1) + vec3(rng(), rng(), rng()) - 0.5f, 0.04f, new dielectric(1.5f))); // lambertian(constant_texture(vec3(0.7f, 0.7f, 0.6f))));
			}
		}
	}

	// w.add<sphere>(vec3(1, 0, 0), 0.5f, new metallic(vec3(0.8f), 0.0f));
	// w.add<sphere>(vec3(0, 0, 0), 0.5f, new dielectric(1.5f));
	// w.add<sphere>(vec3(0, 0, 0), -0.48f, new dielectric(1.5f));

	wrld.compile();

	auto optimal_threads = std::thread::hardware_concurrency();

	framebuffer accumulator(config.WIDTH, config.HEIGHT);

	std::vector<tile> tiles = create_tiles(accumulator, 16, 16);
	{
		ThreadPool pool(optimal_threads);

		std::cerr << "RESOLUTION = " << config.WIDTH << "x" << config.HEIGHT << std::endl;
		std::cerr << "SAMPLES = " << config.NSAMPLES << std::endl;
		std::cerr << "MAX_DEPTH = " << config.MAX_DEPTH << std::endl;
		std::cerr << "THREADS = " << optimal_threads << std::endl;

		volatile std::atomic_int work = 0;
		size_t total_work = config.WIDTH * config.HEIGHT * config.NSAMPLES;
		float total_work_inv = 100.0f / total_work;

		for (auto &ctile : tiles) {
			pool.enqueue([&] {
				for (int s = 0; s < config.NSAMPLES; s++) {
					int h = (int)ctile.get_height();
					int w = (int)ctile.get_width();
					for (int j = 0; j < h; j++) {
						for (int i = 0; i < w; i++) {
							auto [x, y] = ctile.true_index(i, j);
							float u = (x + rng()) / (float)config.WIDTH;
							float v = (y + rng()) / (float)config.HEIGHT;
							ray r = cam.get_ray(u, v);
							ctile(i, j) += color(r, &wrld, config.MAX_DEPTH) / config.NSAMPLES;
						}
					}

					work += (w * h);
				}
			});
		}

		while (work < total_work) {
          printf("\r%.2f%% ", work * total_work_inv);
		}
	}
    
	std::vector<uint8_t> img(config.WIDTH * config.HEIGHT * 3);
	int i = 0;
	for (auto &pixel : accumulator) {
		pixel = pixel / (pixel + 0.25f);
		// tonemap(pixel);
		for (int j = 0; j < 3; j++) {
			img[3 * i + j] = static_cast<uint8_t>(255.99f * pixel[j]);
		}
		i++;
	}

	// [0, 1]
	// [0, inf)
	// f(x) = x / (1 + x);

	stbi_flip_vertically_on_write(true);
	stbi_write_png("image.png", config.WIDTH, config.HEIGHT, 3, img.data(), 3 * config.WIDTH);

    printf("\r100.0%%\t");
	std::cerr << "\nRender Complete." << std::endl;

	return 0;
}
