/*
 * The scene to be rendered
 */

struct Ray {
    vec3 origin;
    vec3 direction;
    vec3 inv_direction;
};

struct AABB {
    vec3 min;
    vec3 max;
};

struct Branch {
    vec4 a;
    vec4 b;
};

layout(std140) uniform Tree {
    vec3 uMin;
    uint uNumBranches;
    vec3 uMax;
    uint padding;
    Branch[1024] uBranches;
};

#include "distance.glsl"
#include "intersection.glsl"

float sdScene(vec3 p) {
    float dist = INF;
    for (uint i = 0; i < uNumBranches; i++) {
        Branch b = uBranches[i];
        dist = min(dist, sdBranch(p, b));
    }
    return dist;
}

vec4 iScene(Ray ray) {
    vec2 hitAABB = iAABB(ray, AABB(uMin, uMax));
    if (!isHit(hitAABB)) return NO_HIT;
    else {
        vec4 hitTree = NO_HIT;
        for (uint i = 0; i < uNumBranches; i++) {
            Branch b = uBranches[i];
            vec3 bmin = min(b.a.xyz - b.a.w, b.b.xyz - b.b.w);
            vec3 bmax = max(b.a.xyz + b.a.w, b.b.xyz + b.b.w);
            vec2 hitAABB = iAABB(ray, AABB(bmin, bmax));
            if (!isHit(hitAABB)) continue;
            vec4 hitBranch = iBranch(ray, b);
            if (hitBranch.x < hitTree.x) hitTree = hitBranch;
        }
        return hitTree;
    }
}