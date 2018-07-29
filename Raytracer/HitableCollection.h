#pragma once

#include <vector>

#include "HitableObject.h"
#include "Ray.h"

class HitableCollection final
{
public:
    /** Creates a new HitableCollection, taking ownership of any HitableObjects added to it. */
    HitableCollection();

    // Not going to fix the original code base design decision to use raw pointers -- will just
    // disallow copying, etc. Also with this simple ray tracer, there should only be one collection
    // (world) of objects within it.
    HitableCollection(const HitableCollection& rhs) = delete;
    HitableCollection(HitableCollection&& rhs) = delete;
    HitableCollection& operator=(const HitableCollection& rhs) = delete;
    HitableCollection& operator=(HitableCollection&& rhs) = delete;

    ~HitableCollection();

    /** Adds the supplied object to this collection (and also takes ownership of it.) */
    void add(HitableObject* object);

    /** Returns true if the ray hits this object. */
    bool hit(const Ray& r, float t_min, float t_max, HitableProperties& properties) const;

private:
    std::vector<HitableObject*> _list;
};
