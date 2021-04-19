#include "utility.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

#include <iostream>

// conducting a lerp (linear interpolation) between white and blue
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've hit our recursive depth limit, we haven't hit any light for this ray so we terminate
    if (depth <= 0) {
        return color(0.0, 0.0, 0.0);
    }

    // 0.001 is used instead of 0 to avoid the shadow acne problem and account for rounding errors
    if (world.hit(r, 0.001, infinity, rec)) {
        point3 target = rec.p + rec.normal + random_unit_vector();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    // Image
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 50; // Tune these
    const int max_ray_depth = 25; // Tune these

    // World
    hittable_list world;
    world.add(std::make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(std::make_shared<sphere>(point3(0,-100.5,-1), 100));

    // Camera
    camera cam;

    // Rendering loop
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int row = image_height-1; row >= 0; --row) {
        std::cerr << "\rScanlines remaining: " << row << ' ' << std::flush;
        for (int column = 0; column < image_width; ++column) {
            color pixel_color(0, 0, 0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                // anti-aliasing by sampling samples_per_pixel amount within each pixel and finding the average colour
                double u = (column + random_double()) / (image_width-1);
                double v = (row + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_ray_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}