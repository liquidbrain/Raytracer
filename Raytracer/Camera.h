#pragma once

#include "Ray.h"
#include "Vec3.h"

class Camera {
public:
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
    Camera(const Vec3& lookFrom, const Vec3& lookAt, const Vec3& up,
           float vFov, float aspectRatio, float aperture, float focusDistance);

    /** Calculates a ray for the supplied position. */
    Ray calculateRay(float s, float t);

private:
    Vec3 origin;
    Vec3 lowerLeftCorner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 u, v, w;

    float lensRadius;

    /** Simulates the camera's the lens. This allows the camera to support depth of field. */
    Vec3 randomPointInUnitDisk();
};
