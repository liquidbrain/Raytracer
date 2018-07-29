#pragma once

#include "Vec3.h"

class Material;
class Ray;

struct HitableProperties
{
    float t;
    Vec3 p;
    Vec3 normal;
    Material* material;
};

/**
 * Base class for the objects that can be hit with a ray.
 */
class HitableObject
{
public:
    virtual ~HitableObject() { }

    virtual bool hit(const Ray& r, float t_min, float t_max, HitableProperties& properties) const = 0;
};
