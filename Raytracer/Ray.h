#pragma once

#include "Vec3.h"

class Ray
{
public:
    Ray() { }

    Ray(const Vec3& origin, const Vec3& direction) : _origin(origin), _direction(direction) { }

    Vec3 origin() const    { return _origin; }

    Vec3 direction() const { return _direction; }

    Vec3 pointAtParameter(float t) const
    {
        return _origin + (t * _direction);
    }

private:
    Vec3 _origin;
    Vec3 _direction;
};
