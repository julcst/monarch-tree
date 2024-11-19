/*
 * Intersection functions for raycasting
 * Derived from Inigo Quilez (https://iquilezles.org/articles/intersectors)
 */

#define INF 1.0/0.0
#define NO_HIT vec4(INF, 0.0, 0.0, 0.0)
#define NO_HIT2 vec2(INF)

bool isHit(float hit) { return !isinf(hit); }
bool isHit(vec2 hit) { return !isinf(hit.x); }
bool isHit(vec4 hit) { return !isinf(hit.x); }

/*
 * Inigo Quilez, Rounded Cone - Intersection, 2018
 * (https://www.shadertoy.com/view/MlKfzm)
 * Published 2018 by Inigo Quilez under MIT License
 */
vec4 iBranch(Ray ray, Branch branch) {
    vec3  ba = branch.b.xyz - branch.a.xyz;
    vec3  oa = ray.origin - branch.a.xyz;
    vec3  ob = ray.origin - branch.b.xyz;
    float rr = branch.a.w - branch.b.w;
    float m0 = dot(ba, ba);
    float m1 = dot(ba, oa);
    float m2 = dot(ba, ray.direction);
    float m3 = dot(ray.direction, oa);
    float m5 = dot(oa, oa);
	float m6 = dot(ob, ray.direction);
    float m7 = dot(ob, ob);
    
    // body
    float d2 = m0 - rr*rr;
    float k2 = d2    - m2*m2;
    float k1 = d2*m3 - m1*m2 + m2*rr*branch.a.w;
    float k0 = d2*m5 - m1*m1 + m1*rr*branch.a.w*2.0 - m0*branch.a.w*branch.a.w;
    float h = k1*k1 - k0*k2;
    if (h < 0.0) return NO_HIT;
    float t = (-sqrt(h) - k1) / k2;
    if (t < 0.0) return NO_HIT;
    float y = m1 - branch.a.w*rr + t*m2;
    if (y > 0.0 && y < d2) return vec4(t, normalize(d2*(oa + t*ray.direction) - ba*y));
    
    // caps
    float h1 = m3*m3 - m5 + branch.a.w*branch.a.w;
    float h2 = m6*m6 - m7 + branch.b.w*branch.b.w;
    if (max(h1, h2) < 0.0) return NO_HIT;
    vec4 r = vec4(1e20);
    if (h1 > 0.0) {        
    	t = -m3 - sqrt(h1);
        r = vec4(t, (oa + t*ray.direction) / branch.a.w);
    }
	if (h2 > 0.0) {
    	t = -m6 - sqrt(h2);
        if(t < r.x) r = vec4(t, (ob + t*ray.direction) / branch.b.w);
    }
    return r;
}

/*
 * Derived from https://iquilezles.org/articles/boxfunctions/
 * Wrong distances in interior but we do not need those for bounding volumes
 * Published 2014 by Inigo Quilez under MIT License
 */
vec2 iAABB(Ray ray, AABB aabb) {
    vec3 t_min = (aabb.min - ray.origin) * ray.inv_direction;
    vec3 t_max = (aabb.max - ray.origin) * ray.inv_direction;
    vec3 t_1 = min(t_min, t_max);
    vec3 t_2 = max(t_min, t_max);
    float t_near = max(t_1.x, max(t_1.y, t_1.z));
    float t_far = min(t_2.x, min(t_2.y, t_2.z));
    return t_near <= t_far && t_far >= 0.0 ? vec2(t_near, t_far) : NO_HIT2;
}