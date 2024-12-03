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
#include "tree/model.hpp"

void Swarm::spawn(const Tree::Model& tree, bool replaceHomes) {
    if (replaceHomes) homes.clear();

    if (config.nBoids < homes.size()) {
        homes.erase(homes.begin() + config.nBoids, homes.end());
    } else if (config.nBoids > homes.size()) {
        homes.reserve(config.nBoids);

        auto randBranch = std::uniform_int_distribution<size_t>(0, tree.branches.size() - 1);

        for (size_t i = homes.size(); i < config.nBoids; i++) {
            // Pick a random branch
            const auto branch = Tree::BranchSDF(tree.branches[randBranch(rng)]);
            // Pick random initial home
            auto aabb = AABB(branch.branch.start); aabb.add(branch.branch.end);
            glm::vec3 home = glm::linearRand(vec3(aabb.min), vec3(aabb.max));
            for (int j = 0; j < 10; j++) {
                vec3 step = branch.calcScaledNormal(home);
                home -= step;
                if (glm::length2(step) < 1.e-4f) break;
            }
            homes.push_back(home);
        }
    }

    if (config.nBoids < boids.size()) {
        boids.erase(boids.begin() + config.nBoids, boids.end());
    } else if (config.nBoids > boids.size()) {
        boids.reserve(config.nBoids);
        for (size_t i = boids.size(); i < config.nBoids; i++) {
            boids.emplace_back(homes.at(i), glm::linearRand(0.0f, two_pi<float>()), glm::sphericalRand(1.0f), 0.0f);
        }
    }
}

/* Simple boid flocking implementation.
 * Based on the amazing paper "Steering Behaviors For Autonomous Characters" by Craig W. Reynolds */
void Swarm::update(float dt) {
    auto shuffled(boids);
    std::ranges::shuffle(shuffled, rng);
    // Hash all boids
    SpatialHash hash(config.viewRange);
    // TODO: Limit the number of boids in a cell
    for (Boid b : shuffled) hash.add(b, config.neighborLimit);

    float sqrange = config.viewRange * config.viewRange; // Squaring for faster comparisons
    float viewangle = glm::cos(config.viewAngle); // Cosine of view angle
    float seprange = config.separationRange * config.separationRange; // Squaring for faster comparisons

    for (size_t i = 0; i < boids.size(); i++) {
        Boid& boid = boids.at(i);
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
                if (sqdist < 1.e-4f) continue; // Neighbor is self (or very close)
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

        // Return home
        glm::vec3 home = homes.at(i);
        glm::vec3 homing = glm::vec3(home) - boid.position;
        float distHome = glm::length(homing);
        float moveRadius = boid.excitement;
        if (distHome > moveRadius) force += homing * (distHome - moveRadius) * (distHome - moveRadius) * 0.001f;

        boid.velocity = normalizeNoNaN(boid.velocity * config.forwardFactor + force);

        boid.phase += glm::length(boid.velocity) * dt * config.speed;
        boid.phase = mod(boid.phase, two_pi<float>());

        // Move
        float speed = dt * config.speed;
        if (distHome < 0.1f && moveRadius < 0.1f) speed = 0.0f;
        boid.position = boid.position + boid.velocity * speed;
        boid.excitement = std::max(0.0f, boid.excitement - dt);
    }
}

void Swarm::flee(glm::vec3 ro, glm::vec3 rd) { 
    for (auto& boid : boids) {
        glm::vec3 closest = glm::dot(boid.position - ro, rd) * rd + ro;
        glm::vec3 fleedir = normalizeNoNaN(boid.position - closest);
        glm::vec3 fleeing = fleedir - boid.velocity; // Steer away from the the line
        boid.velocity += fleeing * std::gamma_distribution<float>(2.0f, 1.0f)(rng);
        boid.excitement = std::gamma_distribution<float>(2.0f, 0.2f)(rng);
    }
}

void Swarm::reset() { boids.clear(); }

Swarm::Swarm(SwarmConfig config) : config(config) {}