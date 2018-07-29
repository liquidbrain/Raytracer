#pragma once

#include "HitableObject.h"

class Sphere final : public HitableObject
{
public:
    /** Creates a new Sphere, taking ownership of the Material object. */
    Sphere(const Vec3& center, float radius, Material* material);

    Sphere(const Sphere& rhs);
    Sphere(Sphere&& rhs);

    Sphere& operator=(const Sphere& rhs);
    Sphere& operator=(Sphere&& rhs);

    ~Sphere() override;

    /**
     * Returns true if the ray hits this sphere.
     * If the ray does hit the sphere, the properties object us updated for the point at which it does.
     */
    bool hit(const Ray& r, float t_min, float t_max, HitableProperties& properties) const override;

private:
    Vec3 _center;
    float _radius;
    Material* _material;

    bool hit(const Ray& r, float t_min, float t_max, HitableProperties& properties, float t) const;
};
