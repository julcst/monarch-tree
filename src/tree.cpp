/*
 * A tree generator using particle systems
 * Copyright © Julian Stamm 2023
 */
#include "tree.hpp"

#include "framework/aabb.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>

#include <queue>
#include <vector>

using namespace glm;
using namespace Tree;

Generator::Generator(Config config, unsigned int seed) : config(config), seed(seed) {}

float area(float radius) { return pi<float>() * radius * radius; }
float radius(float area) { return sqrt(area / pi<float>()); }
float scaleArea(float r, float f) { return r * sqrt(f); } /* Result of radius(area(r)*f) */

Particle growBranch(Config config, Particle previous, vec3 position, float ratio) {
    vec3 growDir = normalize(
        config.forwardFactor * previous.momentum +
        config.randomFactor * sphericalRand(1.f) +
        config.upFactor * config.up);
    Particle newParticle = Particle { position, growDir, scaleArea(previous.radius, ratio), previous.age + config.segmentLength };
    return newParticle;
}

bool compare(Particle a, Particle b) { return a.age > b.age; }

Model Generator::generate(bool fixSeed) {
    if (!fixSeed) seed += 1;
    srand(seed);

    AABBBuilder aabbBuilder = AABBBuilder(config.root);
    std::vector<Branch> branches;
    std::priority_queue<Particle, std::vector<Particle>, decltype(&compare)> particles(compare);
    particles.push(Particle { config.root, normalize(config.up), config.radius, 0.0f });

    for (unsigned int i = 0; i < config.nBranches; i++) {
        // Get youngest particle
        Particle particle = particles.top();
        particles.pop();
        // Grow it
        vec3 endpoint = particle.position + particle.momentum * config.segmentLength;

        // Grow branches from it
        float r = 0.f;
        for (unsigned int i = 0; i < config.branching; i++) {
            Particle newParticle = growBranch(config, particle, endpoint, 0.5f);
            particles.push(newParticle);
            r = max(r, newParticle.radius);
        }

        // Add branch to tree
        Branch branch = Branch { vec4(particle.position, particle.radius), vec4(endpoint, r) };
        branches.push_back(branch);
        aabbBuilder.add(branch.start);
        aabbBuilder.add(branch.end);
    }

    return Model { branches, aabbBuilder.build() };
}