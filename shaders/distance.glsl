/*
 * Signed distance functions used for raymarching
 * Derived from Inigo Quilez (https://iquilezles.org/articles/distfunctions)
 */

float dot2(vec3 v) { return dot(v, v); }

/*
 * Inigo Quilez, Round Cone - distance, 2018
 * (https://www.shadertoy.com/view/tdXGWr)
 * Published 2018 by Inigo Quilez under MIT License
 */
float sdBranch(vec3 p, Branch branch) {
    // sampling independent computations (only depend on shape)
    vec3  ba = branch.b.xyz - branch.a.xyz;
    float l2 = dot(ba, ba);
    float rr = branch.a.w - branch.b.w;
    float a2 = l2 - rr*rr;
    float il2 = 1.0/l2;
        
    // sampling dependant computations
    vec3 pa = p - branch.a.xyz;
    float y = dot(pa, ba);
    float z = y - l2;
    float x2 = dot2( pa*l2 - ba*y );
    float y2 = y*y*l2;
    float z2 = z*z*l2;

    // single square root!
    float k = sign(rr)*rr*rr*x2;
    if( sign(z)*a2*z2>k ) return  sqrt(x2 + z2)        *il2 - branch.b.w;
    if( sign(y)*a2*y2<k ) return  sqrt(x2 + y2)        *il2 - branch.a.w;
                          return (sqrt(x2*a2*il2)+y*rr)*il2 - branch.a.w;
}

/*
 * Derived from https://iquilezles.org/articles/boxfunctions/
 * Wrong distances in interior but we do not need those for bounding volumes
 * Published 2016 by Inigo Quilez under MIT License
 */
float sqAABB(vec3 p, vec3 center, vec3 size) {
    vec3 d = max(abs(p - center) - size, 0.0);
    return dot(d, d);
}