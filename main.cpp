#include "stb/stbi_write.h"

#include "core/prelude.h"
#include "core/thread_pool.h"
#include "geometry/prelude.h"
#include "material/prelude.h"
#include "textures/prelude.h"

#include <atomic>
#include <memory>
#include <string_view>

constexpr float EXPOSURE = 4.5f;
constexpr uint NTHREAD_AUTO = 0xFFFFFFFF;

vec3 Uncharted2Tonemap(const vec3 &color) {
	constexpr float A = 0.15f;
	constexpr float B = 0.5f;
	constexpr float C = 0.1f;
	constexpr float D = 0.2f;
	constexpr float E = 0.02f;
	constexpr float F = 0.30f;
	return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

vec3 tonemap(const vec3 &color) {
	constexpr float W = 11.2f;
	vec3 col = Uncharted2Tonemap(color * EXPOSURE);
	col = col * (1.0f / Uncharted2Tonemap(W));
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
	}
}

int main(int argc, char *argv[]) {
	struct {
		uint WIDTH = 640;
		uint HEIGHT = 480;
		uint NSAMPLES = 10;
		uint MAX_DEPTH = 5;
		uint NTHREADS = NTHREAD_AUTO;
		uint TILE_SIZE = 4;
	} config;

	for (int i = 0; i < argc; i++) {
		if ("-s" == std::string_view(argv[i])) {
			config.NSAMPLES = atoi(argv[++i]);
		} else if ("-r" == std::string_view(argv[i])) {
			config.WIDTH = atoi(argv[++i]);
			config.HEIGHT = atoi(argv[++i]);
		} else if ("-d" == std::string_view(argv[i])) {
			config.MAX_DEPTH = atoi(argv[++i]);
		} else if ("-x" == std::string_view(argv[i])) {
			config.TILE_SIZE = atoi(argv[++i]);
		} else if ("-j" == std::string_view(argv[i])) {
			config.NTHREADS = atoi(argv[++i]);
		}
	}

	vec3 eye = vec3(0.0f, 0.0f, -2.0f);
	vec3 target = vec3(0, 0, 0.0f);
	float focal_length = (target - eye).length();
	camera cam(eye, target, vec3(0, 1, 0), 60.0f, (float)config.WIDTH / (float)config.HEIGHT,
			0.0f, focal_length);

	std::vector<vec3> verts;
	{
		float x_beg = -0.5f;
		float x_end = 0.5f;
		float y_beg = -0.5f;
		float y_end = 0.5f;
		int subdiv = 100;

		vec3 o = vec3(x_beg, y_beg, 0.5f);
		vec3 dx = vec3(x_end - x_beg, 0.0f, 0.0f) / subdiv;
		vec3 dy = vec3(0.0f, y_end - y_beg, 0.0f) / subdiv;

		for (int i = 0; i < subdiv; i++) {
			for (int j = 0; j < subdiv; j++) {
				verts.push_back(i * dx + j * dy + o);
				verts.push_back((i + 1) * dx + j * dy + o);
				verts.push_back(i * dx + (j + 1) * dy + o);

				verts.push_back((i + 1) * dx + j * dy + o);
				verts.push_back((i + 1) * dx + (j + 1) * dy + o);
				verts.push_back(i * dx + (j + 1) * dy + o);
			}
		}

		for (vec3 &v : verts) {
			auto x = v.x();
			auto y = v.y();
			auto z = (x * x + y * y) / 2.0f;
            v.z() -= z;
        }
	}

	std::vector<int> idx;
	idx.reserve(verts.size());
	for (int i = 0; i < verts.size(); i++) {
		idx.push_back(i);
	}

	world wrld;
	wrld.add(std::make_shared<sphere>(vec3(-1, 0, 0), 0.5f, std::make_shared<dielectric>(1.5f)));
	wrld.add(std::make_shared<sphere>(vec3(-1, 0, 0), 0.45f, std::make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.8f, 0.2f, 0.1f)))));
	wrld.add(std::make_shared<sphere>(vec3(0, 0, 0.0f), 0.02f, std::make_shared<lambertian>(std::make_shared<constant_texture>(vec3(0.8f)), vec3(10.0f))));
	wrld.add(std::make_shared<sphere>(vec3(1, 0, 0), 0.5f, std::make_shared<metallic>(vec3(0.8f, 0.6f, 0.2f), 0.2f)));
	wrld.add(std::make_shared<sphere>(vec3(0, -100.5f, 0), 100.0f, std::make_shared<lambertian>(std::make_shared<checker_texture>(vec3(1.0f), vec3(0.0f), vec3(10.0f)))));
	wrld.add(std::make_shared<mesh>(verts, idx, std::make_shared<metallic>(vec3(1.0f), 0.0f)));

	wrld.compile();

	if (config.NTHREADS == NTHREAD_AUTO) {
		config.NTHREADS = std::thread::hardware_concurrency();
	}
	printf("RESOLUTION = %ux%u\n", config.WIDTH, config.HEIGHT);
	printf("SAMPLES = %u\n", config.NSAMPLES);
	printf("MAX_DEPTH = %u\n", config.MAX_DEPTH);
	printf("THREADS = %u\n", config.NTHREADS);
	printf("TILE SIZE = %u\n", config.TILE_SIZE);

	framebuffer accumulator(config.WIDTH, config.HEIGHT);

	std::vector<tile> tiles = create_tiles(accumulator, config.TILE_SIZE, config.TILE_SIZE);
	{
		ThreadPool pool(config.NTHREADS);

		volatile std::atomic_size_t work = 0;
		size_t total_work = config.WIDTH * config.HEIGHT * config.NSAMPLES;
		float total_work_inv = 100.0f / total_work;

		for (auto &ctile : tiles) {
			pool.enqueue([&] {
				for (uint s = 0; s < config.NSAMPLES; s++) {
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
