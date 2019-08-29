
#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "rng.h"

const float PI = 3.1415826f;

class camera {
    private:
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 origin;
        vec3 front;
        vec3 right;
        vec3 up;

        float lens_radius;
    public:
        camera(vec3 eye, vec3 lookAt, vec3 worldUp, float vfov, float aspect, float aperture, float focal_dist) {
            float theta = vfov * PI/180.f;
            float half_height = tan(theta/2);
            float half_width = aspect * half_height;

            lens_radius = aperture * 0.5f;

            origin = eye;

            front = normalize(lookAt - eye);
            right = normalize(cross(front, worldUp));
            up = normalize(cross(right, front));

            horizontal = 2 * half_width * right * focal_dist;
            vertical = 2 * half_height * up * focal_dist;
            lower_left_corner = front * focal_dist - half_width * right * focal_dist - half_height * up * focal_dist;
        }

        ray get_ray(float u, float v) const { 
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = right * rd.x() + up * rd.y();
            return ray(origin + offset, lower_left_corner + u*horizontal + v*vertical - offset); 
        }
};

#endif // CAMERA_H
