/*
 * The scene to be rendered
 */
#include "distance.glsl"
#include "intersection.glsl"

uniform vec4[1014] uBranches;
uniform uint uNumBranches;

float sdScene(vec3 p) {
    float dist = INF;
    for (uint i = 0; i < uNumBranches; i++) {
        dist = min(dist, sdBranch(p, uBranches[i * 2], uBranches[i * 2 + 1]));
    }
    return dist;
}

float iScene(vec3 ro, vec3 rd) {
    vec2 hitAABB = iAABB(ro, rd, uAABBCenter, uAABBSize);
    if (isinf(hitAABB.x)) return INF;
    else {
        float hitTree = INF;
        for (uint i = 0; i < uNumBranches; i++) {
            vec4 hitBranch = iBranch(ro, rd, uBranches[i * 2], uBranches[i * 2 + 1]);
            hitTree = min(hitTree, hitBranch.x);
        }
        return hitTree;
    }
}