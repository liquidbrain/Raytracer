#include "Sphere.h"

#include "Material.h"
#include "Ray.h"

/** Creates a new Sphere, taking ownership of the Material object. */
Sphere::Sphere(const Vec3& center, float radius, Material* material)
        : _center(center),
          _radius(radius),
          _material(material)
{ }

Sphere::Sphere(const Sphere& rhs)
        : _center(rhs._center),
          _radius(rhs._radius),
          _material(rhs._material)
{ }

Sphere::Sphere(Sphere&& rhs)
{
    rhs._material = nullptr;
}

Sphere& Sphere::operator=(const Sphere& rhs)
{
    if (this == &rhs) {
        return *this;
    }

    _center = rhs._center;
    _radius = rhs._radius;
    _material = rhs._material;

    return *this;
}

Sphere& Sphere::operator=(Sphere&& rhs)
{
    if (this == &rhs) {
        return *this;
    }

    delete _material;
    _material = rhs._material;
    rhs._material = nullptr;

    return *this;
}

Sphere::~Sphere()
{
    delete _material;
}

/**
 * Returns true if the ray hits this sphere.
 * If the ray does hit the sphere, the properties object us updated for the point at which it does.
 */
bool Sphere::hit(const Ray& r, float t_min, float t_max, HitableProperties& properties) const
{
    Vec3 oc = r.origin() - _center;
    float a = Vec3::dotProduct(r.direction(), r.direction());
    float b = Vec3::dotProduct(oc, r.direction());
    float c = Vec3::dotProduct(oc, oc) - (_radius * _radius);
    float discriminant = (b * b) - (a * c);

    bool didHit = false;

    if (discriminant > 0) {
        float t = (-b - sqrt(discriminant)) / a;
        didHit = hit(r, t_min, t_max, properties, t);
        if (didHit) {
            return didHit;
        }
        else {
            t = (-b + sqrt(discriminant)) / a;
            didHit = hit(r, t_min, t_max, properties, t);
        }
    }

    return didHit;
}

bool Sphere::hit(const Ray& r, float t_min, float t_max, HitableProperties& properties, float t) const
{
    if (t < t_max && t > t_min) {
        properties.t = t;
        properties.p = r.pointAtParameter(properties.t);
        properties.normal = (properties.p - _center) / _radius;
        properties.material = _material;
        return true;
    }
    return false;
}
