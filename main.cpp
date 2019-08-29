#include <iostream>
#include <string>
#include <fstream>
#include "ray.h"
#include "sphere.h"
#include "hitable.h"
#include "world.h"
#include "material.h"
#include "camera.h"

#include "rng.h"

vec3 color(const ray& r, hitable* world, int depth) {
    hit_record rec;
    if (depth == 0) {
        return vec3(0.0f);
    }
    if (world->hit(r, 0.001f, MAXFLOAT, rec)) {
        vec3 attenuation;
        ray scattered;
        if (rec.mat->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth-1);
        } else {
            return vec3(0.0f);
        }
    } else {
        vec3 unit_dir = normalize(r.direction());
        float t = 0.5f*(unit_dir.y() + 1.0f);
        return (1.0f - t)*vec3(1.0f) + t*vec3(0.5f, 0.7f, 1.0f);
    }
}

int main(int argc, char* argv[]) {
    int WIDTH = 200, HEIGHT = 100;
    int NSAMPLES = 10;

    std::ios_base::sync_with_stdio(false);
    std::cout.tie(nullptr);

    for (int i = 0; i < argc; i++) {
        if ("-s" == std::string(argv[i])) {
            NSAMPLES = atoi(argv[++i]);
        } else if ("-r" == std::string(argv[i])) {
            std::cerr << "RESOLUTION " << argv[i+1] << " " << argv[i+2] << std::endl;
            WIDTH = atoi(argv[++i]);
            HEIGHT = atoi(argv[++i]);
        }
    }

    vec3 eye = vec3(1);
    vec3 target = vec3(0, 0, -1);
    float focal_length = (target - eye).length();
    camera cam(eye, target, vec3(0,1,0), 60.0f, (float)WIDTH/(float)HEIGHT, 0.1f, focal_length);

    world w;
    w.add<sphere>(vec3(-1,0,-1), 0.5f, new dielectric(1.5f));
    w.add<sphere>(vec3(0,0,-1), 0.5f, new lambertian(vec3(0.1f, 0.2f, 0.5f)));
    w.add<sphere>(vec3(1,0,-1), 0.5f, new metallic(vec3(0.8f, 0.6f, 0.2f), 0.2f));
    w.add<sphere>(vec3(0,-100.5f, -1), 100.0f, new lambertian(vec3(0.8f, 0.8f, 0.0f)));

    std::cout << "P3 " << WIDTH << ' ' << HEIGHT << " 255\n";
    for (int j = HEIGHT-1; j >= 0; j--) {
        for (int i = 0; i < WIDTH; i++) {
            vec3 col(0.0f);
            for (int s = 0; s < NSAMPLES; s++) {
                float u = (i + rng())/(float)WIDTH;
                float v = (j + rng())/(float)HEIGHT;
                ray r = cam.get_ray(u, v);
                col += color(r, &w, NSAMPLES);
            }
            col /= NSAMPLES;
            float ir = (int)(255.99f*(col.r()));
            float ig = (int)(255.99f*(col.g()));
            float ib = (int)(255.99f*(col.b()));
            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    return 0;
}
