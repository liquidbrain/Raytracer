#include "Camera.h"

#include <cmath>

/**
 * Creates a new camera.
 * @param lookFrom Camera position.
 * @param lookAt The position that the camera is looking at.
 * @param up Camera roll (sideways tilt).
 * @param vFov Vertical field of view (top to bottom) in degrees.
 * @param aspectRatio Film aspect ratio.
 * @param aperture Camera opening through which light travels. Increasing the aperture decreases
 * the depth of field (increases defocus blur).
 * @param focusDistance Distance between the lens and the film plane.
 */
Camera::Camera(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up,
               float vFov, float aspectRatio, float aperture, float focusDistance)
{
    lensRadius = aperture / 2;

    float theta = vFov * M_PI / 180;
    float halfHeight = tan(theta / 2);
    float halfWidth = aspectRatio * halfHeight;

    w = Vec3::unitVector(lookFrom - lookAt);
    u = Vec3::unitVector(Vec3::crossProduct(up, w));
    v = Vec3::crossProduct(w, u);

    origin = lookFrom;
    lowerLeftCorner = origin - halfWidth * focusDistance * u - halfHeight * focusDistance * v - focusDistance * w;
    horizontal = 2 * halfWidth*focusDistance * u;
    vertical = 2 * halfHeight*focusDistance * v;
}

/** Calculates a ray for the supplied position. */
Ray Camera::calculateRay(float s, float t)
{
    Vec3 randomPointOnLens = lensRadius * randomPointInUnitDisk();
    Vec3 offset = u * randomPointOnLens.x() + v * randomPointOnLens.y();
    return Ray(origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset);
}

/** Simulates the camera's the lens. This allows the camera to support depth of field. */
Vec3 Camera::randomPointInUnitDisk()
{
    Vec3 point;

    do {
        point = 2 * Vec3(drand48(), drand48(), 0.0f) - Vec3(1.0f, 1.0f, 0.0f);
    }
    while (Vec3::dotProduct(point, point) >= 1);

    return point;
}
