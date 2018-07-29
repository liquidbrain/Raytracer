#pragma once

#include "HitableObject.h"
#include "Ray.h"
#include "Vec3.h"

Vec3 randomPointInUnitSphere();
Vec3 reflect(const Vec3& v, const Vec3& n);
bool refract(const Vec3& v, const Vec3& n, float ni_over_nt, Vec3& refracted);
float schlick(float cosine, float reflectionCoefficient);

/**
 * Base class for the materials this ray tracer uses.
 */
class Material
{
public:
    virtual ~Material() { }

    virtual bool scatter(const Ray &r_in, const HitableProperties &properties,
                         Ray &scatteredRay, Vec3 &rayAttenuation) const = 0;
};

/**
 * This Lambertian material represents an ideal "matte" or diffusely reflecting surface. The
 * apparent brightness of a Lambertian surface to an observer is the same regardless of the
 * observer's angle of view.
 */
class Lambertian final : public Material
{
public:
    /**
     * Creates a new Lambertian material.
     * @param albedo The proportion of light that is reflected away from a surface.
     */
    Lambertian(const Vec3& albedo) : _albedo(albedo) { }

    ~Lambertian() override { }

    bool scatter(const Ray &r_in, const HitableProperties &hitRecord,
            Ray &scatteredRay, Vec3 &rayAttenuation) const override
    {
        Vec3 target = hitRecord.p + hitRecord.normal + randomPointInUnitSphere();
        scatteredRay = Ray(hitRecord.p, target - hitRecord.p);
        rayAttenuation = _albedo;
        return true;
    }

private:
    Vec3 _albedo;
};

/**
 * Metal material.
 */
class Metal final : public Material {
public:
    /**
     * Creates a new metal material.
     * @param albedo The proportion of light that is reflected away from a surface.
     * @param bluriness Reflection blurriness such as that caused by bumps or pits on the material's
     * surface.
     */
    Metal(const Vec3& albedo, float bluriness) : _albedo(albedo)
    {
        if (bluriness < 1) _bluriness = bluriness; else _bluriness = 1.0f;
    }

    ~Metal() override { }

    bool scatter(const Ray &r_in, const HitableProperties &hitRecord,
            Ray &scatteredRay, Vec3 &rayAttenuation) const override
    {
        Vec3 reflected = reflect(Vec3::unitVector(r_in.direction()), hitRecord.normal);
        scatteredRay = Ray(hitRecord.p, reflected + _bluriness * randomPointInUnitSphere());
        rayAttenuation = _albedo;
        return (Vec3::dotProduct(scatteredRay.direction(), hitRecord.normal) > 0);
    }

private:
    Vec3 _albedo;
    float _bluriness;
};

/**
 * This dielectric material represents clear surfaces such as water, glass, and diamond.
 * Example reflective indexes: air 1, water 1.3, glass 1.5 - 1.7, diamond 1.8 - 2.4.
 */
class Dielectric final : public Material {
public:
    /**
     * Creates a new dielectric material.
     * @param refractiveIndex The amount that light is bent when traveling from one medium to
     * another.
     */
    Dielectric(float refractiveIndex) : _refractiveIndex(refractiveIndex) { }

    ~Dielectric() override { }

    bool scatter(const Ray &r_in, const HitableProperties &hitRecord,
            Ray &scatteredRay, Vec3 &rayAttenuation) const override
    {
        rayAttenuation = Vec3(1.0f, 1.0f, 1.0f);   // always 1 for now: a glass surface absorbs nothing

        Vec3 outwardNormal;
        Vec3 refracted;
        Vec3 reflected = reflect(r_in.direction(), hitRecord.normal);
        float ni_over_nt;
        float cosine;
        float probabilityOfReflection;

        if (Vec3::dotProduct(r_in.direction(), hitRecord.normal) > 0) {
            outwardNormal = -hitRecord.normal;
            ni_over_nt = _refractiveIndex;
            cosine = Vec3::dotProduct(r_in.direction(), hitRecord.normal) / r_in.direction().length();
            cosine = sqrt(1.0f - _refractiveIndex * _refractiveIndex * (1.0f - cosine * cosine));
        }
        else {
            outwardNormal = hitRecord.normal;
            ni_over_nt = 1.0f / _refractiveIndex;
            cosine = -Vec3::dotProduct(r_in.direction(), hitRecord.normal) / r_in.direction().length();
        }

        if (refract(r_in.direction(), outwardNormal, ni_over_nt, refracted)) {
            probabilityOfReflection = schlick(cosine, _refractiveIndex);
        }
        else {
            probabilityOfReflection = 1.0f;
        }

        if (drand48() < probabilityOfReflection) {
            scatteredRay = Ray(hitRecord.p, reflected);
        }
        else {
            scatteredRay = Ray(hitRecord.p, refracted);
        }

        return true;
    }

private:
    float _refractiveIndex;
};

/**
 * Calculate reflection.
 */
inline Vec3 reflect(const Vec3& v, const Vec3& n)
{
    return v - 2 * Vec3::dotProduct(v, n) * n;
}

/**
 * Calculate refraction.
 */
inline bool refract(const Vec3& v, const Vec3& n, float ni_over_nt, Vec3& refracted)
{
    Vec3 uv = Vec3::unitVector(v);
    float dt = Vec3::dotProduct(uv, n);
    float discriminant = 1 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    else {
        return false;
    }
}

/**
 * Calculate specular reflection of light (i.e., glass has reflectivity that varies with the viewing
 * angle).
 * See Schlick's approximation: https://en.wikipedia.org/wiki/Schlick%27s_approximation
 */
inline float schlick(float cosine, float reflectionCoefficient)
{
    float r0 = (1 - reflectionCoefficient) / (1 + reflectionCoefficient);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

/**
 * Returns a random point from within a unit radius sphere.
 */
inline Vec3 randomPointInUnitSphere()
{
    Vec3 point;

    do {
        float x = drand48();
        float y = drand48();
        float z = drand48();
        point = 2 * Vec3(x, y, z) - Vec3(1.0f, 1.0f, 1.0f);
    }
    while (point.squaredLength() >= 1);

    return point;
}
