/*
 * The scene to be rendered
 */

float distToScene(vec3 p) {
    return sqrt(dot(p, p)) - 0.5;
}