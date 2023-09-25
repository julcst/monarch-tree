/*
 * Intersection functions for raycasting
 * Derived from Inigo Quilez (https://iquilezles.org/articles/intersectors)
 */

/*
 * Derived from https://iquilezles.org/articles/boxfunctions/
 * Wrong distances in interior but we do not need those for bounding volumes
 * Published 2014 by Inigo Quilez under MIT License
 */
vec2 iAABB(vec3 ro, vec3 rd, vec3 center, vec3 size) {
    ro -= center;
    vec3 m = 1.0 / rd;
    vec3 n = m * ro;
    vec3 k = abs(m) * size;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max(max(t1.x, t1.y), t1.z);
    float tF = min(min(t2.x, t2.y), t2.z);
    if (tN > tF || tF < 0.0) return vec2(-1.0); // no intersection
    return vec2(max(tN, 0.0), tF);
}