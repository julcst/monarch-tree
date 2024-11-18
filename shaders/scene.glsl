/*
 * The scene to be rendered
 */
#include "distance.glsl"
#include "intersection.glsl"

uniform Tree {
    vec3 uAABBCenter;
    uint uNumBranches;
    vec3 uAABBSize;
    uint padding;
    vec4[1014] uBranches;
};

float sdScene(vec3 p) {
    float dist = INF;
    for (uint i = 0; i < uNumBranches; i++) {
        dist = min(dist, sdBranch(p, uBranches[i * 2], uBranches[i * 2 + 1]));
    }
    return dist;
}

vec4 iScene(vec3 ro, vec3 rd) {
    vec2 hitAABB = iAABB(ro, rd, uAABBCenter, uAABBSize);
    if (!isHit(hitAABB)) return NO_HIT;
    else {
        vec4 hitTree = NO_HIT;
        for (uint i = 0; i < uNumBranches; i++) {
            vec4 hitBranch = iBranch(ro, rd, uBranches[i * 2], uBranches[i * 2 + 1]);
            if (hitBranch.x < hitTree.x) hitTree = hitBranch;
        }
        return hitTree;
    }
}