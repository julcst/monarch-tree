/*
 * A tree generator using particle systems
 * Copyright © Julian Stamm 2023
 */
#include "model.hpp"

#include "framework/aabb.hpp"

#include <glm/glm.hpp>

#include <limits>
#include <vector>

#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace glm;
using namespace Tree;

void Model::addBranch(Branch branch) {
    aabb.add(branch.start);
    aabb.add(branch.end);
    branches.push_back(branch);
}

/*
 * Inigo Quilez, Round Cone - distance, 2018
 * (https://www.shadertoy.com/view/tdXGWr)
 * Published 2018 by Inigo Quilez under MIT License
 */
float sdBranch(vec3 p, Branch branch) {
    // sampling independent computations (only depend on shape)
    vec3  ba = vec3(branch.end) - vec3(branch.start);
    float l2 = dot(ba, ba);
    float rr = branch.start.w - branch.end.w;
    float a2 = l2 - rr*rr;
    float il2 = 1.0/l2;
        
    // sampling dependant computations
    vec3 pa = p - vec3(branch.start);
    float y = dot(pa, ba);
    float z = y - l2;
    vec3 x = pa*l2 - ba*y;
    float x2 = dot(x, x);
    float y2 = y*y*l2;
    float z2 = z*z*l2;

    // single square root!
    float k = sign(rr)*rr*rr*x2;
    if( sign(z)*a2*z2>k ) return  sqrt(x2 + z2)        *il2 - branch.end.w;
    if( sign(y)*a2*y2<k ) return  sqrt(x2 + y2)        *il2 - branch.start.w;
                          return (sqrt(x2*a2*il2)+y*rr)*il2 - branch.start.w;
}

float Model::calcSignedDistance(vec3 position) {
    float dist = 1000.f;//std::numeric_limits<float>::infinity();
    for (Branch branch : branches) {
        dist = min(dist, sdBranch(position, branch));
    }
    return dist;
}