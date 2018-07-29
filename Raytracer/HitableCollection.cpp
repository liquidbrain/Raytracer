#include "HitableCollection.h"

/** Creates a new HitableCollection, taking ownership of any HitableObjects added to it. */
HitableCollection::HitableCollection() { }

HitableCollection::~HitableCollection()
{
    for (auto i = _list.rbegin(); i != _list.rend(); ++i) {
        delete *i;
    }
}

/** Adds the supplied object to this collection (and also takes ownership of it.) */
void HitableCollection::add(HitableObject* object)
{
    _list.push_back(object);
}

/** Returns true if the ray hits this object. */
bool HitableCollection::hit(const Ray& r, float t_min, float t_max, HitableProperties& properties) const
{
    bool didHit = false;
    float nearestHitSoFar = t_max;
    HitableProperties tempProperties;

    for (const auto& object : _list) {
        if (object->hit(r, t_min, nearestHitSoFar, tempProperties)) {
            didHit = true;
            nearestHitSoFar = tempProperties.t;
            properties = tempProperties;
        }
    }

    return didHit;
}
