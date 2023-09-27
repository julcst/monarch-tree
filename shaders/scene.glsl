/*
 * The scene to be rendered
 */
#define INF 1./0.

struct Branch {
    vec4 start;
    vec4 end;
};

uniform vec4[1000] uBranches;
uniform uint uNumBranches;
 
float distToScene(vec3 p) {
    float dist = INF;
    for (uint i = 0; i < uNumBranches; i++) {
        dist = min(dist, sdBranch(p, uBranches[i * 2], uBranches[i * 2 + 1]));
    }
    return dist;
}