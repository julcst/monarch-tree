#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <unordered_map>
#include <vector>

#include "swarm/swarm.hpp"

class SpatialHash {  
    public:
        std::unordered_map<glm::ivec3, std::vector<Boid>> map;
        float radius;
        void add(const Boid& b);
        void add(const Boid& b, size_t limit);
        //std::generator<const Boid&> iterateNeighbors(const Boid& b);
        void reset(float radius);
        SpatialHash(float radius);
};

/* A simple spatial hash */
inline SpatialHash::SpatialHash(float radius) : radius(radius), map() {}

inline void SpatialHash::add(const Boid& b) {
    const glm::ivec3 cell = glm::ivec3(b.position / radius);
    map[cell].push_back(b);
}

inline void SpatialHash::add(const Boid& b, size_t limit) {
    const glm::ivec3 cell = glm::ivec3(b.position / radius);
    map[cell].reserve(limit);
    if (map[cell].size() < limit) map[cell].push_back(b);
}

// TODO: Use C++23 generator
// inline void SpatialHash::iterateNeighbors(const Boid& b, const std::function<void(const Boid)>& callback) {
//     const glm::ivec3 gridPosition = glm::floor(b.position / radius);
//     for (int x = -1; x <= 1; ++x)
//     for (int y = -1; y <= 1; ++y)
//     for (int z = -1; z <= 1; ++z) {
//         const glm::ivec3 neighborCell = gridPosition + glm::ivec3(x, y, z);
//         for (const Boid& neighbor : map[neighborCell]) callback(neighbor);
//     }
// }

inline void SpatialHash::reset(float r) { radius = r; map.clear(); }