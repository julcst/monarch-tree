#include "swarm.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <random>

#include "util.hpp"
#include "swarm/spatialhash.hpp"

/* Simple boid flocking implementation.
 * Based on the amazing paper "Steering Behaviors For Autonomous Characters" by Craig W. Reynolds */
void Swarm::update(float dt, glm::vec3 ro, glm::vec3 rd, bool flee) {
    if (config.nBoids < boids.size()) {
        boids.erase(boids.begin() + config.nBoids, boids.end());
    } else if (config.nBoids > boids.size()) {
        boids.reserve(config.nBoids);
        for (unsigned int i = boids.size(); i < config.nBoids; i++) {
            boids.emplace_back(glm::ballRand(config.spawn.w) + glm::vec3(config.spawn), glm::linearRand(0.0f, two_pi<float>()), glm::sphericalRand(1.0f), 0.0f);
        }
    }

    std::ranges::shuffle(boids, rng);
    // Hash all boids
    SpatialHash hash(config.viewRange);
    // TODO: Limit the number of boids in a cell
    for (Boid b : boids) hash.add(b, 128);

    float sqrange = config.viewRange * config.viewRange; // Squaring for faster comparisons
    float viewangle = glm::cos(config.viewAngle); // Cosine of view angle
    float seprange = config.separationRange * config.separationRange; // Squaring for faster comparisons

    for (auto& boid : boids) {
        glm::vec3 separation(0.f), alignment(0.f), cohesion(0.f);
        float nSeparation = 0.f, nCohesion = 0.f;

        // Iterate over neighbors
        const glm::ivec3 gridPosition = glm::floor(boid.position / config.viewRange);
        for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
        for (int z = -1; z <= 1; ++z) {
            const glm::ivec3 neighborCell = gridPosition + glm::ivec3(x, y, z);
            for (const Boid& other : hash.map[neighborCell]) {
                float sqdist = glm::distance2(boid.position, other.position); // Squared distance between boid and neighbor
                if (sqdist > sqrange) continue;; // Neighbor is out of visual range
                if (sqdist < 0.0001f) continue; // Neighbor is self (or very close)
                glm::vec3 dir = (other.position - boid.position) / glm::sqrt(sqdist); // Direction from boid to neighbor
                if (sqdist < seprange) { // Neighbor is too close => separate
                    separation -= dir;
                    nSeparation++;
                } else if (glm::dot(boid.velocity, dir) > viewangle) { // Neighbor is in view => align
                    alignment += other.velocity;
                    cohesion += dir;
                    nCohesion++;
                }
            }
        }

        // Randomization
        glm::vec3 force = (glm::ballRand(0.5f) + boid.velocity * 0.5f) * config.randomFactor; // Sphere in front of boid

        // Separation
        if (nSeparation > 0) {
            separation /= nSeparation;
            force += separation * config.separationFactor;
        }

        // Cohesion
        if (nCohesion > 0) {
            alignment /= nCohesion;
            cohesion /= nCohesion;
            force += alignment * config.alignmentFactor;
            force += cohesion * config.cohesionFactor;
        }

        // Bounding sphere
        glm::vec3 bounddir = glm::vec3(config.bound) - boid.position;
        float distbound = glm::length(bounddir) - config.bound.w;
        if (distbound > 0.0f) force += bounddir * distbound * 0.001f;

        // Flee from mouse
        // For the flee direction find the closest point on the line
        if (flee) {
            glm::vec3 closest = glm::dot(boid.position - ro, rd) * rd + ro;
            glm::vec3 fleedir = normalizeNoNaN(boid.position - closest);
            glm::vec3 fleeing = fleedir - boid.velocity; // Steer away from the the line
            force += fleeing * config.fleeFactor;
            boid.excitement = glm::gaussRand(1.0f, config.excitementVariance);
        }

        // Attraction to spawn
        float distspawn = glm::length(glm::vec3(config.spawn) - boid.position) - config.spawn.w;
        if (boid.excitement <= config.returnTime) force += (glm::vec3(config.spawn) + glm::ballRand(config.spawn.w) - boid.position);

        // Move
        boid.velocity = normalizeNoNaN(boid.velocity * config.forwardFactor + force);
        boid.phase += glm::length(boid.velocity) * dt * config.speed;
        boid.phase = mod(boid.phase, two_pi<float>());
        if (distspawn > 0.f || boid.excitement > 0.f) boid.position = boid.position + boid.velocity * dt * config.speed * (1.f + boid.excitement * config.excitementFactor);
        boid.excitement = std::max(0.f, boid.excitement - dt * config.excitementDecay);
    }
}

void Swarm::reset() { boids.clear(); }

Swarm::Swarm(SwarmConfig config) : config(config) {}