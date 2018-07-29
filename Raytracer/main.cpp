/**
 * Ray tracer based on Peter Shirley's book, "Ray Tracing in One Weekend".
 *
 * The author points out what is created is actually a path tracer:
 * https://en.wikipedia.org/wiki/Path_tracing
 * https://chunky.llbit.se/path_tracing.html
 *
 * Creates a world of spheres with different material properties: diffuse ("normal"), metal, and
 * glass. Ray (path) traces the world and writes the results to a Portable PixMap (.ppm) file.
 *
 * While the book is a great introduction to ray tracing, the code is a bit terse (e.g., one letter
 * variable names, no encapsulation, etc.) so I re-wrote the book's code, partially to make sure
 * that I was reading and understanding the code presented, and partially to better match my own
 * coding style. I'm leaving the use of raw pointers in because fixing everything is not a fun use
 * of my time....
 *
 * Original source code:
 * https://github.com/petershirley/raytracinginoneweekend
 *
 * Created by John Koszarek on 7/2/18.
 */

#include <cstdlib>
#include <ctime>
#include <cfloat>
#include <fstream>
#include <iostream>
#include <random>

#include "Camera.h"
#include "HitableObject.h"
#include "HitableCollection.h"
#include "Material.h"
#include "Ray.h"
#include "Sphere.h"
#include "Vec3.h"

const char* IMAGE_PATH = "/Users/john/Dev/Raytracing/Raytracer/image.ppm";

const Vec3 BLACK(0.0f, 0.0f, 0.0f);
const Vec3 BLUE(0.5f, 0.7f, 1.0f);
const Vec3 WHITE(1.0f, 1.0f, 1.0f);

std::random_device rndNbrGenerator;
std::default_random_engine rndNbrEngine(rndNbrGenerator());
std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

Vec3 calculateColor(const Ray &r, HitableCollection* world, int depth)
{
    HitableProperties properties;

    if (world->hit(r, 0.00001, FLT_MAX, properties)) {
        Ray scatteredRay;
        Vec3 rayAttenuation;

        if (depth < 50 && properties.material->scatter(r, properties, scatteredRay, rayAttenuation)) {
            return rayAttenuation * calculateColor(scatteredRay, world, depth + 1);
        }
        else {
            return BLACK;
        }
    }
    else {  // blended_value = (1 - t) * start_value + t * end_value; t goes from 0 to 1
        Vec3 unitDirection = Vec3::unitVector(r.direction());
        float t = 0.5 * (unitDirection.y() + 1.0);
        Vec3 blendedColorValue = ((1.0 - t) * WHITE) + (t * BLUE);
        return blendedColorValue;
    }
}

// Creates a world of spheres with different material properties: diffuse ("normal"), metal, and
// glass.
void populateRandomWorld(HitableCollection* const world)
{
    const int START_X_INDEX = -2;   // -11;
    const int END_X_INDEX = 2;      // 10
    const int START_Y_INDEX = -2;   // -11;
    const int END_Y_INDEX = 2;      // 10

    world->add(new Sphere(Vec3(0.0f, -1000.0f, 0.0f), 1000.0f, new Lambertian(Vec3(0.5f, 0.5f, 0.5f))));

    for (int x = START_X_INDEX; x <= END_X_INDEX; x++) {
        for (int y = START_Y_INDEX; y <= END_Y_INDEX; y++) {
            Vec3 center(x + 0.9f * distribution(rndNbrEngine), 0.2f, y + 0.9f * distribution(rndNbrEngine));
            if ((center - Vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f) {
                float materialType = distribution(rndNbrEngine);
                if (materialType < 0.8f) {
                    //std::cout << "Adding diffuse sphere" << std::endl;
                    float r = distribution(rndNbrEngine) * distribution(rndNbrEngine);
                    float g = distribution(rndNbrEngine) * distribution(rndNbrEngine);
                    float b = distribution(rndNbrEngine) * distribution(rndNbrEngine);
                    world->add(new Sphere(center, 0.2f, new Lambertian(Vec3(r, g, b))));
                }
                else if (materialType < 0.95f) {
                    //std::cout << "Adding metal sphere" << std::endl;
                    float r = distribution(rndNbrEngine);
                    float g = distribution(rndNbrEngine);
                    float b = distribution(rndNbrEngine);
                    float bluriness = distribution(rndNbrEngine);
                    world->add(new Sphere(center, 0.2f, new Metal(
                            Vec3(0.5f * (1.0f + r), 0.5f * (1.0f + g), 0.5f * (1.0f + b)),
                            0.5f * bluriness)));
                }
                else {
                    //std::cout << "Adding glass sphere" << std::endl;
                    world->add(new Sphere(center, 0.2f, new Dielectric(1.5f)));
                }
            }
        }
    }

    world->add(new Sphere(Vec3(0.0f, 1.0f, 0.0f), 1.0f, new Dielectric(1.5f)));
    world->add(new Sphere(Vec3(-4.0f, 1.0f, 0.0f), 1.0f, new Lambertian(Vec3(0.4f, 0.2f, 0.1f))));
    world->add(new Sphere(Vec3(4.0f, 1.0f, 0.0f), 1.0f, new Metal(Vec3(0.7f, 0.6f, 0.5f), 0.0f)));
}

/**
 * Creates a world of spheres with different material properties: diffuse ("normal"), metal, and
 * glass. Ray (path) traces the world and writes the results to a Portable PixMap (.ppm) file.
 */
int main(int argc, const char* argv[])
{
    clock_t beginTime = clock();
    srand(static_cast<uint>(time(0)));

    const uint imageWidth = 1200;
    const uint imageHeight = 800;
    const uint nbrOfSamples = 5; // 500;

    // Create the camera.
    Vec3 lookFrom(13.0f, 2.0f, 3.0f);
    Vec3 lookAt(0.0f, 0.0f, 0.0f);
    float aperture = 0.1f;
    float distanceToFocusPlane = 10.0f;
    Camera camera(lookFrom, lookAt, Vec3(0.0f, 1.0f, 0.0f), 20.0f, float(imageWidth) / float(imageHeight),
                  aperture, distanceToFocusPlane);

    // Create the world of spheres.
    std::cout << "Make world... " << std::flush;
    HitableCollection* world = new HitableCollection();
    populateRandomWorld(world);
    std::cout << "World complete" << std::endl;

    // Open the the .ppm file.
    std::ofstream imageFile;
    imageFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        imageFile.open(IMAGE_PATH);
    }
    catch(std::ios_base::failure& e) {
        std::cerr << "Raytracer: " << IMAGE_PATH << " " << e.what() << " error code: " << e.code() << std::endl;
        exit(EXIT_FAILURE);
    }
    imageFile << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

    // Write the ray traced image to the .ppm file.
    std::cout << "Rendering... " << std::flush;
    for (int j = imageHeight - 1; j >= 0; --j) {
        for (int i = 0; i < imageWidth; ++i) {
            Vec3 color(0.0f, 0.0f, 0.0f);
            for (int s = 0; s < nbrOfSamples; ++s) {
                float u = (i + distribution(rndNbrEngine)) / float(imageWidth);
                float v = (j + distribution(rndNbrEngine)) / float(imageHeight);

                Ray r = camera.calculateRay(u, v);
                color += calculateColor(r, world, 0);
            }
            color /= float(nbrOfSamples);
            color = Vec3(sqrtf(color[0]), sqrtf(color[1]), sqrtf(color[2]));    // gamma correction

            int red = int(255.99999f * color[0]);
            int green = int(255.99999f * color[1]);
            int blue = int(255.99999f * color[2]);

            imageFile << red << " " << green << " " << blue << "\n";
        }
    }
    std::cout << "Render complete" << std::endl;

    // Close the file and destroy the world.
    imageFile.close();
    delete world;

    std::cout << "Elapsed time: " << (double(clock() - beginTime) / CLOCKS_PER_SEC) << " seconds" << std::endl;

    return EXIT_SUCCESS;
}
