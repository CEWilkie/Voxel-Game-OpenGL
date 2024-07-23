//
// Created by cew05 on 11/07/2024.
//

#ifndef UNTITLED7_MODELSTRUCTS_H
#define UNTITLED7_MODELSTRUCTS_H

#include <iostream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ModelTransformations.h"

struct Plane {
    glm::vec3 normal {};
    float distance {};

    void SetPlane(const glm::vec3& _pos, const glm::vec3& _normal) {
        normal = glm::normalize(_normal);
        distance = glm::dot(normal, _pos);
    }

    [[nodiscard]] float DistToPlane(const glm::vec3& _vertex) const {
        // Signed Distance from a given point (_vertex) to the plane
        return glm::dot(normal, _vertex) - distance;
    }
};

struct Frustrum {
    // INDEX SIDE : |0 LEFT | 1 RIGHT | 2 BOTTOM | 3 TOP | 4 NEAR | 5 FAR
    Plane planes[6] {};
};

struct Vertex {
    glm::vec3 position {};
    glm::vec2 textureIndex {};
    glm::vec2 texture {};
    glm::vec3 color {};
};

struct BoundingVolume {
    enum FRUSTRUM : int{
        OUTSIDE, INTERSECTING, IN,
    };

    [[nodiscard]] virtual int InFrustrum(const Frustrum& _frustrum, const Transformation& _transformation) const {
        return IN;
    }
};

struct SphereBounds : public BoundingVolume {
    glm::vec3 centre {0.0f, 0.0f, 0.0f};
    float radius {0.0f};

    SphereBounds(const glm::vec3& _centre, float _radius) : BoundingVolume() {
        centre = _centre;
        radius = _radius;
    }

    [[nodiscard]] int InFrustrum(const Frustrum& _camFrustrum, const Transformation& _transformation) const override {
        // move sphere centre to model centre by using the model's transformation matrix
        glm::vec3 globalCentre = { _transformation.GetModelMatrix() * glm::vec4(centre, 1.0f)};

        // Get the largest scale of the object to use for radius
        glm::vec3 globalScale = _transformation.GetGlobalScale();
        float maxScale = std::max(std::max(globalScale.x, globalScale.y), globalScale.z);

        // Create a temp sphere struct using this global positioning info
        SphereBounds globalSphere(globalCentre, radius * maxScale * 0.5f);

        // test temp sphere in each frustrum plane
        bool inOrInFrontOfPlane;
        for (const auto& plane : _camFrustrum.planes) {
            float dist = plane.DistToPlane(globalSphere.centre);
            inOrInFrontOfPlane = (dist > -globalSphere.radius);

            // If sphere is behind plane, sphere is not in view
            if (!inOrInFrontOfPlane) {
                return OUTSIDE;
            }
        }

        // no checks failed
        return IN;
    }
};

inline SphereBounds GenerateSphere(const std::vector<Vertex>& _verticies)
{
    auto minAABB = glm::vec3(std::numeric_limits<float>::max());
    auto maxAABB = glm::vec3(std::numeric_limits<float>::min());
    for (auto& vertex : _verticies) {
        minAABB.x = std::min(minAABB.x, vertex.position.x);
        minAABB.y = std::min(minAABB.y, vertex.position.y);
        minAABB.z = std::min(minAABB.z, vertex.position.z);

        maxAABB.x = std::max(maxAABB.x, vertex.position.x);
        maxAABB.y = std::max(maxAABB.y, vertex.position.y);
        maxAABB.z = std::max(maxAABB.z, vertex.position.z);
    }

    return {(maxAABB + minAABB) * 0.5f, glm::length(minAABB - maxAABB)};
}

struct BoxBounds : public BoundingVolume {
    glm::vec3 centre {0.0f, 0.0f, 0.0f};
    float extent {0.0f};

    BoxBounds(const glm::vec3& _centre, float _extent) : BoundingVolume() {
        centre = _centre;
        extent = _extent;
    }

    [[nodiscard]] int InFrustrum(const Frustrum& _camFrustrum, const Transformation& _transformation) const override {
        glm::vec3 globalCentre = { _transformation.GetModelMatrix() * glm::vec4(centre, 1.0f)};

        // Scaled orientation
        const glm::vec3 right = _transformation.GetRight() * extent;
        const glm::vec3 up = _transformation.GetUp() * extent;
        const glm::vec3 forward = _transformation.GetForward() * extent;

        const float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
                            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
                            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

        const float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
                            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
                            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

        const float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
                            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
                            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));


        const BoxBounds globalAABB(globalCentre, std::max(std::max(newIi, newIj), newIk));

        // test temp sphere in each frustrum plane
        bool inOrInFrontOfPlane;
        for (const auto& plane : _camFrustrum.planes) {
            float r = globalAABB.extent * (std::abs(plane.normal.x) + std::abs(plane.normal.y) + std::abs(plane.normal.z));
            inOrInFrontOfPlane = (-r <= plane.DistToPlane(globalAABB.centre));

            // If sphere is behind plane, sphere is not in view
            if (!inOrInFrontOfPlane) return OUTSIDE;
        }

        // no checks failed
        return IN;
    }
};


#endif //UNTITLED7_MODELSTRUCTS_H