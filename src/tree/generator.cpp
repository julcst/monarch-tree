/*
 * A tree generator using particle systems
 * Copyright © Julian Stamm 2023
 */
#include "generator.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>

#include <queue>

using namespace glm;
using namespace Tree;

Generator::Generator(const Config& config, uint seed) : config(config), seed(seed) {}

float area(float radius) { return pi<float>() * radius * radius; }
float radius(float area) { return sqrt(area / pi<float>()); }
float scaleArea(float r, float f) { return r * sqrt(f); } /* Result of radius(area(r)*f) */
bool compare(Particle a, Particle b) { return a.age > b.age; }

void Generator::shuffle() {
    seed += 1;
}

Model Generator::generate() {
    if (!config.fixedSeed) shuffle();
    srand(seed);

    Model model;
    std::priority_queue<Particle, std::vector<Particle>, decltype(&compare)> particles(compare);
    particles.push(Particle { config.root, config.up, config.radius, 0.0f });

    for (uint i = 0; i < config.nBranches; i++) {
        // Get youngest particle
        const auto particle = particles.top();
        particles.pop();
        // Grow it
        const auto endpoint = particle.position + particle.momentum * config.segmentLength;

        const auto normal = model.calcNormal(endpoint);

        // Grow branches from it
        float radius = 0.0f;
        for (uint i = 0; i < config.branching; i++) {

            const auto growDir = normalize(
                config.forwardFactor * particle.momentum +
                config.randomFactor * sphericalRand(1.0f) +
                config.spreadFactor * normal +
                config.upFactor * config.up
            );

            const auto newParticle = Particle {
                endpoint,
                growDir,
                scaleArea(particle.radius, 0.5f),
                particle.age + config.segmentLength
            };
            
            particles.push(newParticle);
            radius = max(radius, newParticle.radius);
        }

        // Add branch to tree
        model.addBranch(Branch(vec4(particle.position, particle.radius), vec4(endpoint, radius)));
    }

    return model;
}