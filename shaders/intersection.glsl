/*
 * Intersection functions for raycasting
 * Derived from Inigo Quilez (https://iquilezles.org/articles/intersectors)
 */

#define INF 1.0/0.0
#define NO_HIT vec4(INF, 0.0, 0.0, 0.0)
#define NO_HIT2 vec2(INF)

bool hit(float hit) { return !isinf(hit); }
bool hit(vec2 hit) { return !isinf(hit.x); }
bool hit(vec4 hit) { return !isinf(hit.x); }

/*
 * Inigo Quilez, Rounded Cone - Intersection, 2018
 * (https://www.shadertoy.com/view/MlKfzm)
 * Published 2018 by Inigo Quilez under MIT License
 */
vec4 iBranch(vec3 ro, vec3 rd, vec4 a, vec4 b) {
    vec3  ba = b.xyz - a.xyz;
    vec3  oa = ro - a.xyz;
    vec3  ob = ro - b.xyz;
    float rr = a.w - b.w;
    float m0 = dot(ba, ba);
    float m1 = dot(ba, oa);
    float m2 = dot(ba, rd);
    float m3 = dot(rd, oa);
    float m5 = dot(oa, oa);
	float m6 = dot(ob, rd);
    float m7 = dot(ob, ob);
    
    // body
    float d2 = m0 - rr*rr;
    float k2 = d2    - m2*m2;
    float k1 = d2*m3 - m1*m2 + m2*rr*a.w;
    float k0 = d2*m5 - m1*m1 + m1*rr*a.w*2.0 - m0*a.w*a.w;
    float h = k1*k1 - k0*k2;
    if(h < 0.0) return NO_HIT;
    float t = (-sqrt(h) - k1) / k2;
    // if(t < 0.0) return NO_HIT;
    float y = m1 - a.w*rr + t*m2;
    if (y > 0.0 && y < d2) return vec4(t, normalize(d2*(oa + t*rd) - ba*y));

    // caps
    float h1 = m3*m3 - m5 + a.w*a.w;
    float h2 = m6*m6 - m7 + b.w*b.w;
    if (max(h1, h2) < 0.0) return NO_HIT;
    vec4 r = vec4(1e20);
    if (h1 > 0.0) {        
    	t = -m3 - sqrt(h1);
        r = vec4(t, (oa + t*rd) / a.w);
    }
	if (h2 > 0.0) {
    	t = -m6 - sqrt(h2);
        if(t < r.x) r = vec4(t, (ob + t*rd) / b.w);
    }
    return r;
}

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
    if (tN > tF || tF < 0.0) return NO_HIT2; // no intersection
    return vec2(max(tN, 0.0), tF);
}